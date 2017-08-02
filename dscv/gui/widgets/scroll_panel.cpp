#include "scroll_panel.hpp"

using namespace nana;

namespace dscv
{
	namespace gui
	{
		namespace detail
		{
			void ScrollPanelBase::horz_scroll_to(std::size_t pos)
			{
				pos = std::min(pos, _horz_scroll_max_value());
				content_pos_.x = -static_cast<int>(pos);
				scroll_horz_.value(pos);
			}

			void ScrollPanelBase::vert_scroll_to(std::size_t pos)
			{
				pos = std::min(pos, _vert_scroll_max_value());
				content_pos_.y = -static_cast<int>(pos);
				scroll_vert_.value(pos);
			}

			void ScrollPanelBase::_init(std::unique_ptr<ContentPanelBaseType>&& content_panel)
			{
				using namespace scroll_panel;

				content_panel_ = std::move(content_panel);
				if (!content_panel_)
					throw std::runtime_error{ "WrapperPanel failed to create the content's instance!" };

				const auto weight_str = std::to_string(k_scrollbar_weight);

				plc_.div((
					std::string{} + ""
					"<vert margin=1"
					"  <"
					"    <content_panel>"
					"    <weight=" + weight_str + " scroll_vert>"
					"  >"
					"  <weight=" + weight_str + " bottom_bar"
					"    <scroll_horz>"
					"    <weight=" + weight_str + " corner>"
					"  >"
					">"
					).c_str()
				);

				plc_["content_panel"] << *content_panel_;
				plc_["scroll_vert"] << scroll_vert_;
				plc_["scroll_horz"] << scroll_horz_;

				content_panel_->umake_content_div();

				// Set default scroll steps
				scroll_vert_.step(k_scrollbar_step);
				scroll_horz_.step(k_scrollbar_step);

				events().mouse_wheel([this](const arg_wheel& arg) {
					if (arg.shift && arg.alt)
						scroll_horz_.make_scroll(!arg.upwards);
					else
						scroll_vert_.make_scroll(!arg.upwards);
				});

				content_panel_->content_widget().events().click([this] {
					nana::API::refresh_window(*this);
				});

				scroll_horz_.events().focus([this] {
					API::refresh_window(*this);
				});

				scroll_horz_.events().value_changed([this] {
					content_pos_.x = -static_cast<int>(scroll_horz_.value());
					API::refresh_window(*this);
				});

				scroll_vert_.events().focus([this] {
					API::refresh_window(*this);
				});

				scroll_vert_.events().value_changed([this] {
					content_pos_.y = -static_cast<int>(scroll_vert_.value());
					API::refresh_window(*this);
				});

				drawing{ *this }.draw([this](paint::graphics& graph) {
					const auto graph_size = graph.size();

					// The content's size on the screen (without its border and scrollbars)
					::size scr_size{ graph_size.width - 2, graph_size.height - 2 };

					// Calculate scr_size
					if (vert_forced_ || scr_size.height < content_size_.height)
					{
						scr_size.width -= k_scrollbar_weight;
						if (scr_size.width < content_size_.width)
							scr_size.height -= k_scrollbar_weight;
					}
					else if (horz_forced_ || scr_size.width < content_size_.width)
					{
						scr_size.height -= k_scrollbar_weight;
						if (scr_size.height < content_size_.height)
							scr_size.width -= k_scrollbar_weight;
					}

					// Calculate content_pos_
					if (content_pos_.x + content_size_.width < scr_size.width)
					{
						content_pos_.x = static_cast<int>(scr_size.width) - static_cast<int>(content_size_.width);
						if (content_pos_.x > 0)
							content_pos_.x = 0;
					}
					if (content_pos_.y + content_size_.height < scr_size.height)
					{
						content_pos_.y = static_cast<int>(scr_size.height) - static_cast<int>(content_size_.height);
						if (content_pos_.y > 0)
							content_pos_.y = 0;
					}

					// Set the vertical scrollbar
					const bool has_vert = vert_forced_ || scr_size.height < content_size_.height;
					plc_.field_display("scroll_vert", has_vert);
					if (has_vert)
					{
						scroll_vert_.range(scr_size.height);
						scroll_vert_.amount(content_size_.height);
						scroll_vert_.value(-content_pos_.y);
					}
					else
					{
						content_pos_.y = 0;
					}

					// Set the vertical scrollbar
					const bool has_horz = horz_forced_ || scr_size.width < content_size_.width;
					// Important: must set display of "bottom_bar", not of "scroll_horz"
					plc_.field_display("bottom_bar", has_horz);
					if (has_horz)
					{
						scroll_horz_.range(scr_size.width);
						scroll_horz_.amount(content_size_.width);
						scroll_horz_.value(-content_pos_.x);
					}
					else
					{
						content_pos_.x = 0;
					}

					// Set position of the content
					content_panel_->content_widget().move(rectangle{ content_pos_, content_size_ });

					// Draw the corner
					plc_.field_display("corner", has_vert && has_horz);
					if (has_vert && has_horz)
					{
						graph.rectangle(
							rectangle{
								static_cast<int>(graph_size.width) - static_cast<int>(k_scrollbar_weight) - 1,
								static_cast<int>(graph_size.height) - static_cast<int>(k_scrollbar_weight) - 1,
								k_scrollbar_weight,
								k_scrollbar_weight },
							true,
							colors::white_smoke
						);
					}

					// Draw the border
					const auto is_focused = scroll_vert_.focused() || scroll_horz_.focused();
					const auto& border_color{ is_focused ? k_focused_border_color : colors::gray_border };
					graph.rectangle(rectangle{ graph_size }, false, border_color);

					plc_.collocate();
				});
			}

			std::size_t ScrollPanelForAdaptableBase::content_height_for_adaption()
			{
				const auto this_size = size();
				auto in_height = this_size.height - 2;
				if (forced_horz_bar() || content_width() > this_size.width)
					in_height -= detail::scroll_panel::k_scrollbar_weight;
				return in_height;
			}

			std::size_t ScrollPanelForAdaptableBase::content_width_for_adaption()
			{
				const auto this_size = size();
				auto in_width = this_size.width - 2;
				if (forced_vert_bar() || content_height() > this_size.height)
					in_width -= detail::scroll_panel::k_scrollbar_weight;
				return in_width;
			}

			void ScrollPanelForAdaptableBase::SideAdaptableEventToggle::side_adaptable(bool is_adaptable)
			{
				if (is_adaptable)
				{
					// Check if the event exists
					if (event_handle_)
						return;

					// Make a resized event
					event_handle_ = scroll_panel_.events().resized([this](const arg_resized& arg) {
						scroll_panel_.apply_adaptable_side();
					});

					// Apply right now
					scroll_panel_.apply_adaptable_side();
				}
				else
				{
					// Remove the event
					if (event_handle_)
						scroll_panel_.events().resized.remove(event_handle_);
					event_handle_ = nullptr;
				}
			}
		}
	}
}