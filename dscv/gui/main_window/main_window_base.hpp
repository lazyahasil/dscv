#pragma once

namespace dscv
{
	namespace gui
	{
		namespace detail
		{
			class MainWindowBase : public nana::form
			{
			public:
				explicit MainWindowBase(nana::window wd)
					: nana::form(wd, nana::API::make_center(640, 640),
						appear::decorate<appear::sizable, appear::minimize, appear::maximize>())
				{ }

				MainWindowBase() : MainWindowBase(nullptr) { }

				virtual ConfigWindow& config_window() = 0;
			};
		}
	}
}