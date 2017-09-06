#pragma once

namespace dscv
{
	namespace gui
	{
		namespace config_gui_helper
		{
			//! Reads the configuration JSON, being able to show a msgbox.
			//!
			//! It shows an error msgbox if ConfigHandler::JsonParserError is thrown.
			void read_json_noexcept() noexcept;

			//! Writes the configuration JSON, being able to show a msgbox.
			//!
			//! It shows an error msgbox if ConfigHandler::JsonParserError is thrown.
			void write_json_noexcept() noexcept;
		}
	}
}