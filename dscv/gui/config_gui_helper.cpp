#include "config_gui_helper.hpp"

#include "../config_handler.hpp"

#include <nana/gui.hpp>
#include <nana/gui/msgbox.hpp>

using namespace nana;

namespace dscv
{
	namespace gui
	{
		namespace config_gui_helper
		{
			void read_json_noexcept() noexcept
			{
				try
				{
					ConfigHandler::instance().read_json();
				}
				catch (ConfigHandler::JsonParserError& e)
				{
					internationalization i18n;
					msgbox mb{ i18n("JSON Parsing Error") };
					mb.icon(msgbox::icon_error) << i18n("_msgbox_error_json_read") << std::endl;
					mb << charset{ e.what() }.to_bytes(unicode::utf8);
					mb.show();
				}
			}

			void write_json_noexcept() noexcept
			{
				try
				{
					ConfigHandler::instance().write_json();
				}
				catch (ConfigHandler::JsonParserError& e)
				{
					internationalization i18n;
					msgbox mb{ i18n("JSON Parsing Error") };
					mb.icon(msgbox::icon_error) << i18n("_msgbox_error_json_write") << std::endl;
					mb << charset{ e.what() }.to_bytes(unicode::utf8);
					mb.show();
				}
			}
		}
	}
}

