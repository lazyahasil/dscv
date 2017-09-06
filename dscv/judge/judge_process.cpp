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
					env,
					bp::start_dir = data_.dir_overload
				);

				item_wptr_ = item;

				return item;
			}

			void JudgeProcessUnit::_handle_receive(const boost::system::error_code& ec, size_t bytes_transferred)
			{
				if (ec)
				{
					group_ref_.handle_error(process_num_, ec);
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
					_post_receive();
				}
			}

			void JudgeProcessUnit::_handle_send(const boost::system::error_code& ec, size_t bytes_transferred)
			{
				if (ec)
				{
					group_ref_.handle_error(process_num_, ec);
					if (auto item = item_wptr_.lock())
						item->child.terminate();
					return;
				}

				if (auto item = item_wptr_.lock())
				{
					// Close the pipe to make the stdin EOF
					item->stdin_pipe.async_close();
				}
			}

			void JudgeProcessUnit::_post_receive()
			{
				if (auto item = item_wptr_.lock())
				{
					boost::asio::async_read(item->stdout_pipe, item->stdout_buf, boost::asio::transfer_at_least(1),
						[this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
						_handle_receive(ec, bytes_transferred);
					});
				}
			}

			void JudgeProcessUnit::_post_send()
			{
				if (auto item = item_wptr_.lock())
				{
					boost::asio::async_write(item->stdin_pipe, boost::asio::buffer(data_.stdin_str),
						[this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
						_handle_send(ec, bytes_transferred);
					});
				}
			}
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
				p->prepare_async();
			}

			io_service.run();

			if (!group->wait_for(std::chrono::milliseconds(1500)))
			{
				log_handler_(0, boost::asio::error::make_error_code(boost::asio::error::timed_out));
				group->terminate();
			}

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
			{
				log_handler_(0, boost::system::errc::make_error_code(boost::system::errc::operation_canceled));
				group->terminate();
			}
		}
	}
}