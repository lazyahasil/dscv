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

		//! A struct of process' data.
		struct JudgeProcessData
		{
			JudgeProcessData() = delete;

			template <typename WStringT, typename StringT, typename FunctorT>
			JudgeProcessData(
				WStringT&& _dir_overload,
				StringT&& _stdin_str,
				FunctorT&& _stdout_handler
			)
				: dir_overload(std::forward<WStringT>(_dir_overload)),
				  stdin_str(std::forward<StringT>(_stdin_str)),
				  stdout_handler(std::forward<FunctorT>(_stdout_handler))
			{ }

			std::wstring dir_overload; //!< Working directory for the process
			std::string stdin_str; //!< String for stdin output
			std::function<StdoutHandler> stdout_handler; //!< Handler function object for stdout input
		};

		namespace detail
		{
			//! A single process unit for JudgeProcess.
			//!
			//! It provides asynchronous I/O methods using Boost.ASIO and Boost.Process.
			class JudgeProcessUnit
			{
			public:
				struct ChildItem; // Forward declaration

			public:
				JudgeProcessUnit() = delete;

				//! The constructor.
				//!
				//! @param group_ref const reference to the JudgeProcess
				//! @param process_num number of this process; must start from 1
				//! @param args arguments for JudgeProcessData's constructor
				template <typename ...Args>
				JudgeProcessUnit(const JudgeProcess& group_ref, std::size_t process_num, Args&&... args)
					: group_ref_(group_ref), process_num_(process_num), data_(std::forward<Args>(args)...)
				{ }

				JudgeProcessUnit(const JudgeProcessUnit& src) = delete;
				JudgeProcessUnit& operator=(const JudgeProcessUnit& src) = delete;

				//! Returns a new std::shared_ptr<ChildItem> object.
				//!
				//! JudgeProcessUnit keeps std::weak_ptr of the returned object.\n
				//! You have to call prepare_async() after calling it.
				//! @param program_path path of the executable program
				//! @param group boost::process:group object
				//! @param env process environment in std::wstring
				//! @param io_service reference to the boost::asio::io_service object
				//! @sa ChildItem
				std::shared_ptr<ChildItem> make_child(
					const boost::filesystem::path& program_path,
					boost::process::group& group,
					const boost::process::wenvironment& env,
					boost::asio::io_service& io_service
				);

				//! Start asynchronous post methods.
				void prepare_async()
				{
					_post_send();
					_post_receive();
				}

			private:
				//! Handles asynchronously received data and makes it await.
				//!
				//! It's called by the functor argument for boost::asio::async_read() at _post_receive().\n
				//! Then it calls _post_receive() so that incoming data can be received repeatedly.\n
				//! If there's any error, the functor group_ref_.handle_error will be called and this child process
				//! will be terminated by compulsion.
				//! @param ec Boost-style error code
				//! @param bytes_transferred bytes of data transferred
				//! @sa _post_receive()
				void _handle_receive(const boost::system::error_code& ec, size_t bytes_transferred);

				//! Handles asynchronously sending error and closes the pipe.
				//!
				//! It's called by the functor argument for boost::asio::async_write() at _post_send().\n
				//! Then it closes the boost::process::async_pipe so that EOF may be sended.\n
				//! If there's any error, the functor group_ref_.handle_error will be called and this child process
				//! will be terminated by compulsion.
				//! @param ec Boost-style error code
				//! @param bytes_transferred bytes of data transferred
				//! @sa _post_send()
				void _handle_send(const boost::system::error_code& ec, size_t bytes_transferred);

				//! Asynchronously waits for incoming data and receives it.
				//!
				//! @sa _handle_receive()
				void _post_receive();

				//! Asynchronously sends data.
				//!
				//! @sa _handle_send()
				void _post_send();

			public:
				//! The base class of JudgeProcessUnit::ChildItem.
				//!
				//! This class is intended to initialize pipes first.
				struct ChildItemBase;

				//! It contains destructable I/O items.
				//!
				//! There are a process handle, a input buffer for stdout and I/O pipes.\n
				//! The function, which manages process routine, owns and destructs std::shared_ptr of it.
				//! So JudgeProcessUnit has std::weak_ptr and is able to use it safely whether the process is running.
				struct ChildItem;

			private:
				const JudgeProcess& group_ref_; //!< Const reference to the JudgeProcess
				std::size_t process_num_; //!< Number of this process
				JudgeProcessData data_; //!< Data of this process

				std::weak_ptr<ChildItem> item_wptr_; //!< std::weak_ptr to the ChildItem
			};

			struct JudgeProcessUnit::ChildItemBase
			{
				ChildItemBase() = delete;

				explicit ChildItemBase(boost::asio::io_service& io_service)
					: stdin_pipe(io_service), stdout_pipe(io_service)
				{ }

				virtual ~ChildItemBase() = default;

				boost::process::async_pipe stdin_pipe; //!< Pipe for stdin output.
				boost::process::async_pipe stdout_pipe; //!< Pipe for stdout input.
			};

			struct JudgeProcessUnit::ChildItem : public JudgeProcessUnit::ChildItemBase
			{
				ChildItem() = delete;

				//! The constructor.
				//!
				//! Be aware that it automatically sets boost::process::child constructor's
				//! boost::process::std_in and boost::process::std_out parameters.
				//! @param io_service reference to the boost::asio::io_service object
				//! @param args additional arguments for boost::process::child's constructor
				template <typename ...Args>
				ChildItem(boost::asio::io_service& io_service, Args&&... args)
					: ChildItemBase(io_service),
					  child(
						  std::forward<Args>(args)...,
						  boost::process::std_in < stdin_pipe,
						  boost::process::std_out > stdout_pipe
					  )
				{ }

				boost::process::child child; //!< boost::process::child object
				boost::asio::streambuf stdout_buf; //!< stdout input buffer
			};
		}

		//! Judged process pack.
		//!
		//! It contains a list of detail::JudgeProcessUnit objects.
		//! @sa detail::ChildItem
		class JudgeProcess
		{
		public:
			JudgeProcess() = delete;

			//! The constructor.
			//!
			//! @param log_handler Log handler function object
			template <typename FunctorT>
			explicit JudgeProcess(FunctorT&& log_handler)
				: log_handler_(std::forward<FunctorT>(log_handler))
			{ }

			//! The constructor.
			//!
			//! @param processes_data List of JudgeProcessData elements
			//! @param log_handler Log handler function object
			template <typename FunctorT>
			JudgeProcess(
				std::initializer_list<JudgeProcessData> processes_data,
				FunctorT&& log_handler
			) : log_handler_(std::forward<FunctorT>(log_handler))
			{
				processes_.reserve(processes_data.size());
				std::size_t num = 1;
				for (const auto& d : processes_data)
					processes_.emplace_back(std::make_unique<detail::JudgeProcessUnit>(*this, num++, d));
			}

			//! The constructor.
			//!
			//! By default, it moves elements if range iterators are moveable.
			//! Otherwise, it copies.
			//! @param begin begin iterator to the range of JudgeProcessData elements
			//! @param end end iterator to the range of JudgeProcessData elements
			//! @param log_handler log handler function object
			template<typename IterT, typename FunctorT>
			JudgeProcess(IterT begin, IterT end, FunctorT&& log_handler)
				: log_handler_(std::forward<FunctorT>(log_handler))
			{
				// For std::move_iterator, don't add const and pass it through std::move()
				std::for_each(begin, end, [](IterT::reference process_data) {
					processes_.emplace_back(
						std::make_unique<detail::JudgeProcessUnit>(
							*this, processes_.size() + 1, std::move(process_data))
					);
				});
			}

			//! Emplaces back a JudgeProcessData.
			//!
			//! @param args arguments for JudgeProcessData's constructor
			template <typename ...Args>
			void emplace_back(Args&&... args)
			{
				processes_.emplace_back(
					std::make_unique<detail::JudgeProcessUnit>(
						*this, processes_.size() + 1, std::forward<Args>(args)...)
				);
			}

			//! Invokes the log handler.
			//!
			//! It is usually called by detail::JudgeProcessUnit.
			//! @sa detail::JudgeProcessUnit
			void handle_error(std::size_t process_num, const boost::system::error_code& ec) const
			{
				log_handler_(process_num, ec);
			}
			
			//! Checks if the boost::process::group object is destroyed.
			//! @returns true if destroyed
			bool is_done() const noexcept
			{
				return group_wptr_.expired();
			}

			//! Launches all the processes in group.
			bool launch(
				const boost::filesystem::path& program_path,
				const boost::process::wenvironment& env = boost::this_process::wenvironment()
			) noexcept;

			//! Push back a JudgeProcessData.
			//!
			//! emplace_back() is more efficient in usual.
			//! @param data a JudgeProcessData object
			//! @sa emplace_back()
			template <typename JudgeProcessDataT>
			void push_back(JudgeProcessDataT&& data)
			{
				processes_.emplace_back(
					std::make_unique<detail::JudgeProcessUnit>(
						*this, processes_.size() + 1, std::forward<JudgeProcessDataT>(data))
				);
			}

			//! Returns the number of processes it has.
			std::size_t size() const noexcept
			{
				return processes_.size();
			}

			//! Terminate the process group running.
			void terminate();

		private:
			std::vector<std::unique_ptr<detail::JudgeProcessUnit>> processes_; //!< detail::JudgeProcessUnit list
			std::function<ErrorHandler> log_handler_; //!< Handler function object for logging

			std::weak_ptr<boost::process::group> group_wptr_; //!< std::weak_ptr to the boost::process::group
		};
	}
}