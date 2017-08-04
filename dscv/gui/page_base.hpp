#pragma once

#include "widgets/scroll_panel.hpp"

namespace dscv
{
	namespace gui
	{
		//! The Base class of page panels into ScrollPanel.
		class PageBase
			: public nana::panel<false>, public SideAdaptable<true>
		{
		public:
			explicit PageBase(nana::window wd) : nana::panel<false>(wd) { }

			PageBase() : PageBase(nullptr) { }

			void scroll_panel_make_vert_scroll(bool is_upward);
			void scroll_panel_vert_scroll_to_end();
			void scroll_panel_refresh();

			//! Sets its scroll panel pointer from reference.
			void set_scroll_panel(detail::ScrollPanelForAdaptableBase& scroll_panel_ref) noexcept
			{
				scroll_panel_ptr_ = &scroll_panel_ref;
			}

		private:
			detail::ScrollPanelForAdaptableBase* scroll_panel_ptr_{ nullptr };
		};
	}
}