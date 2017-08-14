#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>

namespace dscv
{
	namespace gui
	{
		namespace judge_page
		{
			//! The settings window for JudgePage
			class JudgeSettingsForm : public nana::nested_form
			{
			public:
				explicit JudgeSettingsForm(nana::window wd);

				JudgeSettingsForm() : JudgeSettingsForm(nullptr) { }

			private:
				nana::place plc_{ *this };
			};
		}
	}
}
