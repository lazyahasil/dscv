#pragma once

#include "widgets/scroll_panel.hpp"

namespace dscv
{
	namespace gui
	{
		//! The base class of page panels into ScrollPanel.
		class PageBase
			: public nana::panel<false>, public SideAdaptable<true>
		{
		public:
			explicit PageBase(nana::window wd) : nana::panel<false>(wd) { }

			PageBase() : PageBase(nullptr) { }

			void scroll_panel_make_vert_scroll(bool is_upward);
			void scroll_panel_vert_scroll_to_end();
			void scroll_panel_refresh();

			//! Sets a pointer to the ScrollPanel which contains this page.
			//!
			//! @param scroll_panel_ref ScrollPanel which contains this page
			//! @returns false if scroll_panel_ref doesn't contain this page
			bool set_scroll_panel(detail::ScrollPanelForAdaptableBase& scroll_panel_ref)
			{
				if (&scroll_panel_ref.content_widget() != dynamic_cast<nana::widget*>(this))
					return false;
				scroll_panel_ptr_ = &scroll_panel_ref;
				return true;
			}

		private:
			detail::ScrollPanelForAdaptableBase* scroll_panel_ptr_{ nullptr };
		};
	}
}