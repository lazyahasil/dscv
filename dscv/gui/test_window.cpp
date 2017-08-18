#include "test_window.hpp"

#include "config_gui_helper.hpp"

#include <nana/gui/msgbox.hpp>

using namespace nana;

namespace dscv
{
	namespace gui
	{
		TestWindow::TestWindow(nana::window wd)
			: form(wd, rectangle{ 10, 10, 640, 480 },
				appear::decorate<appear::sizable>())
		{
			// Set the form window's title
			caption("Test Window");

			// Set minimum window size
			//API::track_window_size(*this, { 200, 100 }, false);

			plc_.div(
				"vert margin=10"
				"<"
				"  <code_textbox>"
				"  <weight=10>"
				"  <scroll_pic>"
				">"
				"<weight=20>"
				"<weight=30 gap=5 buttons>"
			);

			plc_["code_textbox"] << code_textbox_;
			plc_["scroll_pic"] << scroll_pic_;
			plc_["buttons"] << btn_config_read_json_ << btn_config_write_json_;
			plc_.collocate();

			paint::image img("test_image.bmp");
			scroll_pic_.content().load(img);
			scroll_pic_.content_size(img.size());

			btn_config_read_json_.events().click([] {
				config_gui_helper::read_json_noexcept();
			});

			btn_config_write_json_.events().click([] {
				config_gui_helper::write_json_noexcept();
			});
		}
	}
}