﻿#include "page_base.hpp"

#include "main_window.hpp"

#include <climits>

using namespace nana;

namespace dscv
{
	namespace gui
	{
		PageBase::~PageBase()
		{
			_main_window_config_prevent_invalid_ref();
		}

		bool PageBase::scroll_panel(PageBase::ScrollPanelPage& scroll_panel_ref)
		{
			if (&scroll_panel_ref.content_widget() != dynamic_cast<nana::widget*>(this))
				return false;
			scroll_panel_ptr_ = &scroll_panel_ref;
			return true;
		}

		void PageBase::scroll_panel_make_vert_scroll(bool is_upward)
		{
			if (!scroll_panel_ptr_)
				return;

			scroll_panel_ptr_->make_vert_scroll(is_upward);
		}

		void PageBase::scroll_panel_vert_scroll_to_end()
		{
			if (!scroll_panel_ptr_)
				return;

			scroll_panel_ptr_->vert_scroll_to(SIZE_MAX);
		}

		void PageBase::scroll_panel_refresh()
		{
			if (!scroll_panel_ptr_)
				return;

			scroll_panel_ptr_->apply_inadaptable_side();
		}

		void PageBase::_main_window_config_prevent_invalid_ref()
		{
			if (!main_window_ptr_)
				return;
			main_window_ptr_->config_window().prevent_invalid_ref(*this);
		}
	}
}
