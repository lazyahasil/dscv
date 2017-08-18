#include "detail/hack_textbox.hpp" // Include it first

#include "code_textbox.hpp"

using namespace nana;

namespace dscv
{
	namespace gui
	{
		using namespace detail::code_textbox;

		CodeTextbox::CodeTextbox(nana::window wd) : panel<true>(wd)
		{
			bgcolor(colors::white);

			plc_.div(
				"<margin=1"
				"  <weight=16 line_num>"
				"  <margin=[0,0,0,6] textbox>"
				">"
			);
			plc_["line_num"] << line_num_;
			plc_["textbox"] << textbox_;

			line_num_.bgcolor(k_default_line_num_bgcolor);
			textbox_.borderless(true);

			API::effects_edge_nimbus(textbox_, effects::edge_nimbus::none);
			API::tabstop(textbox_);
			API::eat_tabstop(textbox_, false);

			plc_.collocate();

			textbox_.events().mouse_down([this](const arg_mouse& arg) {
				textbox_.focus();
				CodeTextboxPopupMenu::instance().open_menu(*this, arg);
			});

			events().focus([this] {
				textbox_.focus();
			});

			line_num_.events().click([this] {
				textbox_.focus();
			});

			textbox_.events().focus([this] {
				API::refresh_window(*this);
			});

			textbox_.events().text_exposed([this] {
				API::refresh_window(*this);
				API::refresh_window(line_num_);
			});

			textbox_.events().mouse_wheel([this](const arg_wheel& arg) {
				// Beware that the mouse wheel does effect even if not focused
				API::refresh_window_tree(*this);
				if (!min_or_max_vert_scrolled_func_)
					return;
				if (arg.upwards)
				{
					if (detail::hack_textbox::check_if_min_vert_scrolled(textbox_))
						min_or_max_vert_scrolled_func_(true);
				}
				else if (detail::hack_textbox::check_if_max_vert_scrolled(textbox_))
				{
					min_or_max_vert_scrolled_func_(false);
				}
			});

			// Draw the border
			drawing{ *this }.draw([this](paint::graphics& graph) {
				const auto& border_color{ textbox_.focused() ? k_focused_border_color : colors::gray_border };
				graph.rectangle(rectangle{ graph.size() }, false, border_color);
			});

			// Draw the line numbers
			drawing{ line_num_ }.draw([this](paint::graphics& graph) {
				// Based on Nana 1.5.4 example
				// https://github.com/qPCR4vir/nana-demo/blob/master/Examples/textbox_line_number.cpp

				if (textbox_.empty()) // return if textbox_ is not initiated
					return;

				const auto text_pos = textbox_.text_position();

				const auto log10_of_largest_num = static_cast<std::size_t>(std::log10(text_pos.back().y + 1));
				const auto width = log10_of_largest_num * graph.text_extent_size("0").width + 16;

				// Check whether the current width is suitable or not
				const auto current_width = graph.width();
				if (current_width != 0 && current_width != width)
				{
					const auto attr_str = std::string("weight=") + std::to_string(width);
					plc_.modify("line_num", attr_str.c_str());
					plc_.collocate();
				}

				const std::size_t inner_width = width - 4;

				auto line_height = textbox_.line_pixels();
				int top_offset = textbox_.text_area().y;

				if (line_height != 0) // It prevents "divided by zero" when there's no text
					top_offset -= textbox_.content_origin().y % line_height;
				else
					top_offset -= 2;

				// Draw the line numbers
				for (const auto& pos : text_pos)
				{
					const auto num_str = std::to_string(pos.y + 1);
					const auto num_pixels = graph.text_extent_size(num_str).width;
					const auto num_bgcolor = line_num_bgcolor_func_(pos.y);

					if (num_bgcolor != line_num_panel_bgcolor_)
					{
						graph.rectangle(
							rectangle{ 3, top_offset, inner_width, line_height },
							true,
							num_bgcolor
						);
					}

					graph.string(
						point{ static_cast<int>(inner_width - num_pixels), top_offset },
						num_str,
						line_num_fgcolor_func_(pos.y)
					);

					top_offset += line_height;
				}
			});
		}

		CodeTextbox::~CodeTextbox()
		{
			// Prevent a dangling pointer of the popup menu
			CodeTextboxPopupMenu::instance().prevent_invalid_ref(*this);
		}

		void CodeTextbox::action_copy()
		{
			if (!textbox_.selected())
				return;
			textbox_.copy();
		}

		void CodeTextbox::action_paste()
		{
			if (textbox_.editable())
				textbox_.paste();
		}

		void CodeTextbox::apply_i18n()
		{
			CodeTextboxPopupMenu::instance().apply_i18n();
		}

		void CodeTextbox::editable(bool is_editable)
		{
			textbox_.editable(is_editable);
			textbox_.enable_caret();
		}

		void CodeTextbox::min_or_max_vert_scrolled_func(std::function<void(bool)> functor) noexcept
		{
			min_or_max_vert_scrolled_func_ = std::move(functor);
		}

		void CodeTextbox::line_num_bgcolor_func(std::function<nana::color(std::size_t)> functor) noexcept
		{
			line_num_bgcolor_func_ = std::move(functor);
			line_num_panel_bgcolor_ = line_num_bgcolor_func_(0);
			line_num_.bgcolor(line_num_panel_bgcolor_);
		}

		void CodeTextbox::line_num_fgcolor_func(std::function<nana::color(std::size_t)> functor) noexcept
		{
			line_num_fgcolor_func_ = std::move(functor);
		}

		CodeTextbox::CodeTextboxPopupMenu::CodeTextboxPopupMenu()
		{
			append("", [this](menu::item_proxy& ip) {
				if (ctb_ptr_)
					ctb_ptr_->action_copy();
			});

			append("", [this](menu::item_proxy& ip) {
				if (ctb_ptr_)
					ctb_ptr_->action_paste();
			});

			append_splitter();

			append("", [this](menu::item_proxy& ip) {
				if (ctb_ptr_)
					ctb_ptr_->action_select_all();
			});

			// Make texts
			apply_i18n();
		}

		void CodeTextbox::CodeTextboxPopupMenu::apply_i18n()
		{
			internationalization i18n;
			text(0, i18n("Copy") + " (Ctrl+C)");
			text(1, i18n("Paste") + " (Ctrl+V)");
			text(3, i18n("Select All") + " (Ctrl+A)");
		}

		void CodeTextbox::CodeTextboxPopupMenu::open_menu(CodeTextbox& ctb, const nana::arg_mouse& mouse)
		{
			std::lock_guard<std::recursive_mutex> lock{ mutex_ };
			ctb_ptr_ = &ctb;
			enabled(1, ctb_ptr_->editable());
			// " menu_popuper(*this)(mouse); " is interpreted as a variable by Cppcheck
			auto popuper = menu_popuper(*this);
			popuper(mouse);
		}

		void CodeTextbox::CodeTextboxPopupMenu::prevent_invalid_ref(const CodeTextbox& ctb) noexcept
		{
			std::lock_guard<std::recursive_mutex> lock{ mutex_ };
			if (&ctb == ctb_ptr_)
			{
				close();
				ctb_ptr_ = nullptr;
			}
		}
	}
}