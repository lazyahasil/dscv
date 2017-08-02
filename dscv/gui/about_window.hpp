#pragma once

#include "widgets/scroll_panel.hpp"
#include "about_window/license_box.hpp"

#include <nana/gui/widgets/picture.hpp>

namespace dscv
{
	namespace gui
	{
		class AboutWindow : public nana::form
		{
		public:
			explicit AboutWindow(nana::window wd);
			AboutWindow() : AboutWindow(nullptr) { }

		private:
			nana::place plc_{ *this };

			ScrollPanel<about_window::LicenseBox> sp_licenses_{ *this };
		};
	}
}