#include "config_window.hpp"

using namespace nana;

namespace dscv
{
	namespace gui
	{
		ConfigWindow::ConfigWindow(nana::window wd)
			: form(wd, API::make_center(wd, 640, 480), appear::decorate<appear::sizable>())
		{
			// Set the form window's title
			caption("Options...");

			// Set minimum window size
			API::track_window_size(*this, { 200, 100 }, false);

			plc_.div("<content>");

			plc_.collocate();
		}

		void ConfigWindow::prevent_invalid_ref(PageBase& page)
		{
			if (page_ptr_ == &page)
			{
				content_ptr_.reset();
				page_ptr_ = nullptr;
			}
		}
	}
}