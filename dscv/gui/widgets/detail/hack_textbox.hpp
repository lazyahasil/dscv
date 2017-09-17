// Nana 1.5.4
#pragma once

#define set_wheel_speed dummy_func() { } \
			struct implementation; \
			widgets::skeletons::content_view::implementation* impl() const noexcept; \
			void set_wheel_speed
#include <nana/../../source/gui/widgets/skeletons/content_view.hpp>
#undef set_wheel_speed

#define impl_ impl_; \
		public: \
			struct implementation; \
			implementation* impl() const noexcept; \
			widgets::skeletons::content_view* cview() const noexcept; \
		private:
#include <nana/gui/widgets/skeletons/text_editor.hpp>
#undef impl_

#define text_line_count text_line_count() const noexcept; \
			const drawerbase::textbox::drawer::text_editor* editor
#include <nana/gui/widgets/textbox.hpp>
#undef text_line_count

namespace dscv
{
	namespace gui
	{
		namespace detail
		{
			namespace hack_textbox
			{
				constexpr std::size_t k_scrollbar_weight = 16;

				bool check_if_vert_fully_shown(const nana::textbox& tb);
				bool check_if_max_vert_scrolled(const nana::textbox& tb);
				bool check_if_min_vert_scrolled(const nana::textbox& tb);
				void make_scroll(nana::textbox& tb, bool is_forward, bool is_horz);
				bool text_empty(const nana::textbox& tb);
			}
		}
	}
}