#include "about_window.hpp"

#include "../version.hpp"
#include "i18n_helper.hpp"

using namespace nana;

namespace dscv
{
	namespace gui
	{
		AboutWindow::AboutWindow(nana::window wd)
			: form(wd, rectangle{ 10, 10, 300U, 200U },
				appear::decorate<appear::sizable/*, appear::taskbar*/>())
		{
			// Set the form window's title
			caption("About...");

			// Set minimum window size
			API::track_window_size(*this, { 200, 100 }, false);

			plc_.div("<margin=10 sp_licenses>");

			plc_["sp_licenses"] << sp_licenses_;
			plc_.collocate();
		}
	}
}