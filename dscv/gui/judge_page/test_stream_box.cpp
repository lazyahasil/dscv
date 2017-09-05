#include "test_stream_box.hpp"

#include "../widgets/detail/hack_textbox.hpp"

using namespace nana;

namespace dscv
{
	namespace gui
	{
		namespace judge_page
		{
			TestInStreamBox::TestInStreamBox(nana::window wd, const std::string& label_str, std::size_t num)
				: TestStreamBoxBase(wd)
			{
				plc_.div(
					"vert"
					"<weight=33 margin=[6,0,5,2]"
					"  <weight=20 label_num>"
					"  <label_name>"
					"  <weight=40 gap=3 buttons>"
					">"
					"<"
					"  <vert gap=10 ctb_case>"
					"  <weight=10 gap_of_case_and_result>"
					"  <ctb_result>"
					">"
				);
				plc_["label_num"] << label_num_;
				plc_["label_name"] << label_name_;
				plc_["buttons"] << btn_save_;
				plc_["ctb_case"] << ctb_case_;
				plc_.field_display("gap_of_case_and_result", false);
				plc_.field_display("ctb_result", false);

				set_stream_num(num);
				label_name_.caption(std::move(label_str));
				label_num_.typeface(paint::font{ "", 11, { 700 } });
				label_name_.typeface(paint::font{ "", 12, { 700 } });
				
				label_num_.transparent(true);
				label_name_.transparent(true);
				label_num_.text_align(align::left, align_v::center);
				label_name_.text_align(align::left, align_v::center);

				plc_.collocate();
			}

			void TestInStreamBox::set_min_or_max_vert_scrolled_func(std::function<void(bool)>&& functor) noexcept
			{
				ctb_case_.min_or_max_vert_scrolled_func(std::move(functor));
			}

			bool TestInStreamBox::case_empty()
			{
				return detail::hack_textbox::text_empty(ctb_case_.tb());
			}

			void TestInStreamBox::set_stream_num(std::size_t num)
			{
				if (num != 0)
					label_num_.caption(std::to_string(num));
				else
					label_num_.caption("");
			}

			TestOutStreamBox::TestOutStreamBox(nana::window wd, const std::string& label_str, std::size_t num)
				: TestInStreamBox(wd, label_str, num)
			{
				plc_["ctb_result"] << ctb_result_;
				plc_.field_display("gap_of_case_and_result", true);
				plc_.field_display("ctb_result", true);
				plc_.collocate();
				ctb_result_.editable(false);
			}

			void TestOutStreamBox::clear_result()
			{
				ctb_result_.tb().caption("");
			}

			void TestOutStreamBox::set_min_or_max_vert_scrolled_func(std::function<void(bool)>&& functor) noexcept
			{
				TestInStreamBox::set_min_or_max_vert_scrolled_func(std::move(functor));
				ctb_result_.min_or_max_vert_scrolled_func(std::move(functor));
			}

			TestInOutStreamBox::TestInOutStreamBox(nana::window wd, const std::string& label_str, std::size_t num)
				: TestOutStreamBox(wd, label_str, num)
			{
				plc_["ctb_case"] << ctb_case_out_;
				plc_.collocate();
			}

			bool TestInOutStreamBox::case_empty()
			{
				return detail::hack_textbox::text_empty(ctb_case_out_.tb())
					&& TestOutStreamBox::case_empty();
			}

			void TestInOutStreamBox::clear_result()
			{
				ctb_case_out_.tb().caption("");
			}

			void TestInOutStreamBox::set_min_or_max_vert_scrolled_func(std::function<void(bool)>&& functor) noexcept
			{
				TestOutStreamBox::set_min_or_max_vert_scrolled_func(std::move(functor));
				ctb_case_out_.min_or_max_vert_scrolled_func(std::move(functor));
			}
		}
	}
}
