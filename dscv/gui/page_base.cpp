#include "page_base.hpp"

#include <climits>

using namespace nana;

namespace dscv
{
	namespace gui
	{
		void PageBase::scroll_panel_make_vert_scroll(bool is_upward)
		{
			if (!scroll_panel_ptr_)
				return;

			scroll_panel_ptr_->make_vert_scroll(is_upward);
		}

		void PageBase::scroll_panel_vert_scroll_to_end()
		{
			scroll_panel_ptr_->vert_scroll_to(SIZE_MAX);
		}

		void PageBase::scroll_panel_refresh()
		{
			if (!scroll_panel_ptr_)
				return;

			scroll_panel_ptr_->apply_inadaptable_side();
		}
	}
}
