#include "judge_page.hpp"

#include "judge_page/judge_config_form.hpp"
#include "../judge/judge_file_writer.hpp"

#include <nana/gui/filebox.hpp>

#include <boost/algorithm/string.hpp>

#include <sstream>

using namespace nana;

namespace dscv
{
	namespace gui
	{
		using namespace judge_page;

		JudgePage::JudgePage(nana::window wd) : PageBase(wd)
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
				JudgeConfigForm w{ *this, *this };
				w.show();
				w.wait_for_this();
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

			// Make a default test case
			add_test_case();
			test_cases_[0]->content().disable_removal_btn(true);

			plc_.collocate();
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
				test_cases_[0]->content().disable_removal_btn(false);

			scroll_panel_refresh();
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

		void JudgePage::_start_judging()
		{
			namespace fs = boost::filesystem;

			// Clear results and initiate the time point
			_clear_test_case_results();
			judge_started_time_ = std::chrono::system_clock::now();

			internationalization i18n;
			_propagate_judging_error(i18n("_msg_judge_start"));

			plc_.field_display("btn_judge_start", false);
			plc_.field_display("btn_judge_terminate", true);
			plc_.collocate();

			if (btn_judge_start_.focused())
				btn_judge_terminate_.focus();

			try
			{
				auto program_path = fs::path(tb_exe_path_.caption_wstring());
				if (!fs::exists(program_path))
					throw std::runtime_error{ "Cannot find the execuatable program!" };

				auto work_path = fs::path(L".dscv/judge/test");
				if (!fs::exists(work_path) && !fs::create_directories(work_path))
					throw std::runtime_error{ std::string{ "Cannot create a directory: " } + work_path.string() };
				
				auto process = std::make_shared<judge::JudgeProcess>(
					[this](std::size_t process_num,	const boost::system::error_code& ec) {
					_handle_judging_error(process_num, ec);
				});
				process_wptr_ = process;

				for (const auto& wrapper : test_cases_)
				{
					auto& tc = wrapper->content();
					auto dir = work_path / std::to_wstring(tc.case_num());

					if (!fs::exists(dir) && !fs::create_directories(dir))
						throw std::runtime_error{ std::string{ "Cannot create a directory: " } + work_path.string() };

					auto stdout_handler = [&tc](const char* str, std::size_t bytes) {
						tc.text_stream_stdout_result_append(std::string{ str, bytes });
					};

					// Get the stdin case text
					auto stdin_str = tc.text_stream_stdin();
					//if (!boost::ends_with(stdin_str, "\n")) // If it doesn't end with endl, push one back
					//	stdin_str.push_back('\n');

					process->emplace_back(dir.wstring(), std::move(stdin_str), stdout_handler);

					for (std::size_t i = 0; i < stream_info_.in_files.size(); i++)
						judge::file_writer::write_text_file(dir.wstring(), tc.text_stream_in_file_case(i));

					for (std::size_t i = 0; i < stream_info_.inout_files.size(); i++)
						judge::file_writer::write_text_file(dir.wstring(), tc.text_stream_inout_file_case_in(i));
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
				internationalization i18n;
				msgbox mb{ i18n("Starting Judgment Failed") };
				mb.icon(msgbox::icon_error);
				mb << i18n("_msgbox_error_1_arg", charset{ e.what() }.to_bytes(unicode::utf8));
				mb.show();
			}
		}

		void JudgePage::_clear_test_case_results()
		{
			for (const auto& wrapper : test_cases_)
			{
				auto& tc = wrapper->content();
				tc.clear_results_and_log();
			}
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

				auto str = oss.str();

				// Write a log to all the test cases
				_propagate_judging_error(str);

				// Launch a msgbox
				//msgbox mb{ i18n("Judged Process Error") };
				//mb.icon(msgbox::icon_error) << i18n("_msgbox_error_1_arg", str);
				//mb.show();
			}
		}

		void JudgePage::_propagate_judging_error(const std::string& err_msg)
		{
			auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now() - judge_started_time_);

			for (auto& wrapper : test_cases_)
				wrapper->content().handle_judging_error(elapsed_time, err_msg);
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
	}
}
