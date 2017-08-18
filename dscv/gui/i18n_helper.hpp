#pragma once

#include <string>

namespace dscv
{
	namespace gui
	{
		namespace i18n_helper
		{
			namespace lang
			{
				constexpr const char* k_en_us = "en-us";
				constexpr const char* k_ko_kr = "ko-kr";
			}

			//! Applies i18n of custom widgets.
			void apply_widgets_i18n();

			//! Returns current language.
			//!
			//! An empty string is returned if any language is not set.
			std::string current_language();

			//! Loads language.
			//!
			//! @param lang_str language identifier string
			//! @returns false if lang_str is invalid
			//! @sa dscv::gui::i18n_helper::lang
			bool load_language(const std::string& lang_str);
		}
	}
}