#pragma once

#include "widgets/code_textbox.hpp"
#include "widgets/scroll_panel.hpp"

#include <nana/gui/widgets/picture.hpp>

namespace dscv
{
	namespace gui
	{
		class TestWindow : public nana::form
		{
		public:
			explicit TestWindow(nana::window wd);

			TestWindow() : TestWindow(nullptr) { }

		private:
			nana::place plc_{ *this };

			CodeTextbox code_textbox_{ *this };
			ScrollPanel<nana::picture> scroll_pic_{ *this };
		};
	}
}