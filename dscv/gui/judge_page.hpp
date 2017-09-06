#pragma once

#include "judge_page/test_case_box.hpp"
#include "judge_page/judge_config_panel.hpp"
#include "../judge/judge_process.hpp"

#include <nana/gui/widgets/progress.hpp>

namespace dscv
{
	namespace gui
	{
		namespace judge_page
		{
			namespace default_v_weights
			{
				constexpr std::size_t k_top_and_bottom = 10;
				constexpr std::size_t k_exe_path_bar = 40;
				constexpr std::size_t k_judge_bar = 32;
				constexpr std::size_t k_above_test_cases = 30;
				constexpr std::size_t k_below_test_cases = 30;
				constexpr std::size_t k_btn_add_case = 22;
			}

			namespace dynamic_v_weights
			{
				constexpr std::size_t k_gap_of_test_cases = 40;
			}

			constexpr std::size_t sum_of_default_v_weights()
			{
				using namespace judge_page::default_v_weights;
				return k_top_and_bottom * 2
					+ k_exe_path_bar
					+ k_judge_bar
					+ k_above_test_cases
					+ k_below_test_cases
					+ k_btn_add_case;
			}
		}

		class JudgePage : public PageBase
		{
		public:
			JudgePage() = delete;

			JudgePage(nana::window wd, const std::string& name);

			explicit JudgePage(const std::string& name) : JudgePage(nullptr, name) { }

			void add_test_case();

			ConfigHandler::Ptree& options_ptree() noexcept;

			std::size_t proper_height() const override;

			bool remove_test_case(std::size_t pos);

			ConfigHandler::Ptree& streams_ptree() noexcept;

		private:
			void _start_judging();

			void _clear_test_case_results();

			void _create_default_stream_info();

			//! Returns a ptree reference of configuration.
			//!
			//! This method gets name as a parameter because it's called at the constructor's member initializer list.
			//! @param name identification of this JudgePage
			ConfigHandler::Ptree& _get_config(const std::string& name);

			void _handle_judging_error(std::size_t case_num, const boost::system::error_code& ec);

			void _hide_btn_judge_start();

			void _propagate_judging_error(const std::string& err_msg);

			void _propagate_judging_error(std::size_t case_num, const std::string& err_msg);

			void _show_btn_judge_start();

			bool _stream_info_empty() noexcept;

			void _write_text_file_for_judge(const std::wstring& dir, const std::string& str, bool forced_endl_at_back);

			std::string name_;

			nana::place plc_{ *this };
			
			nana::label label_exe_path_{ *this };
			nana::textbox tb_exe_path_{ *this };
			nana::button btn_exe_path_{ *this };

			nana::button btn_judge_config_{ *this };
			nana::progress btn_judge_progress_{ *this };
			nana::button btn_judge_start_{ *this };
			nana::button btn_judge_terminate_{ *this };

			nana::button btn_add_case_{ *this, "+" };
			
			using TestCaseWrapper = WrapperPanel<false, judge_page::TestCaseBox>;
			std::vector<std::unique_ptr<TestCaseWrapper>> test_cases_;
			std::recursive_mutex test_cases_mutex_;

			ConfigHandler::Ptree& judge_config_;

			std::weak_ptr<judge::JudgeProcess> process_wptr_;
			std::chrono::time_point<std::chrono::system_clock> judge_started_time_;
		};
	}
}
