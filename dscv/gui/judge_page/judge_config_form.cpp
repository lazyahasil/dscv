#include "judge_config_form.hpp"

#include "../judge_page.hpp"

using namespace nana;

namespace dscv
{
	namespace gui
	{
		namespace judge_page
		{
			JudgeConfigForm::JudgeConfigForm(nana::window wd, JudgePage& page)
				: form(wd, API::make_center(wd, 640, 540), appear::decorate<appear::sizable>()),
				  page_ref_(page)
			{
				// Set the form window's title
				this->i18n(i18n_eval{ "Judging Configuration" });

				// Set minimum window size
				API::track_window_size(*this, { 540, 360 }, false);

				plc_.div(
					"margin=5"
					"<grp_streams>"
					"<vert weight=280 margin=[0,0,0,5]"
					"  <grp_process>"
					"  <grp_result_comp>"
					">"
					""
				);
				plc_["grp_streams"] << grp_streams_;
				plc_["grp_process"] << grp_process_;
				plc_["grp_result_comp"] << grp_result_comp_;

				apply_grp_i18n();
				_make_grp_streams();
				_make_grp_process();
				_make_grp_result_comp();

				plc_.collocate();
			}

			void JudgeConfigForm::apply_grp_i18n()
			{
				// In Nana 1.5.4, a string is needed for nana::group's constructor.
				// Otherwise, nana::group will never be captioned with any other means.
				// And i18n_eval and typeface() doesn't work for nana::group.

				internationalization i18n;

				grp_streams_.caption(
					dynamic_cast<std::ostringstream&>(
						std::ostringstream{} << ""
						"<size=11>" << i18n("Test Streams") << "</>"
						).str().c_str()
				);

				grp_process_.caption(
					dynamic_cast<std::ostringstream&>(
						std::ostringstream{} << ""
						"<size=11>" << i18n("Processes") << "</>"
						).str().c_str()
				);

				grp_result_comp_.caption(
					dynamic_cast<std::ostringstream&>(
						std::ostringstream{} << ""
						"<size=11>" << i18n("Comparison") << "</>"
						).str().c_str()
				);
			}

			void JudgeConfigForm::_make_grp_process()
			{
				
			}

			void JudgeConfigForm::_make_grp_result_comp()
			{
								
			}

			void JudgeConfigForm::_make_grp_streams()
			{
				grp_streams_.div(
					"<vert margin=10"
					"  <lb_streams>"
					"  <weight=30 margin=[5,0,0,0]"
					"    <margin=[0,5,0,0] btn_add_stream>"
					"    <margin=[0,5,0,0] btn_modify_stream>"
					"    <margin=[0,5,0,0] btn_remove_stream>"
					"    <weight=25 btn_move_up_stream>"
					"    <weight=5>"
					"    <weight=25 btn_move_down_stream>"
					"  >"
					">"
				);
				grp_streams_["lb_streams"] << lb_streams_;
				grp_streams_["btn_add_stream"] << btn_add_stream_;
				grp_streams_["btn_modify_stream"] << btn_modify_stream_;
				grp_streams_["btn_remove_stream"] << btn_remove_stream_;
				grp_streams_["btn_move_up_stream"] << btn_move_up_stream_;
				grp_streams_["btn_move_down_stream"] << btn_move_down_stream_;

				// Set i18n
				btn_add_stream_.i18n(i18n_eval{ "Add" });
				btn_modify_stream_.i18n(i18n_eval{ "Modify" });
				btn_remove_stream_.i18n(i18n_eval{ "Remove" });

				grp_streams_.collocate();
			}
		}
	}
}
