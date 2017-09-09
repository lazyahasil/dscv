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
				: panel<false>(wd), options_ptree_(page.options_ptree()), streams_ptree_(page.streams_ptree())
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
				apply_grp_i18n();
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

			void JudgeConfigPanel::apply_grp_i18n()
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
					"  <weight=15 check_judging_force_endl_at_input_end>"
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
					"  <weight=15 check_comp_dont_ignore_consecutive_spaces>"
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
				grp_streams_.div(
					"vert margin=10"
					"<lb_streams>"
					"<weight=30 margin=[5,0,0,0]"
					"  <margin=[0,5,0,0] btn_add_stream>"
					"  <margin=[0,5,0,0] btn_modify_stream>"
					"  <margin=[0,5,0,0] btn_remove_stream>"
					"  <weight=25 btn_move_up_stream>"
					"  <weight=5>"
					"  <weight=25 btn_move_down_stream>"
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

				internationalization i18n;

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
					auto file_elements_generator
						= [](const ConfigHandler::Ptree& files_ptree) -> std::vector<ListStreamsElement> {
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

						return elements;
					};

					std::vector<ListStreamsElement> in_files = file_elements_generator(
						ConfigHandler::subtree(streams_ptree_, judge_stream_info::k_array_in_files));
					lb_streams_.at(static_cast<std::size_t>(ListStreamsCategory::in_files))
						.model<std::recursive_mutex>(std::move(in_files), val_translator, cell_translator);

					std::vector<ListStreamsElement> out_files = file_elements_generator(
						ConfigHandler::subtree(streams_ptree_, judge_stream_info::k_array_out_files));
					lb_streams_.at(static_cast<std::size_t>(ListStreamsCategory::out_files))
						.model<std::recursive_mutex>(std::move(out_files), val_translator, cell_translator);

					std::vector<ListStreamsElement> inout_files = file_elements_generator(
						ConfigHandler::subtree(streams_ptree_, judge_stream_info::k_array_inout_files));
					lb_streams_.at(static_cast<std::size_t>(ListStreamsCategory::inout_files))
						.model<std::recursive_mutex>(std::move(inout_files), val_translator, cell_translator);
				}

				grp_streams_.collocate();
			}

			JudgeConfigPanel::StreamAdderPanel::StreamAdderPanel(JudgeConfigPanel& config_panel)
				: nana::panel<false>(nana::window(config_panel)), config_panel_ref_(config_panel)
			{

			}
		}
	}
}
