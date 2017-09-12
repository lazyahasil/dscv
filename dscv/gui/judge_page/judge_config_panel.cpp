#include "judge_config_panel.hpp"

#include "../judge_page.hpp"
#include "../config_gui_helper.hpp"
#include "../../judge/judge_stream_info.hpp"

#include <sstream>

using namespace nana;

namespace dscv
{
	using namespace judge;

	namespace gui
	{
		namespace judge_page
		{
			JudgeConfigPanel::JudgeConfigPanel(nana::window wd, JudgePage& page)
				: ConfigPanelBase(wd), options_ptree_(page.options_ptree()), streams_ptree_(page.streams_ptree())
			{
				plc_.div(
					"margin=5"
					"<grp_streams>"
					"<vert weight=260 margin=[0,0,0,5]"
					"  <grp_judging>"
					"  <weight=3>"
					"  <grp_comp>"
					">"
				);

				plc_["grp_streams"] << grp_streams_;
				plc_["grp_judging"] << grp_judging_;
				plc_["grp_comp"] << grp_comp_;

				// Make groups
				apply_i18n();
				_make_grp_streams();
				_make_grp_judging();
				_make_grp_comp();

				plc_.collocate();

				// Load configuration from the ptree
				_load_config_options();
			}

			JudgeConfigPanel::~JudgeConfigPanel()
			{
				// Write JSON when destroying
				config_gui_helper::write_json_noexcept();
			}

			void JudgeConfigPanel::apply_i18n()
			{
				// In Nana 1.5.5, a string is needed for nana::group's constructor.
				// Otherwise, nana::group will never be captioned with any other means.
				// And i18n_eval and typeface() doesn't work at the same time for nana::group.

				internationalization i18n;

				grp_streams_.caption(
					dynamic_cast<std::ostringstream&>(
						std::ostringstream{} << ""
						"<size=11>" << i18n("Test Streams") << "</>"
						).str().c_str()
				);

				grp_judging_.caption(
					dynamic_cast<std::ostringstream&>(
						std::ostringstream{} << ""
						"<size=11>" << i18n("Judging") << "</>"
						).str().c_str()
				);

				grp_comp_.caption(
					dynamic_cast<std::ostringstream&>(
						std::ostringstream{} << ""
						"<size=11>" << i18n("Result Comparison") << "</>"
						).str().c_str()
				);

				if (grp_streams_extra_ptr_)
					grp_streams_extra_ptr_->apply_i18n();
			}

			void JudgeConfigPanel::_init_checkbox_and_label(
				const nana::group& grp,
				nana::checkbox& checkbox,
				nana::label& label,
				const std::string& option_str
			)
			{
				// nana::checkbox::bgcolor() is more efficient than nana::checkbox::transparent().
				checkbox.bgcolor(grp.bgcolor());
				API::effects_edge_nimbus(checkbox, effects::edge_nimbus::active);
				API::tabstop(checkbox);

				// Bind the checkbox with options_ptree_
				checkbox.events().checked([this, option_str](const arg_checkbox& arg) {
					options_ptree_.put(option_str, arg.widget->checked());
				});

				label.click_for(checkbox);
				label.events().click([&checkbox] {
					checkbox.check(!checkbox.checked());
				});
			}

			void JudgeConfigPanel::_load_config_options()
			{
				// Group "judging"
				{
					check_judging_force_endl_at_input_end_.check(
						options_ptree_.get(options::k_judging_force_endl_at_input_end, false)
					);
				}

				// Group "comp"
				{
					check_comp_dont_ignore_consecutive_spaces_.check(
						options_ptree_.get(options::k_comp_dont_ignore_consecutive_spaces, false)
					);
				}
			}

