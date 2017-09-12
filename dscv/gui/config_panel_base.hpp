#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/panel.hpp>

namespace dscv
{
	namespace gui
	{
		class ConfigPanelBase : public nana::panel<false>
		{
		public:
			ConfigPanelBase() = delete;

			ConfigPanelBase(nana::window wd) : nana::panel<false>(wd) { }

			virtual ~ConfigPanelBase() = default;

			virtual void apply_i18n() = 0;
		};
	}
}