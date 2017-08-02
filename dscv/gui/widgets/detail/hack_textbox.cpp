// Nana 1.5.4
#include "hack_textbox.hpp"

#include <nana/../../source/gui/widgets/skeletons/text_editor.cpp>
#include <nana/../../source/gui/widgets/skeletons/content_view.cpp>

const nana::drawerbase::textbox::drawer::text_editor*
nana::textbox::editor() const noexcept
{
	return get_drawer_trigger().editor();
}

nana::drawerbase::textbox::drawer::text_editor::implementation*
nana::drawerbase::textbox::drawer::text_editor::impl() const noexcept
{
	return impl_;
}

nana::widgets::skeletons::content_view*
nana::drawerbase::textbox::drawer::text_editor::cview() const noexcept
{
	return impl_->cview.get();
}

nana::widgets::skeletons::content_view::implementation*
nana::widgets::skeletons::content_view::impl() const noexcept
{
	return impl_;
}

namespace dscv
{
	namespace gui
	{
		namespace detail
		{
			namespace hack_textbox
			{
				bool check_if_max_vert_scrolled(const nana::textbox& tb)
				{
					const auto cvimpl = tb.editor()->cview()->impl();
					auto val = cvimpl->vert.value();
					if (val < k_scrollbar_weight)
						val = 0;
					auto max = cvimpl->content_size.height;
					const auto disp_height = cvimpl->disp_area.height;
					if (max <= disp_height)
						return true;
					return val == max - disp_height;
				}

				bool check_if_min_vert_scrolled(const nana::textbox& tb)
				{
					const auto cvimpl = tb.editor()->cview()->impl();
					const auto val = cvimpl->vert.value();
					return val < k_scrollbar_weight;
				}

				void make_scroll(nana::textbox& tb, bool is_forward, bool is_horz)
				{
					auto cview = tb.editor()->cview();
					cview->scroll(is_forward, is_horz);
				}

				bool text_empty(const nana::textbox& tb)
				{
					const auto eimpl = tb.editor()->impl();
					return eimpl->textbase.empty();
				}
			}
		}
	}
}