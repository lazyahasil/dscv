#include "judge_page.hpp"

#include "config_gui_helper.hpp"
#include "../judge/judge_file_writer.hpp"

#include <nana/gui/filebox.hpp>

#include <boost/algorithm/string.hpp>

#include <sstream>

using namespace nana;

namespace dscv
{
	using namespace judge;

	namespace gui
	{
		using namespace judge_page;

		JudgePage::JudgePage(nana::window wd, const std::string& name)
			: PageBase(wd), name_(name), judge_config_(_get_config(name))
		{
			{
				using namespace default_v_weights;
				using namespace dynamic_v_weights;

				plc_.div(
					dynamic_cast<std::ostringstream&>(
						std::ostringstream{} << ""
						"vert margin=" << k_top_and_bottom << ""
						"<weight=" << k_exe_path_bar << " margin=[0,0,18,0]"
						"  <weight=90 label_exe_path>"
						"  <margin=[0,4,0,0] tb_exe_path>"
						"  <weight=60 btn_exe_path>"
						">"
						"<weight=" << k_judge_bar << " margin=[0,0,6,0]"
						"  <weight=100 btn_judge_config>"
						"  <weight=5%>"
						"  <margin=[0,4,0,4] btn_judge_progress>"
						"  <weight=80 btn_judge_start>"
						"  <weight=80 btn_judge_terminate>"
						">"
						"<weight=" << k_above_test_cases << ">"
						"<vert gap=" << k_gap_of_test_cases << " test_cases>"
						"<weight=" << k_below_test_cases << ">"
						"<weight=" << k_btn_add_case << " btn_add_case>"
						).str().c_str()
				);
			}

			plc_["label_exe_path"] << label_exe_path_;
			plc_["tb_exe_path"] << tb_exe_path_;
			plc_["btn_exe_path"] << btn_exe_path_;
			plc_["btn_judge_config"] << btn_judge_config_;
			plc_["btn_judge_progress"] << btn_judge_progress_;
			plc_["btn_judge_start"] << btn_judge_start_;
			plc_["btn_judge_terminate"] << btn_judge_terminate_;
			plc_["btn_add_case"] << btn_add_case_;

			// Make some button fields invisible
			plc_.field_display("btn_judge_terminate", false);

			// Set i18n
			label_exe_path_.i18n(i18n_eval{ "Program Path" });
			btn_exe_path_.i18n(i18n_eval{ "Find" });
			btn_judge_config_.i18n(i18n_eval{ "Configuration" });
			btn_judge_start_.i18n(i18n_eval{ "Judge" });
			btn_judge_terminate_.i18n(i18n_eval{ "Terminate" });

			label_exe_path_.click_for(tb_exe_path_);
			label_exe_path_.text_align(align::left, align_v::center);
			label_exe_path_.transparent(true);
			tb_exe_path_.multi_lines(false);
			btn_judge_terminate_.bgcolor(colors::orange);

			// Create streams' information if empty
			if (_stream_info_empty())
				_create_default_stream_info();

			// Make a default test case
			add_test_case();
			test_cases_[0]->content().disable_removal_btn(true);

			plc_.collocate();

			events().destroy([this] {
				// Terminate the process group if running when page being destroyed
				if (auto ptr = process_wptr_.lock())
					ptr->terminate();
			});

			btn_exe_path_.events().click([this] {
				internationalization i18n;
				filebox fb{ *this, true };
				fb.add_filter(i18n("Executable File (%arg0)", "*.exe"), "*.exe");
				fb.add_filter(i18n("All Files (%arg0)", "*.*"), "*.*");
				if (fb())
					tb_exe_path_.caption(fb.file());
			});

			btn_judge_config_.events().click([this] {
				internationalization i18n;
				std::ostringstream oss;
				oss << name_ << " " << i18n("Configuration");
				main_window_config_open<JudgeConfigPanel>(oss.str(), *this);
			});

			btn_judge_start_.events().click([this] {
				_start_judging();
			});

			btn_judge_terminate_.events().click([this] {
				if (auto ptr = process_wptr_.lock())
				{
					ptr->terminate();
					_show_btn_judge_start();
				}
			});

			btn_add_case_.events().click([this] {
				add_test_case();
				scroll_panel_vert_scroll_to_end();
			});
		}

