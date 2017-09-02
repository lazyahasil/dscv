#pragma once

#include "widgets/scroll_panel.hpp"
#include "config_window.hpp"
#include "main_window/main_window_base.hpp"

namespace dscv
{
	namespace gui
	{
		//! The base class of page panels into ScrollPanel.
		class PageBase
			: public nana::panel<false>, public SideAdaptable<true>
		{
		public:
			using MainWindowBase = detail::MainWindowBase;
			using ScrollPanelPage = detail::ScrollPanelForAdaptableBase;

			explicit PageBase(nana::window wd) : nana::panel<false>(wd) { }

			PageBase() : PageBase(nullptr) { }

			virtual ~PageBase();

			//! Sets a pointer to the MainWindow which contains this page.
			//!
			//! @param main_wd_ref MainWindow which contains this page
			void main_window(MainWindowBase& main_wd_ref)
			{
				main_window_ptr_ = &main_wd_ref;
			}

			//! Opens the ConfigWindow.
			//!
			//! @param title the title of ConfigWindow
			//! @param args the arguments forwarded to the widget's constructor without the parent's handle
			//! @sa MainWindow::open_config_window()
			template <typename WidgetT, typename ...Args>
			void main_window_config_open(std::string title, Args&&... args)
			{
				if (!main_window_ptr_)
					return;
				auto config_wd = main_window_ptr_->config_window();
				config_wd.caption(title);
				config_wd.content(std::forward<Args>(args)...);
				config_wd.show();
			}

			//! Sets a pointer to the ScrollPanel which contains this page.
			//!
			//! @param scroll_panel_ref ScrollPanel which contains this page
			//! @returns false if scroll_panel_ref doesn't contain this page
			bool scroll_panel(ScrollPanelPage& scroll_panel_ref);

			void scroll_panel_make_vert_scroll(bool is_upward);

			void scroll_panel_vert_scroll_to_end();

			void scroll_panel_refresh();

		private:
			//! Simply invokes ConfigWindow::prevent_invalid_ref().
			//!
			//! @sa ConfigWindow::prevent_invalid_ref()
			void _main_window_config_prevent_invalid_ref();

			MainWindowBase* main_window_ptr_{ nullptr };
			ScrollPanelPage* scroll_panel_ptr_{ nullptr };
		};
	}
}