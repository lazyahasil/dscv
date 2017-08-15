#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
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
				void _make_grp_process();
				void _make_grp_result_comp();
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

				nana::group grp_process_{ *this, "", true };
				
				nana::group grp_result_comp_{ *this, "", true };
			};
		}
	}
}
