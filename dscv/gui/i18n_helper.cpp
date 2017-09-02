#include "i18n_helper.hpp"

#include "../config_handler.hpp"
#include "i18n_helper/i18n_data.hpp"
#include "widgets/code_textbox.hpp"

using namespace nana;

namespace dscv
{
	namespace gui
	{
		namespace i18n_helper
		{
			void apply_widgets_i18n()
			{
				CodeTextbox::apply_i18n();
			}

			std::string current_language()
			{
				auto cur_lang = internationalization{}.get(detail::k_lang_msgid);

				// Return an empty string if not set
				if (cur_lang == detail::k_lang_msgid)
					return std::string{};

				return cur_lang;
			}

			bool load_language(const std::string& lang_str, bool to_update_ptree)
			{
				auto cur_lang = current_language();

				// Resolve the current language
				if (!cur_lang.empty())
				{
					// Just return doing nothing if the current is same
					if (cur_lang == lang_str)
						return true;

					// Remove i18n data without English
					if (cur_lang == lang::k_ko_kr)
						detail::remove_data_sourced(detail::lang_data::k_data_ko_kr);

					// Return if the new is English (already overwritten with English)
					if (lang_str == lang::k_en_us)
					{
						if (to_update_ptree)
							ConfigHandler::instance().put_language(lang::k_en_us);

						apply_widgets_i18n();

						return true;
					}
				}

				// Set the new language
				if (lang_str == lang::k_en_us)
					detail::load_from_data(detail::lang_data::k_data_en_us);
				else if (lang_str == lang::k_ko_kr)
					detail::load_from_data(detail::lang_data::k_data_ko_kr);
				else
					return false; // Failure

				if (to_update_ptree)
					ConfigHandler::instance().put_language(lang_str);

				apply_widgets_i18n();

				return true;
			}
		}
	}
}