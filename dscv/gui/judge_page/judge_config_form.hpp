#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/listbox.hpp>

namespace dscv
{
	namespace gui
	{
		class JudgePage;

		namespace judge_page
		{
			//! The settings window for JudgePage
			class JudgeConfigForm : public nana::form
			{
			public:
				JudgeConfigForm() = delete;
				JudgeConfigForm(nana::window wd, JudgePage& page);

				void apply_grp_i18n();

			private:
				static void _init_checkbox_and_label(
					const nana::group& grp, nana::checkbox& checkbox, nana::label& label
				);

				void _make_grp_judging();
				void _make_grp_comp();
				void _make_grp_streams();

				JudgePage& page_ref_;

				nana::place plc_{ *this };

				nana::group grp_streams_{ *this, "", true };
				nana::listbox lb_streams_{ grp_streams_ };
				nana::button btn_add_stream_{ grp_streams_ };
				nana::button btn_modify_stream_{ grp_streams_ };
				nana::button btn_remove_stream_{ grp_streams_ };
				nana::button btn_move_up_stream_{ grp_streams_, u8"\u2b06" };
				nana::button btn_move_down_stream_{ grp_streams_, u8"\u2b07" };

				nana::group grp_judging_{ *this, "", true };
				//! Option whether to insert endl to output's end if not existing
				nana::checkbox check_judging_add_endl_to_test_case_input_end_{ grp_judging_ };
				nana::label label_judging_add_endl_to_test_case_input_end_{ grp_judging_ };
				
				nana::group grp_comp_{ *this, "", true };
				//! Option whether to not ignore consecutive spaces while comparsion
				nana::checkbox check_comp_dont_ignore_consecutive_spaces_{ grp_comp_ };
				nana::label label_comp_dont_ignore_consecutive_spaces_{ grp_comp_ };
			};
		}
	}
}
