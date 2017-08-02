#include "test_window.hpp"

#include "../constants.hpp"
#include "i18n_helper.hpp"

using namespace nana;

namespace dscv
{
	namespace gui
	{
		TestWindow::TestWindow(nana::window wd)
			: form(wd, rectangle{ 10, 10, 640U, 200U },
				appear::decorate<appear::sizable>())
		{
			// Set the form window's title
			caption("Test Window");

			// Set minimum window size
			//API::track_window_size(*this, { 200, 100 }, false);

			plc_.div(
				"<margin=10 code_textbox>"
				"<margin=10 scroll_pic>"
			);

			plc_["code_textbox"] << code_textbox_;
			plc_["scroll_pic"] << scroll_pic_;
			plc_.collocate();

			paint::image img("test_image.bmp");
			scroll_pic_.content().load(img);
			scroll_pic_.content_size(img.size());
		}
	}
}