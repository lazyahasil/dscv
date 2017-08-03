#pragma once

#pragma warning(disable : 4244) // boost/process/detail/windows/wait_group.hpp
#include <boost/process.hpp>
#pragma warning(default : 4244)

namespace dscv
{
	namespace judge
	{
		using ErrorHandler = void(std::size_t, const boost::system::error_code&);
		using StdoutHandler = void(const char*, std::size_t);

		class JudgeProcess;

		struct JudgeProcessData
		{
			JudgeProcessData() = delete;

			JudgeProcessData(
				const std::wstring& _dir_overload,
				const std::string& _stdin_str,
				std::function<StdoutHandler>&& _stdout_handler
			) : dir_overload(_dir_overload), stdin_str(_stdin_str), stdout_handler(_stdout_handler)
			{ }

			std::wstring dir_overload;
			std::string stdin_str;
			std::function<StdoutHandler> stdout_handler;
		};

		namespace detail
		{
			class JudgeProcessUnit
			{
			public:
				struct ChildItem;

			public:
				JudgeProcessUnit() = delete;

				template <typename ...Args>
				JudgeProcessUnit(const JudgeProcess& group_ref, std::size_t process_num, Args&&... args)
					: group_ref_(group_ref), process_num_(process_num), data_(std::forward<Args>(args)...)
				{ }

				JudgeProcessUnit(const JudgeProcessUnit& src) = delete;
				JudgeProcessUnit& operator=(const JudgeProcessUnit& src) = delete;

				std::shared_ptr<ChildItem> make_child(
					const boost::filesystem::path& program_path,
					boost::process::group& group,
					const boost::process::wenvironment& env,
					boost::asio::io_service& io_service
				);

				void prepare_async()
				{
					_post_receive();
				}

			private:
				void _handle_receive(const boost::system::error_code& ec, size_t bytes_transferred);
				void _post_receive();

			public:
				//! The base class of JudgeProcessUnit::ChildItem
				//! This class is intended to initialize pipes first.
				struct ChildItemBase
				{
					ChildItemBase() = delete;

					explicit ChildItemBase(boost::asio::io_service& io_service)
						: stdout_pipe(io_service)
					{ }

					boost::process::opstream stdin_ops;
					boost::process::async_pipe stdout_pipe;
				};

				//! It contains destructable I/O items.
				//! There are a process handle, a input buffer for stdout and I/O pipes.
				struct ChildItem : public ChildItemBase
				{
					ChildItem() = delete;

					//! The constructor.
					//! Be aware that it automatically sets boost::process::child constructor's
					//! boost::process::std_in and boost::process::std_out parameters.
					template <typename ...Args>
					ChildItem(
						const JudgeProcessData& process_data, boost::asio::io_service& io_service, Args&&... args
					)
						: ChildItemBase(io_service),
						  child(
							  std::forward<Args>(args)...,
							  boost::process::std_in < stdin_ops,
							  boost::process::std_out > stdout_pipe
						  )
					{
						// std::endl is necessary to send output pipe stream, not a null terminater
						stdin_ops << process_data.stdin_str << std::endl;
					}

					boost::process::child child;
					boost::asio::streambuf stdout_buf;
				};

			private:
				const JudgeProcess& group_ref_;
				std::size_t process_num_;
				JudgeProcessData data_;

				std::weak_ptr<ChildItem> item_wptr_;
			};
		}

		class JudgeProcess
		{
		public:
			JudgeProcess() = delete;

			explicit JudgeProcess(std::function<ErrorHandler>&& log_handler)
				: log_handler_(std::move(log_handler))
			{ }

			JudgeProcess(
				std::initializer_list<JudgeProcessData> processes_data,
				std::function<ErrorHandler>&& log_handler
			);

			template<typename IterT>
			JudgeProcess(IterT begin, IterT end, std::function<ErrorHandler>&& log_handler)
				: log_handler_(std::move(log_handler))
			{
				// For std::move_iterator, don't add const and pass it through std::move()
				std::for_each(begin, end, [](IterT::reference process_data) {
					processes_.emplace_back(
						std::make_unique<detail::JudgeProcessUnit>(
							*this, processes_.size() + 1, std::move(process_data))
					);
				});
			}

			template <typename ...Args>
			void emplace_back(Args&&... args)
			{
				processes_.emplace_back(
					std::make_unique<detail::JudgeProcessUnit>(
						*this, processes_.size() + 1, std::forward<Args>(args)...)
				);
			}

			void handle_error(std::size_t process_num, const boost::system::error_code& ec) const
			{
				log_handler_(process_num, ec);
			}
			
			bool is_done() const noexcept
			{
				return group_wptr_.expired();
			}

			bool launch(
				const boost::filesystem::path& program_path,
				const boost::process::wenvironment& env = boost::this_process::wenvironment()
			) noexcept;

			template <typename JudgeProcessDataT>
			void push_back(JudgeProcessDataT&& data)
			{
				processes_.emplace_back(
					std::make_unique<detail::JudgeProcessUnit>(
						*this, processes_.size() + 1, std::forward<JudgeProcessDataT>(data))
				);
			}

			std::size_t size() const noexcept
			{
				return processes_.size();
			}

			void terminate();

		private:
			std::vector<std::unique_ptr<detail::JudgeProcessUnit>> processes_;
			std::function<ErrorHandler> log_handler_;

			std::weak_ptr<boost::process::group> group_wptr_;
		};
	}
}