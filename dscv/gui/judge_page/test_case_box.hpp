#pragma once

#include "../../judge/judge_stream_info.hpp"
#include "../page_base.hpp"
#include "test_stream_box.hpp"

#include <boost/system/error_code.hpp>

namespace dscv
{
	namespace gui
	{
		class JudgePage;

		namespace judge_page
		{
			namespace test_case_box
			{
				namespace default_v_weights
				{
					constexpr std::size_t k_title = 30;
					constexpr std::size_t k_gap_between_in_and_out = 20;
				}

				constexpr std::size_t sum_of_default_v_weights()
				{
					using namespace default_v_weights;
					return k_title
						+ k_gap_between_in_and_out;
				}
			}

			// Note: add_text_stream_...() methods doesn't call nana::place::collocate().
			class TestCaseBox : public nana::panel<false>, detail::ScrollPanelBase::SideAdaptable<true>
			{
			private:
				struct FileBoxes;

			public:
				TestCaseBox() = delete;
				TestCaseBox(nana::window wd, JudgePage& page, std::size_t num);

				void add_text_stream_inout_file(std::size_t pos, const char* filename);
				void add_text_stream_in_file(std::size_t pos, const char* filename);
				void add_text_stream_out_file(std::size_t pos, const char* filename);
				bool add_text_stream_stdin();
				bool add_text_stream_stdout();
				bool stream_boxes_empty();

				std::size_t case_num() const noexcept
				{
					return case_num_;
				}

				void case_num(std::size_t num);
				void clear_results_and_log();

				void collocate()
				{
					plc_.collocate();
				}

				void disable_removal_btn(bool is_disabled)
				{
					btn_remove_case_.enabled(!is_disabled);
				}

				void handle_judging_error(const std::chrono::milliseconds& elapsed_time, const std::string& err_msg);

				std::size_t proper_height() const override;

				std::string text_stream_inout_file_case_in(std::size_t pos) const
				{
					return dynamic_cast<TestInOutStreamBox&>(*inout_file_boxes_.boxes[pos]).text_case();
				}

				std::string text_stream_inout_file_case_out(std::size_t pos) const
				{
					return dynamic_cast<TestInOutStreamBox&>(*inout_file_boxes_.boxes[pos]).text_case_out();
				}

				void text_stream_inout_file_result_append(std::size_t pos, const std::string& str)
				{
					dynamic_cast<TestInOutStreamBox&>(*inout_file_boxes_.boxes[pos]).text_result_append(str);
				}

				std::string text_stream_in_file_case(std::size_t pos) const
				{
					return dynamic_cast<TestInStreamBox&>(*in_file_boxes_.boxes[pos]).text_case();
				}

				std::string text_stream_out_file_case(std::size_t pos) const
				{
					return dynamic_cast<TestOutStreamBox&>(*out_file_boxes_.boxes[pos]).text_case();
				}

				void text_stream_out_file_result_append(std::size_t pos, const std::string& str)
				{
					dynamic_cast<TestOutStreamBox&>(*out_file_boxes_.boxes[pos]).text_result_append(str);
				}

				std::string text_stream_stdin() const noexcept;
				std::string text_stream_stdout_case() const noexcept;
				void text_stream_stdout_result_append(const std::string& str);

			private:
				void _add_text_stream_file(
					FileBoxes& fb, std::size_t pos, std::unique_ptr<TestStreamBoxBase>&& new_box
				);

				bool _init_streams_from_info(const judge::JudgeStreamInfo& info);
				
				JudgePage& page_ref_;
				std::size_t case_num_;

				nana::place plc_{ *this };
				nana::label label_test_case_{ *this };
				nana::label label_test_log_{ *this };
				nana::button btn_remove_case_{ *this };

				CodeTextbox ctb_test_log_{ *this };

				struct BoxGroup
				{
					std::size_t height{ 0 };
				};

				BoxGroup in_box_group_;
				BoxGroup out_box_group_;

				std::unique_ptr<TestInStreamBox> stdin_box_;
				std::unique_ptr<TestOutStreamBox> stdout_box_;

				struct FileBoxes
				{
					FileBoxes() = delete;

					FileBoxes(const char* _place_str, BoxGroup& _box_group)
						: place_str(_place_str), box_group(_box_group)
					{ }

					std::vector<std::unique_ptr<TestStreamBoxBase>> boxes;
					BoxGroup& box_group;
					std::size_t height{ 0 };
					const char* place_str{ nullptr };
				};

				FileBoxes in_file_boxes_{ "in_file_boxes", in_box_group_ };
				FileBoxes out_file_boxes_{ "out_file_boxes", out_box_group_ };
				FileBoxes inout_file_boxes_{ "inout_file_boxes", out_box_group_ };

				std::function<void(bool)> scroll_page_func_;
			};
		}
	}
}
