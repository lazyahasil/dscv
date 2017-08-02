#include "judge_process.hpp"

#include <boost/process/async.hpp>
#include <boost/process/extend.hpp>

namespace bp = boost::process;

namespace dscv
{
	namespace judge
	{
		namespace detail
		{
			std::shared_ptr<JudgeProcessUnit::ChildItem> JudgeProcessUnit::make_child(
				const boost::filesystem::path& program_path,
				boost::process::group& group,
				const boost::process::wenvironment& env,
				boost::asio::io_service& io_service
			)
			{
				auto item = std::make_shared<ChildItem>(
					io_service,
					group,
					program_path,
					bp::std_in < data_.stdin_str,
					bp::start_dir = data_.dir_overload,
					env,
					bp::on_exit = [this](int exit, const std::error_code& ec_in) { _handle_exit(exit, ec_in); }
				);

				item_wptr_ = item;

				return item;
			}

			void JudgeProcessUnit::post_receive()
			{
				if (auto item = item_wptr_.lock())
				{
					boost::asio::async_read(item->stdout_pipe, item->stdout_buf, boost::asio::transfer_at_least(1),
						[this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
						_handle_receive(ec, bytes_transferred);
					});
				}
			}

			void JudgeProcessUnit::_handle_exit(int exit, const std::error_code& ec_in)
			{
				group_ref_.handle_error(
					process_num_,
					boost::system::error_code{ ec_in.value(), (boost::system::error_category&)(ec_in.category()) },
					std::string{ "exit code: " } + std::to_string(exit)
				);
			}

			void JudgeProcessUnit::_handle_receive(const boost::system::error_code& ec, size_t bytes_transferred)
			{
				if (ec)
				{
					group_ref_.handle_error(process_num_, ec, "");
					if (auto item = item_wptr_.lock())
						item->child.terminate();
					return;
				}

				if (auto item = item_wptr_.lock())
				{
					data_.stdout_handler(
						boost::asio::buffer_cast<const char*>(item->stdout_buf.data()), bytes_transferred
					);
					item->stdout_buf.consume(bytes_transferred);
					post_receive();
				}
			}
		}

		JudgeProcess::JudgeProcess(
			std::initializer_list<JudgeProcessData> processes_data,
			std::function<LogHandler>&& log_handler
		) : log_handler_(std::move(log_handler))
		{
			processes_.reserve(processes_data.size());
			std::size_t num = 1;
			for (const auto& d : processes_data)
				processes_.emplace_back(std::make_unique<detail::JudgeProcessUnit>(*this, num++, d));
		}

		bool JudgeProcess::launch(
			const boost::filesystem::path& program_path,
			const boost::process::wenvironment& env
		) noexcept
		{
			boost::asio::io_service io_service;

			auto group = std::make_shared<bp::group>();
			group_wptr_ = group;

			std::vector<std::shared_ptr<detail::JudgeProcessUnit::ChildItem>> children;
			children.reserve(processes_.size());

			for (auto& p : processes_)
			{
				children.emplace_back(p->make_child(program_path, *group, env, io_service));
				p->post_receive();
			}

			io_service.run();

			if (!group->wait_for(std::chrono::milliseconds(1500)) && group->valid())
				group->terminate();

			for (auto& c : children)
				c->stdout_pipe.close();

			io_service.stop();

			// To avoid a zombie process & get the exit code
			for (auto& c : children)
				c->child.wait();

			return true;
		}

		void JudgeProcess::terminate()
		{
			// To prevent a null pointer exception
			if (auto group = group_wptr_.lock())
				group->terminate();
		}
	}
}