		void JudgePage::adapt_height_of_test_cases()
		{
			for (auto& wrap_ptr : test_cases_)
				wrap_ptr->fix_content_height(wrap_ptr->content().proper_height());
			scroll_panel_refresh();
		}

		void JudgePage::add_test_case()
		{
			std::lock_guard<std::recursive_mutex> g(test_cases_mutex_);

			// Add a test case
			test_cases_.emplace_back(std::make_unique<TestCaseWrapper>(*this, *this, test_cases_.size() + 1));

			// Set the test case
			auto& new_case = *test_cases_.back();
			plc_["test_cases"] << new_case;
			new_case.fix_content_height(new_case.content().proper_height());

			// Unlock removal if the size got 2
			if (test_cases_.size() == 2)
				(*test_cases_.begin())->content().disable_removal_btn(false);

			scroll_panel_refresh();
		}

		bool JudgePage::add_text_stream(
			judge::judge_stream::StreamType type,
			std::size_t pos,
			const std::string& filename
		)
		{
			for (const auto& wrap_ptr : test_cases_)
			{
				auto& tc = wrap_ptr->content();
				auto result = tc.add_text_stream(type, pos, filename);
				if (!result)
					return false;
			}
			return true;
		}

		bool JudgePage::add_text_stream(
			judge::judge_stream::StreamType type,
			const std::string& filename
		)
		{
			for (const auto& wrap_ptr : test_cases_)
			{
				auto& tc = wrap_ptr->content();
				auto result = tc.add_text_stream(type, filename);
				if (!result)
					return false;
				tc.collocate();
			}
			return true;
		}

		ConfigHandler::Ptree& JudgePage::options_ptree() noexcept
		{
			return ConfigHandler::subtree(judge_config_, judge_page::options::k_path_str);
		}

		std::size_t JudgePage::proper_height() const
		{
			std::size_t height = sum_of_default_v_weights();
			
			// Add all of cases' proper height
			for (const auto& tc : test_cases_)
				height += tc->content().proper_height();

			// Add gap weight between cases
			if (!test_cases_.empty())
				height += dynamic_v_weights::k_gap_of_test_cases * (test_cases_.size() - 1);

			return height;
		}

		bool JudgePage::remove_test_case(std::size_t pos)
		{
			if (pos >= test_cases_.size())
				return false;

			std::lock_guard<std::recursive_mutex> g(test_cases_mutex_);

			// Remove the test case.
			// nana::place::erase() is unnecessary and makes a "horrible" delay.
			auto iter = test_cases_.begin() + pos;
			test_cases_.erase(iter);
			
			// Reset remaining test cases' numbers
			const auto new_size = test_cases_.size();
			for (auto i = pos; i < new_size; i++)
				test_cases_[i]->content().case_num(i + 1);

			// Lock removal if size got 1
			if (test_cases_.size() == 1)
				(*test_cases_.begin())->content().disable_removal_btn(true);

			scroll_panel_refresh();

			return true;
		}

		ConfigHandler::Ptree& JudgePage::streams_ptree() noexcept
		{
			return ConfigHandler::subtree(judge_config_, judge::judge_stream::k_path_str);
		}

		void JudgePage::_clear_test_case_results()
		{
			for (const auto& wrap_ptr : test_cases_)
			{
				auto& tc = wrap_ptr->content();
				tc.clear_results_and_log();
			}
		}

		void JudgePage::_create_default_stream_info()
		{
			auto& ptree = streams_ptree();
			ptree.put(judge_stream::k_has_stdin, true);
			ptree.put(judge_stream::k_has_stdout, true);
			ptree.put_child(judge_stream::k_array_in_files, ConfigHandler::Ptree{});
			ptree.put_child(judge_stream::k_array_out_files, ConfigHandler::Ptree{});
			ptree.put_child(judge_stream::k_array_inout_files, ConfigHandler::Ptree{});
			// Write JSON
			config_gui_helper::write_json_noexcept();
		}

