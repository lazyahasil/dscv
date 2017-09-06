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

			CodeTextbox(const CodeTextbox&) = delete;
			CodeTextbox& operator=(const CodeTextbox&) = delete;

			~CodeTextbox();

			//! Invokes nana::textbox::copy().
			void action_copy();

			//! Invokes nana::textbox::paste().
			void action_paste();

			//! Invokes nana::textbox::select(bool).
			void action_select_all()
			{
				textbox_.select(true);
			}

			//! Applies i18n to CodeTextbox::CodeTextboxPopupMenu.
			static void apply_i18n();

			//! Returns whether the textbox is editable.
			bool editable() const noexcept
			{
				return textbox_.editable();
			}

			//! Toggles whether it's editable by a boolean.
			//!
			//! @param is_editable whether to make it editable or not
			void editable(bool is_editable);

			//! Sets the function which is called when vertically scrolled to the end.
			//!
			//! @param functor std::function object
			template <typename FunctorT>
			void min_or_max_vert_scrolled_func(FunctorT&& functor) noexcept
			{
				min_or_max_vert_scrolled_func_ = std::forward<FunctorT>(functor);
			}

			//! Sets the function which decides background colors of line numbers.
			//!
			//! @param functor std::function object
			template <typename FunctorT>
			void line_num_bgcolor_func(FunctorT&& functor) noexcept
			{
				line_num_bgcolor_func_ = std::forward<FunctorT>(functor);
				line_num_panel_bgcolor_ = line_num_bgcolor_func_(0);
				line_num_.bgcolor(line_num_panel_bgcolor_);
			}

			//! Sets the function which decides foreground colors of line numbers.
			//!
			//! @param functor std::function object
			template <typename FunctorT>
			void line_num_fgcolor_func(FunctorT&& functor) noexcept
			{
				line_num_fgcolor_func_ = std::forward<FunctorT>(functor);
			}

			//! Returns a reference to the textbox
			nana::textbox& tb() noexcept
			{
				return textbox_;
			}

			//! Returns a const reference to the textbox
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

			//! Applies i18n.
			void apply_i18n();

			//! Opens the menu.
			//!
			//! @param ctb reference to the CodeTextbox
			//! @param mouse nana::arg_mouse object which is passed on the mouse_down event
			void open_menu(CodeTextbox& ctb, const nana::arg_mouse& mouse);

			//! Nullifies the reference if the CodeTextbox matches.
			//!
			//! @ctb reference to the CodeTextbox which is destroying
			void prevent_invalid_ref(const CodeTextbox& ctb) noexcept;

		private:
			CodeTextbox* ctb_ptr_{ nullptr };
			std::recursive_mutex mutex_;
		};
	}
}