﻿#include "test_case_box.hpp"

#include "../judge_page.hpp"

#include <nana/gui/msgbox.hpp>

using namespace nana;

namespace dscv
{
	namespace gui
	{
		namespace judge_page
		{
			using namespace test_case_box;

			TestCaseBox::TestCaseBox(nana::window wd, JudgePage& page, std::size_t num)
				: panel<false>(wd), page_ref_(page), case_num_(num)
			{
				{
					using namespace test_case_box::default_v_weights;

					plc_.div((
						std::string{} + ""
						"vert"
						"<weight=" + std::to_string(k_title) + ""
						"  <label_test_case>"
						"  <weight=100 margin=[4,0,4,0] btn_remove_case>"
						">"
						"<"
						"  <vert"
						"    <weight=200 stdin_box>"
						"    <weight=0 in_file_boxes>"
						"  >"
						"  <weight=10>"
						"  <vert"
						"    <weight=33 margin=[6,0,5,2] label_test_log>"
						"    <ctb_test_log>"
						"  >"
						">"
						"<weight=" + std::to_string(k_gap_between_in_and_out) + ">"
						"<weight=200 stdout_box>"
						"<weight=0 out_file_boxes>"
						"<weight=0 inout_file_boxes>"
						).c_str()
					);
				}

				plc_["label_test_case"] << label_test_case_;
				plc_["btn_remove_case"] << btn_remove_case_;
				plc_["label_test_log"] << label_test_log_;
				plc_["ctb_test_log"] << ctb_test_log_;

				// Make stream fields invisible
				plc_.field_display("stdin_box", false);
				plc_.field_display("in_file_boxes", false);
				plc_.field_display("stdout_box", false);
				plc_.field_display("out_file_boxes", false);
				plc_.field_display("inout_file_boxes", false);

				// Set i18n
				btn_remove_case_.i18n(i18n_eval{ "Remove Case" });
				label_test_log_.i18n(i18n_eval{ "Test Case %arg0", i18n_eval{ "Log" } });

				case_num(num);
				label_test_case_.typeface(paint::font{ "", 14, { 700 } });
				label_test_log_.typeface(paint::font{ "", 12, { 700 } });
				label_test_case_.transparent(true);
				label_test_log_.transparent(true);

				ctb_test_log_.editable(false);
				ctb_test_log_.tb().enable_caret();

				// Initiate min_or_max_vert_scrolled_func
				scroll_page_func_ = [this](bool is_upward) {
					page_ref_.scroll_panel_make_vert_scroll(is_upward);
				};
				ctb_test_log_.min_or_max_vert_scrolled_func(scroll_page_func_);

				// Initiate stream boxes
				_init_streams_from_info(page.stream_info());

				auto removal_func = [this] {
					// Show a msgbox if not empty
					if (!stream_boxes_empty())
					{
						internationalization i18n;
						msgbox mb{ *this, i18n("Test Case %arg0", i18n("Removal")), msgbox::yes_no };
						mb.icon(msgbox::icon_question) << i18n("_msg_test_case_asking_removal");
						if (mb() == msgbox::pick_no)
							return;
					}
					page_ref_.remove_test_case(case_num_ - 1);
				};

				btn_remove_case_.events().click(removal_func);
				
				plc_.collocate();
			}

			void TestCaseBox::add_text_stream_in_file(std::size_t pos, const char* filename)
			{
				_add_text_stream_file(in_file_boxes_, pos,
					std::make_unique<TestInStreamBox>(*this, filename, pos + 1));
			}

			void TestCaseBox::add_text_stream_inout_file(std::size_t pos, const char* filename)
			{
				_add_text_stream_file(inout_file_boxes_, pos,
					std::make_unique<TestInOutStreamBox>(*this, filename, pos + 1));
			}

			void TestCaseBox::add_text_stream_out_file(std::size_t pos, const char* filename)
			{
				_add_text_stream_file(out_file_boxes_, pos,
					std::make_unique<TestOutStreamBox>(*this, filename, pos + 1));
			}

			bool TestCaseBox::add_text_stream_stdin()
			{
				// Check if it already exists
				if (stdin_box_)
					return false;

				stdin_box_ = std::make_unique<TestInStreamBox>(*this);
				stdin_box_->i18n_name_label(i18n_eval{ "Console Input" });
				stdin_box_->set_min_or_max_vert_scrolled_func(scroll_page_func_);

				plc_["stdin_box"] << *stdin_box_;
				plc_.field_display("stdin_box", true);

				in_box_group_.height += 200;

				return true;
			}