			void JudgeConfigPanel::_make_grp_judging()
			{
				grp_judging_.div(
					"vert margin=10"
					"<>"
					"<weight=40 margin=[0,0,5,0]"
					"  <vert weight=15"
					"    <weight=15 check_judging_force_endl_at_input_end>"
					"  >"
					"  <margin=[0,0,0,3] label_judging_force_endl_at_input_end>"
					">"
					"<>"
				);
				grp_judging_["check_judging_force_endl_at_input_end"]
					<< check_judging_force_endl_at_input_end_;
				grp_judging_["label_judging_force_endl_at_input_end"]
					<< label_judging_force_endl_at_input_end_;

				// Set i18n
				label_judging_force_endl_at_input_end_.i18n(
					i18n_eval{ "_opt_judge_judging_force_endl_at_input_end" });

				_init_checkbox_and_label(
					grp_judging_,
					check_judging_force_endl_at_input_end_,
					label_judging_force_endl_at_input_end_,
					options::k_judging_force_endl_at_input_end
				);

				grp_judging_.collocate();
			}

			void JudgeConfigPanel::_make_grp_comp()
			{
				grp_comp_.div(
					"vert margin=10"
					"<>"
					"<weight=27 margin=[0,0,5,0]"
					"  <label_comp_sorry_for_incompletion>"
					">"
					"<weight=27 margin=[0,0,5,0]"
					"  <vert weight=15"
					"    <weight=15 check_comp_dont_ignore_consecutive_spaces>"
					"  >"
					"  <margin=[0,0,0,3] label_comp_dont_ignore_consecutive_spaces>"
					">"
					"<>"
				);
				grp_comp_["label_comp_sorry_for_incompletion"]
					<< label_comp_sorry_for_incompletion_;
				grp_comp_["check_comp_dont_ignore_consecutive_spaces"]
					<< check_comp_dont_ignore_consecutive_spaces_;
				grp_comp_["label_comp_dont_ignore_consecutive_spaces"]
					<< label_comp_dont_ignore_consecutive_spaces_;

				// Set i18n
				label_comp_dont_ignore_consecutive_spaces_.i18n(
					i18n_eval{ "_opt_judge_comp_dont_ignore_consecutive_spaces" });

				_init_checkbox_and_label(
					grp_comp_,
					check_comp_dont_ignore_consecutive_spaces_,
					label_comp_dont_ignore_consecutive_spaces_,
					options::k_comp_dont_ignore_consecutive_spaces
				);

				grp_comp_.collocate();
			}

			void JudgeConfigPanel::_make_grp_streams()
			{
				plc_streams_.div(
					"vert margin=[25,10,10,10]"
					"<lb_streams>"
					"<weight=30 margin=[5,0,0,0]"
					"  <margin=[0,5,0,0] btn_add_stream>"
					"  <margin=[0,5,0,0] btn_modify_stream>"
					"  <margin=[0,5,0,0] btn_remove_stream>"
					"  <weight=25 btn_move_up_stream>"
					"  <weight=5>"
					"  <weight=25 btn_move_down_stream>"
					">"
					"<weight=120 margin=[7,0,0,0] grp_streams_extra>"
				);
				plc_streams_["lb_streams"] << lb_streams_;
				plc_streams_["btn_add_stream"] << btn_streams_add_;
				plc_streams_["btn_modify_stream"] << btn_streams_modify_;
				plc_streams_["btn_remove_stream"] << btn_streams_remove_;
				plc_streams_["btn_move_up_stream"] << btn_streams_move_up_;
				plc_streams_["btn_move_down_stream"] << btn_streams_move_down_;
				plc_streams_.field_display("grp_streams_extra", false);

				// Set i18n
				btn_streams_add_.i18n(i18n_eval{ "Add" });
				btn_streams_modify_.i18n(i18n_eval{ "Modify" });
				btn_streams_remove_.i18n(i18n_eval{ "Remove" });

				btn_streams_move_up_.enabled(false); // Not implemented yet
				btn_streams_move_down_.enabled(false); // Not implemented yet

				// Make the listbox
				_make_grp_streams_lb();

				btn_streams_add_.events().click([this] {
					grp_streams_extra_ptr_ = std::make_unique<StreamAdderGroup>(grp_streams_, *this);
					plc_streams_["grp_streams_extra"] << *grp_streams_extra_ptr_;
					plc_streams_.field_display("grp_streams_extra", true);
					plc_streams_.collocate();
				});

				btn_streams_modify_.events().click([this] {
					grp_streams_extra_ptr_ = std::make_unique<StreamModifierGroup>(grp_streams_, *this);
					plc_streams_["grp_streams_extra"] << *grp_streams_extra_ptr_;
					plc_streams_.field_display("grp_streams_extra", true);
					plc_streams_.collocate();
				});

				plc_streams_.collocate();
			}

