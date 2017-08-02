#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/panel.hpp>

namespace dscv
{
	namespace gui
	{
		namespace about_window
		{
			class LicenseBox : public nana::panel<true>
			{
			public:
				explicit LicenseBox(nana::window wd);
				LicenseBox() : LicenseBox(nullptr) { }

			private:
				nana::place plc_{ *this };


			};
		}
	}
}