#pragma once

#include "widgets/code_textbox.hpp"
#include "widgets/scroll_panel.hpp"

#include <nana/gui/widgets/button.hpp>
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

			nana::button btn_config_read_json_{ *this, "Read Configuration JSON" };
			nana::button btn_config_write_json_{ *this, "Write Configuration JSON" };
		};
	}
}