			void JudgeConfigPanel::_make_grp_streams_lb()
			{
				internationalization i18n;

				API::tabstop(lb_streams_);

				lb_streams_.sortable(false); // Prevent sorting
				lb_streams_.enable_single(true, false); // Prevent selecting multiples

				lb_streams_.append_header(i18n("Num"), 45);
				lb_streams_.append_header(i18n("File Name"), 210);
				lb_streams_.append_header(i18n("Media"), 60);

				lb_streams_.append(i18n("Console Streams"));
				lb_streams_.append(i18n("Input Files"));
				lb_streams_.append(i18n("Output Files"));
				lb_streams_.append(i18n("I/O Hybrid Files"));

				// Set the standalone model and push data

				auto val_translator = [](const std::vector<nana::listbox::cell>& cells) {
					ListStreamsElement element;
					element.num = std::stoul(cells[static_cast<std::size_t>(ListStreamsHeader::num)].text);
					element.filename = cells[static_cast<std::size_t>(ListStreamsHeader::filename)].text;
					element.media = cells[static_cast<std::size_t>(ListStreamsHeader::media)].text;
					return element;
				};

				auto cell_translator = [](const ListStreamsElement& val) {
					std::vector<nana::listbox::cell> cells;
					if (val.num == 0)
						cells.emplace_back("");
					else
						cells.emplace_back(std::to_string(val.num));
					cells.emplace_back(val.filename);
					cells.emplace_back(val.media);
					return cells;
				};

				// Console elements
				{
					std::vector<ListStreamsElement> console;
					internationalization i18n;

					auto has_stdin = streams_ptree_.get(judge_stream_info::k_has_stdin, false);
					auto has_stdout = streams_ptree_.get(judge_stream_info::k_has_stdout, false);

					if (has_stdin)
						console.emplace_back(0, i18n("Console Input"), judge_stream_info::file_media::k_text);
					if (has_stdout)
						console.emplace_back(0, i18n("Console Output"), judge_stream_info::file_media::k_text);

					lb_streams_.at(static_cast<std::size_t>(ListStreamsCategory::console))
						.model<std::recursive_mutex>(std::move(console), val_translator, cell_translator);
				}

				// File elements
				{
					auto file_elements_generator = [
						this,
						val_translator{ std::move(val_translator) },
						cell_translator{ std::move(cell_translator) }
					](std::size_t lb_cat_pos, const std::string& files_key) {
						auto& files_ptree = ConfigHandler::subtree(streams_ptree_, files_key);
						std::vector<ListStreamsElement> elements;
						std::size_t count = 1;

						for (const auto& val : files_ptree)
						{
							auto media = val.second.get_optional<std::string>(judge_stream_info::file_info::k_media);
							auto filename = val.second.get(judge_stream_info::file_info::k_name, "");
							if (!media)
								media = judge_stream_info::file_media::k_text;
							elements.emplace_back(count++, std::move(filename), std::move(*media));
						}

						lb_streams_.at(lb_cat_pos).model<std::recursive_mutex>(
							std::move(elements), val_translator, cell_translator);
					};

					file_elements_generator(
						static_cast<std::size_t>(ListStreamsCategory::in_files),
						judge_stream_info::k_array_in_files
					);
					file_elements_generator(
						static_cast<std::size_t>(ListStreamsCategory::out_files),
						judge_stream_info::k_array_out_files
					);
					file_elements_generator(
						static_cast<std::size_t>(ListStreamsCategory::inout_files),
						judge_stream_info::k_array_inout_files
					);
				}

				lb_streams_.events().focus([this](const arg_focus& arg) {
					if (arg.getting)
						lb_streams_.scheme().item_selected = static_cast<color_rgb>(0xcce8ff);
					else
						lb_streams_.scheme().item_selected = colors::light_gray;
					API::refresh_window(lb_streams_);
				});
			}

