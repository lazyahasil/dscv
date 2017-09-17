#pragma once

#include "../config_panel_base.hpp"
#include "../../config_handler.hpp"

#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/textbox.hpp>

namespace dscv
{
	namespace gui
	{
		class JudgePage;

		namespace judge_page
		{
			namespace options
			{
				constexpr const char* k_path_str = "options";
				constexpr const char* k_judging_force_endl_at_input_end
					= "judging_force_endl_at_input_end";
				constexpr const char* k_comp_dont_ignore_consecutive_spaces
					= "comp_dont_ignore_consecutive_spaces";
			}

			namespace config_panel
			{
				constexpr const char* k_path_str = "config_panel";
				constexpr const char* k_window_width = "window_width";
				constexpr const char* k_window_height = "window_height";

				constexpr const char* k_list_streams_num_width = "list_streams_num_width";
				constexpr const char* k_list_streams_list_media_width = "list_streams_media_width";
				constexpr const char* k_list_streams_list_filename_width = "list_streams_filename_width";
			}

			//! The settings window for JudgePage
			class JudgeConfigPanel : public ConfigPanelBase
			{
			private:
				class StreamExtraGroupBase;
				class StreamAdderGroup;
				class StreamModifierGroup;

				struct ListStreamsElement;

			public:
				JudgeConfigPanel() = delete;
				JudgeConfigPanel(nana::window wd, JudgePage& page);

				~JudgeConfigPanel();

				//! Applies i18n to all the group.
				void apply_i18n() override;

				JudgePage& page() noexcept
				{
					return page_ref_;
				}

				void remove_stream_extra_group();

			private:
				//! Initiates a option pair of a checkbox and a label in the same group.
				//!
				//! This was made since this is a common trifle for every checkbox and label of every option in
				//! every group.\n
				//! By this, appearance settings, mutual binding and adding events are done.
				//! @param grp the group which contains the checkbox and the label
				//! @param checkbox nana::checkbox widget of the option
				//! @param label nana::label widget of the option
				//! @param option_str option string for the ptree
				void _init_checkbox_and_label(
					const nana::group& grp,
					nana::checkbox& checkbox,
					nana::label& label,
					const std::string& option_str
				);

				//! Loads configurations from the ptree into widgets.
				void _load_config_options();

				//! Makes the group "judging".
				void _make_grp_judging();

				//! Makes the group "comp".
				void _make_grp_comp();

				//! Makes the group "streams"
				void _make_grp_streams();

				//! Initiates nana::listbox at the group "streams"
				//!
				//! Using STL containers shows much better performance.
				//! http://nanapro.org/en-us/blog/2016/07/3-methods-to-insert-data-into-listbox/
				void _make_grp_streams_lb();

				enum class ListStreamsCategory : std::size_t
				{
					unclassified,
					console,
					in_files,
					out_files,
					inout_files,
				};

				enum class ListStreamsHeader : std::size_t
				{
					num,
					filename,
					media
				};

				JudgePage& page_ref_;
				ConfigHandler::Ptree& options_ptree_;
				ConfigHandler::Ptree& streams_ptree_;

				nana::place plc_{ *this };

				struct ListStreamsElement
				{
					ListStreamsElement() = default;

					template <typename StringT1, typename StringT2>
					ListStreamsElement(std::size_t _num, StringT1&& _filename, StringT2&& _media)
						: num(_num),
						  filename(std::forward<StringT1>(_filename)),
						  media(std::forward<StringT2>(_media))
					{ }

					std::size_t num{ 0 };
					std::string filename;
					std::string media;
				};

				nana::group grp_streams_{ *this, "", true };
				nana::place plc_streams_{ grp_streams_ };

				nana::listbox lb_streams_{ grp_streams_ };

				nana::button btn_streams_add_{ grp_streams_ };
				nana::button btn_streams_modify_{ grp_streams_ };
				nana::button btn_streams_remove_{ grp_streams_ };
				nana::button btn_streams_move_up_{ grp_streams_, u8"\u2b06" };
				nana::button btn_streams_move_down_{ grp_streams_, u8"\u2b07" };

				std::unique_ptr<StreamExtraGroupBase> grp_streams_extra_ptr_;

				nana::group grp_judging_{ *this, "", true };

				//! Option whether to insert endl to output's end if not existing
				nana::checkbox check_judging_force_endl_at_input_end_{ grp_judging_ };
				//! Option whether to insert endl to output's end if not existing
				nana::label label_judging_force_endl_at_input_end_{ grp_judging_ };

				nana::group grp_comp_{ *this, "", true };
				nana::label label_comp_sorry_for_incompletion_{ grp_comp_, "Sorry, comparison is WIP." };

				//! Option whether to not ignore consecutive spaces while comparsion
				nana::checkbox check_comp_dont_ignore_consecutive_spaces_{ grp_comp_ };
				//! Option whether to not ignore consecutive spaces while comparsion
				nana::label label_comp_dont_ignore_consecutive_spaces_{ grp_comp_ };
			};

			class JudgeConfigPanel::StreamExtraGroupBase : public nana::group
			{
			public:
				StreamExtraGroupBase() = delete;
				StreamExtraGroupBase(nana::window wd, JudgeConfigPanel& config_panel);

				virtual ~StreamExtraGroupBase() = default;

				virtual void apply_i18n() = 0;

			protected:
				void _apply_i18n_styled(const std::string& str_translated);

				JudgeConfigPanel& config_panel_ref_;

				nana::place plc_{ *this };

				nana::label label_type_{ *this };
				nana::combox combo_type_{ *this };
				nana::label label_media_{ *this };
				nana::combox combo_media_{ *this };
				nana::label label_filename_{ *this };
				nana::textbox tb_filename_{ *this };

			private:
				nana::button btn_cancel_{ *this };
			};

			class JudgeConfigPanel::StreamAdderGroup : public JudgeConfigPanel::StreamExtraGroupBase
			{
			public:
				StreamAdderGroup() = delete;
				StreamAdderGroup(nana::window wd, JudgeConfigPanel& config_panel);

				void apply_i18n() override;

			private:
				nana::button btn_add_{ *this };
			};

			class JudgeConfigPanel::StreamModifierGroup : public JudgeConfigPanel::StreamExtraGroupBase
			{
			public:
				StreamModifierGroup() = delete;
				StreamModifierGroup(nana::window wd, JudgeConfigPanel& config_panel);

				void apply_i18n() override;

			private:
				nana::button btn_modify_{ *this };
			};
		}
	}
}