		ConfigHandler::Ptree& JudgePage::_get_config(const std::string& name)
		{
			std::ostringstream oss;
			oss << config_handler::str_path::k_judge_page << "." << name;
			return ConfigHandler::instance().subtree(oss.str());
		}

		void JudgePage::_handle_judging_error(
			std::size_t case_num,
			const boost::system::error_code& ec
		)
		{
			internationalization i18n;
			std::ostringstream oss;
			
			oss << i18n("Error") << ": ";

			if (case_num != 0) // Test case error
			{
				// Interpret error_code
				if (ec == boost::asio::error::broken_pipe)
					oss << i18n("_msg_error_asio_broken_pipe");
				else
					oss << charset{ ec.message() }.to_bytes(unicode::utf8);

				// Write a log to the corresponding test case
				_propagate_judging_error(case_num, oss.str());
			}
			else // Termination signals
			{
				if (ec == boost::asio::error::timed_out)
					oss << i18n("_msg_error_process_termination_timed_out");
				else if (case_num == 0 && ec == boost::system::errc::operation_canceled)
					oss << i18n("_msg_error_process_termination_ordered");
				else
					oss << charset{ ec.message() }.to_bytes(unicode::utf8);

				// Write a log to all the test cases
				_propagate_judging_error(oss.str());

				// Launch a msgbox
				// (Removed)
			}
		}

		void JudgePage::_hide_btn_judge_start()
		{
			plc_.field_display("btn_judge_start", false);
			plc_.field_display("btn_judge_terminate", true);
			plc_.collocate();
			if (btn_judge_start_.focused())
				btn_judge_terminate_.focus();
		}

		void JudgePage::_propagate_judging_error(const std::string& err_msg)
		{
			auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now() - judge_started_time_);

			for (auto& wrap_ptr : test_cases_)
				wrap_ptr->content().handle_judging_error(elapsed_time, err_msg);
		}

		void JudgePage::_propagate_judging_error(std::size_t case_num, const std::string& err_msg)
		{
			auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now() - judge_started_time_);

