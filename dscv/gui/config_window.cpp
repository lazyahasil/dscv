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
			API::track_window_size(*this, { 540, 360 }, false);

			plc_.div("<content>");

			plc_.collocate();

			events().unload([this](const arg_unload& arg) {
				// Hide this instead of closing
				hide();
				arg.cancel = true;
				// Clear
				std::lock_guard<std::recursive_mutex> lock{ mutex_ };
				page_ptr_ = nullptr;
				content_ptr_.reset();
			});
		}

		void ConfigWindow::apply_i18n()
		{
			if (!content_ptr_)
				return;
			content_ptr_->apply_i18n();
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