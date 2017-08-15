#pragma once

#include "judge_page/test_case_box.hpp"
#include "page_base.hpp"
#include "../judge/judge_process.hpp"

#include <nana/gui/widgets/progress.hpp>

#include <mutex>

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
			explicit JudgePage(nana::window wd);

			JudgePage() : JudgePage(nullptr) { }

			void add_test_case();
			std::size_t proper_height() const override;
			bool remove_test_case(std::size_t pos);

			const judge::JudgeStreamInfo& stream_info() const noexcept
			{
				return stream_info_;
			}

		private:
			void _start_judging();
			void _clear_test_case_results();
			void _handle_judging_error(std::size_t case_num, const boost::system::error_code& ec);
			void _propagate_judging_error(const std::string& err_msg);
			void _propagate_judging_error(std::size_t case_num, const std::string& err_msg);
			void _show_btn_judge_start();

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

			judge::JudgeStreamInfo stream_info_;
			std::weak_ptr<judge::JudgeProcess> process_wptr_;
			std::chrono::time_point<std::chrono::system_clock> judge_started_time_;
		};
	}
}