			JudgeConfigPanel::StreamExtraGroupBase::StreamExtraGroupBase(
				nana::window wd, JudgeConfigPanel& config_panel
			) : nana::group(wd, "", true), config_panel_ref_(config_panel)
			{
				plc_.div(
					"vert margin=[25,10,10,10]"
					"<weight=20"
					"  <weight=55%"
					"    <weight=70 label_type>"
					"    <margin=[0,0,0,3] combo_type>"
					"  >"
					"  <weight=5%>"
					"  <"
					"    <weight=49 label_media>"
					"    <margin=[0,0,0,3] combo_media>"
					"  >"
					">"
					"<weight=5>"
					"<weight=20"
					"  <weight=70 label_filename>"
					"  <margin=[0,0,0,3] tb_filename>"
					">"
					"<weight=7>"
					"<"
					"  <weight=80 btn_cancel>"
					"  <>"
					"  <weight=80 btn_action>"
					">"
				);

				plc_["label_type"] << label_type_;
				plc_["combo_type"] << combo_type_;
				plc_["label_media"] << label_media_;
				plc_["combo_media"] << combo_media_;
				plc_["label_filename"] << label_filename_;
				plc_["tb_filename"] << tb_filename_;
				plc_["btn_cancel"] << btn_cancel_;

				// Set i18n
				label_type_.i18n(i18n_eval{ "I/O Type" });
				label_media_.i18n(i18n_eval{ "Media" });
				label_filename_.i18n(i18n_eval{ "File Name" });
				btn_cancel_.i18n(i18n_eval{ "Cancel" });

				// Set labels' text alignment
				label_type_.text_align(align::left, align_v::center);
				label_media_.text_align(align::left, align_v::center);
				label_filename_.text_align(align::left, align_v::center);

				// Set tb_filename_
				tb_filename_.multi_lines(false);
			}

			void JudgeConfigPanel::StreamExtraGroupBase::_apply_i18n_styled(const std::string& str_translated)
			{
				caption(
					dynamic_cast<std::ostringstream&>(
						std::ostringstream{} << ""
						"<size=11>" << str_translated << "</>"
						).str().c_str()
				);
			}

			JudgeConfigPanel::StreamAdderGroup::StreamAdderGroup(nana::window wd, JudgeConfigPanel& config_panel)
				: StreamExtraGroupBase(wd, config_panel)
			{
				plc_["btn_action"] << btn_add_;

				// Set i18n
				apply_i18n();
				btn_add_.i18n(i18n_eval{ "Add" });

				plc_.collocate();
			}

			void JudgeConfigPanel::StreamAdderGroup::apply_i18n()
			{
				internationalization i18n;
				_apply_i18n_styled(i18n("Stream Addition"));
			}

			JudgeConfigPanel::StreamModifierGroup::StreamModifierGroup(nana::window wd, JudgeConfigPanel& config_panel)
				: StreamExtraGroupBase(wd, config_panel)
			{
				plc_["btn_action"] << btn_modify_;

				// Set i18n
				apply_i18n();
				btn_modify_.i18n(i18n_eval{ "Modify" });

				plc_.collocate();
			}

			void JudgeConfigPanel::StreamModifierGroup::apply_i18n()
			{
				internationalization i18n;
				_apply_i18n_styled(i18n("Stream Modification"));
			}
		}
	}
}
