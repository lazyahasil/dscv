#pragma once

#include "../../misc/singleton.hpp"

#include <nana/gui.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/textbox.hpp>

#include <mutex>

namespace dscv
{
	namespace gui
	{
		namespace detail
		{
			namespace code_textbox
			{
				const nana::color k_default_line_num_bgcolor = nana::colors::white;
				const nana::color k_default_line_num_fgcolor = nana::colors::gray_border;
				const nana::color k_focused_border_color = nana::color_rgb(0x0595e2U);
				constexpr int k_refresh_timer_interval = 10;
			}
		}

		class CodeTextbox : public nana::panel<true>
		{
		private:
			class CodeTextboxPopupMenu;

		public:
			explicit CodeTextbox(nana::window wd);

			CodeTextbox() : CodeTextbox(nullptr) { }

			~CodeTextbox();

			void action_copy();
			void action_paste();

			void action_select_all()
			{
				textbox_.select(true);
			}

			static void apply_i18n();

			bool editable() const noexcept
			{
				return textbox_.editable();
			}

			void editable(bool is_editable);

			void min_or_max_vert_scrolled_func(std::function<void(bool)> functor) noexcept;
			void line_num_bgcolor_func(std::function<nana::color(std::size_t)> functor) noexcept;
			void line_num_fgcolor_func(std::function<nana::color(std::size_t)> functor) noexcept;

			nana::textbox& tb() noexcept
			{
				return textbox_;
			}

			const nana::textbox& tb() const noexcept
			{
				return textbox_;
			}

		private:
			nana::place plc_{ *this };
			nana::panel<true> line_num_{ nana::window(*this) };
			nana::textbox textbox_{ *this };

			nana::color line_num_panel_bgcolor_{ detail::code_textbox::k_default_line_num_bgcolor };

			std::function<void(bool)> min_or_max_vert_scrolled_func_;
			std::function<nana::color(std::size_t)> line_num_bgcolor_func_{ [&](std::size_t) {
				return detail::code_textbox::k_default_line_num_bgcolor;
			} };
			std::function<nana::color(std::size_t)> line_num_fgcolor_func_{ [&](std::size_t) {
				return detail::code_textbox::k_default_line_num_fgcolor;
			} };
		};

		class CodeTextbox::CodeTextboxPopupMenu
			: public nana::menu, public Singleton<CodeTextboxPopupMenu>
		{
		public:
			CodeTextboxPopupMenu();

			CodeTextboxPopupMenu(const CodeTextboxPopupMenu&) = delete;
			CodeTextboxPopupMenu& operator=(const CodeTextboxPopupMenu&) = delete;

			void apply_i18n();
			void open_menu(CodeTextbox& ctb, const nana::arg_mouse& mouse);
			void prevent_invalid_ref(const CodeTextbox& ctb) noexcept;

		private:
			CodeTextbox* ctb_ptr_{ nullptr };
			std::recursive_mutex mutex_;
		};
	}
}