			test_cases_[case_num - 1]->content().handle_judging_error(elapsed_time, err_msg);
		}

		void JudgePage::_show_btn_judge_start()
		{
			plc_.field_display("btn_judge_start", true);
			plc_.field_display("btn_judge_terminate", false);
			plc_.collocate();

			if (btn_judge_terminate_.focused())
				btn_judge_start_.focus();
		}

		void JudgePage::_start_judging()
		{
			namespace fs = boost::filesystem;

			internationalization i18n;

			// Clear results and initiate the time point
			_clear_test_case_results();
			judge_started_time_ = std::chrono::system_clock::now();

			// Push a start message (after clearing)
			_propagate_judging_error(i18n("_msg_judge_start"));

			// Hide the start button and show the termination button
			_hide_btn_judge_start();

			try
			{
				// Check the executable
				auto program_path = fs::path{ tb_exe_path_.caption_wstring() };
				if (!fs::exists(program_path))
					throw std::runtime_error{ "Cannot find the execuatable program!" };

				// Check the work path
				auto work_path = fs::path{ L".dscv/judge/test" };
				if (!fs::exists(work_path) && !fs::create_directories(work_path))
					throw std::runtime_error{ std::string{ "Cannot create a directory: " } +work_path.string() };

				// Get ptrees of in and inout files
				auto ptree_in_f = ConfigHandler::subtree(judge_config_, judge_stream::k_array_in_files);
				auto ptree_inout_f = ConfigHandler::subtree(judge_config_, judge_stream::k_array_inout_files);

				// Options
				auto forced_endl_at_back
					= options_ptree().get(options::k_judging_force_endl_at_input_end, false);

				// Lambda to write input text files
				auto lambda_text_writer = [this, forced_endl_at_back](
					const ConfigHandler::Ptree& files_ptree,
					const std::wstring& dir_wstr,
					const TestCaseBox& tc,
					std::string(TestCaseBox::*text_reader)(std::size_t) const
					) {
					std::size_t count = 0;
					for (const auto& val : files_ptree)
					{
						auto media = val.second.get_optional<std::string>(judge_stream::file_info::k_media);
						auto filename = val.second.get(judge_stream::file_info::k_name, "");
						if (!media || *media == judge_stream::file_media::k_text) // Text stream
						{
							_write_text_file_for_judge(
								dir_wstr + std::wstring(charset{ filename }),
								(tc.*text_reader)(count),
								forced_endl_at_back
							);
						}
						count++;
					}
				};

				// Create std::shared_ptr of JudgeProcess
				auto process = std::make_shared<judge::JudgeProcess>(
					[this](std::size_t process_num, const boost::system::error_code& ec) {
					_handle_judging_error(process_num, ec);
				});
				process_wptr_ = process; // Bind a std::weak_ptr

				for (const auto& wrap_ptr : test_cases_)
				{
					auto& tc = wrap_ptr->content();
					auto dir = work_path / std::to_wstring(tc.case_num());

					if (!fs::exists(dir) && !fs::create_directories(dir))
						throw std::runtime_error{ std::string{ "Cannot create a directory: " } +work_path.string() };

					auto stdout_handler = [&tc](const char* str, std::size_t bytes) {
						tc.text_stream_stdout_result_append(std::string{ str, bytes });
					};

					// Get the stdin case text
					auto stdin_str = tc.text_stream_stdin();

					// If "forced_endl_at_back" is set and stdin doesn't end with endl, push one back
					if (forced_endl_at_back && !boost::ends_with(stdin_str, "\n"))
						stdin_str.push_back('\n');

					// Add a process' data to be judged
					process->emplace_back(dir.wstring(), std::move(stdin_str), stdout_handler);

					// Write input text files
					lambda_text_writer(ptree_in_f, dir.wstring(), tc, &TestCaseBox::text_stream_in_file_case);
					lambda_text_writer(ptree_inout_f, dir.wstring(), tc, &TestCaseBox::text_stream_inout_file_case_in);
				}

				_propagate_judging_error(i18n("_msg_judge_launching_processes"));

				// Launch asynchronously, detaching from the main thread.
				// Warning: never use std::launch(). It will block if it returns std::future.
				std::thread{ [this, process, program_path] {
					process->launch(program_path);
					_show_btn_judge_start();
				} }.detach();
			}
			catch (std::exception& e)
			{
				_show_btn_judge_start();
				msgbox mb{ i18n("Starting Judgment Failed") };
				mb.icon(msgbox::icon_error) << i18n("_msgbox_error_occurred") << std::endl;
				mb << charset{ e.what() }.to_bytes(unicode::utf8);
				mb.show();
			}
		}

		bool JudgePage::_stream_info_empty() noexcept
		{
			try
			{
				// If one of Ptree::get_value() or Ptree::get_child() fails, return true
				const auto& ptree = streams_ptree();
				auto has_stdin = ptree.get<bool>(judge_stream::k_has_stdin);
				auto has_stdout = ptree.get<bool>(judge_stream::k_has_stdout);
				const auto& ptree_in_f = ptree.get_child(judge_stream::k_array_in_files);
				const auto& ptree_out_f = ptree.get_child(judge_stream::k_array_out_files);
				const auto& ptree_inout_f = ptree.get_child(judge_stream::k_array_inout_files);

				// If there's no stream, return true
				if (!has_stdin && !has_stdout && ptree_in_f.empty() && ptree_out_f.empty() && ptree_inout_f.empty())
					return true;
			}
			catch (ConfigHandler::PtreeError&)
			{
				// If the ptree is invalid, return true
				return true;
			}

			return false;
		}

		void JudgePage::_write_text_file_for_judge(
			const std::wstring& dir, const std::string& str, bool forced_endl_at_back
		)
		{
			if (forced_endl_at_back && str.back() != '\n' && str.back() != '\r')
				file_writer::write_text_file(dir, str + '\n');
			else
				file_writer::write_text_file(dir, str);
		}
	}
}