			bool TestCaseBox::add_text_stream_stdout()
			{
				// Check if it already exists
				if (stdout_box_)
					return false;

				stdout_box_ = std::make_unique<TestOutStreamBox>(*this);
				stdout_box_->i18n_name_label(i18n_eval{ "Console Output" });
				stdout_box_->set_min_or_max_vert_scrolled_func(scroll_page_func_);

				plc_["stdout_box"] << *stdout_box_;
				plc_.field_display("stdout_box", true);

				out_box_group_.height += 200;

				return true;
			}

			void TestCaseBox::case_num(std::size_t num)
			{
				case_num_ = num;
				label_test_case_.i18n(i18n_eval{ "Test Case %arg0", std::to_string(num) });
			}

			void TestCaseBox::clear_results()
			{
				if (stdout_box_)
					stdout_box_->clear_result();

				for (auto& out_box : out_file_boxes_.boxes)
					dynamic_cast<TestOutStreamBox&>(*out_box).clear_result();

				for (auto& inout_box : inout_file_boxes_.boxes)
					dynamic_cast<TestInOutStreamBox&>(*inout_box).clear_result();
			}

			bool TestCaseBox::stream_boxes_empty()
			{
				if (stdin_box_ && !stdin_box_->case_empty())
					return false;

				if (stdout_box_ && !stdout_box_->case_empty())
					return false;

				for (const auto& in_box : in_file_boxes_.boxes)
				{
					if (!in_box->case_empty())
						return false;
				}

				for (const auto& out_box : out_file_boxes_.boxes)
				{
					if (!out_box->case_empty())
						return false;
				}

				for (const auto& inout_box : inout_file_boxes_.boxes)
				{
					if (!inout_box->case_empty())
						return false;
				}

				return true;
			}

			std::size_t TestCaseBox::proper_height() const
			{
				std::size_t height = test_case_box::sum_of_default_v_weights();
				height += in_box_group_.height + out_box_group_.height;
				return height;
			}

			std::string TestCaseBox::text_stream_stdin() const noexcept
			{
				return std::string();
			}

			std::string TestCaseBox::text_stream_stdout_case() const noexcept
			{
				return std::string();
			}

			void TestCaseBox::text_stream_stdout_result_append(const std::string& str)
			{
				if (stdout_box_)
					stdout_box_->text_result_append(str);
			}

			void TestCaseBox::_add_text_stream_file(
				FileBoxes& fbs,
				std::size_t pos,
				std::unique_ptr<TestStreamBoxBase>&& new_box
			)
			{
				auto& cur = **fbs.boxes.insert(fbs.boxes.begin() + pos, std::move(new_box));
				const auto len = fbs.boxes.size(); // Be careful of order to call std::vector<>::size()

				cur.set_min_or_max_vert_scrolled_func(scroll_page_func_);

				// For boxes which were shoved, increase the number and get erased from the place
				for (auto i = pos + 1; i < len; i++)
				{
					auto& box_ptr = *fbs.boxes[i];
					box_ptr.set_stream_num(i + 1);
					plc_.erase(box_ptr);
				}

				// Add the current box
				plc_[fbs.place_str] << cur;

				// Re-add the shoved boxes to the place
				for (auto i = pos + 1; i < len; i++)
					plc_[fbs.place_str] << *fbs.boxes[i];

				// Set display of boxes
				if (len == 1)
					plc_.field_display(fbs.place_str, true);

				// Add height
				fbs.height += 200;
				fbs.box_group.height += 200;

				// Set the whole height of boxes
				const auto weight_str = std::string("weight=") + std::to_string(fbs.height);
				plc_.modify(fbs.place_str, weight_str.c_str());
			}

			bool TestCaseBox::_init_streams_from_info(const judge::JudgeStreamInfo& info)
			{
				// Check if there are at least one output files
				if (!info.has_stdout && info.out_files.empty() && info.inout_files.empty())
					return false;

				if (info.has_stdin)
					add_text_stream_stdin();

				if (info.has_stdout)
					add_text_stream_stdout();

				for (std::size_t i = 0; i < info.in_files.size(); i++)
				{
					if (info.in_files[i].is_text)
						add_text_stream_in_file(i, info.in_files[i].filename.c_str());
				}

				for (std::size_t i = 0; i < info.out_files.size(); i++)
				{
					if (info.out_files[i].is_text)
						add_text_stream_out_file(i, info.out_files[i].filename.c_str());
				}

				for (std::size_t i = 0; i < info.inout_files.size(); i++)
				{
					if (info.inout_files[i].is_text)
						add_text_stream_inout_file(i, info.inout_files[i].filename.c_str());
				}

				return true;
			}
		}
	}
}