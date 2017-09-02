#pragma once

#include "misc/singleton.hpp"

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser/error.hpp>

namespace dscv
{
	namespace config_handler
	{
		constexpr const char* k_json_path = ".dscv/dscv_config.json";

		namespace str_path
		{
			constexpr const char* k_version = "dscv.version";
			constexpr const char* k_language = "dscv.language";
			constexpr const char* k_judge_page = "dscv.judge";
		}
	}

	//! Contains Boost.PropertyTree for JSON parser
	class ConfigHandler : public Singleton<ConfigHandler>
	{
	public:
		using Ptree = boost::property_tree::ptree;
		using JsonParserError = boost::property_tree::json_parser::json_parser_error;

		//! Sets the language infomation at the ptree.
		//!
		//! It overwrites if it exists.
		//! @param lang_str language identifier string 
		void put_language(const std::string& lang_str);

		//! Sets the version infomation at the ptree.
		//!
		//! It overwrites if it exists.
		void put_version();

		//! Reads JSON, able to throw.
		//!
		//! Use gui::config_gui_helper::read_json_noexcept() instead for safety.
		//! @throws JsonParserError in case of error
		//! @sa gui::config_gui_helper::read_json_noexcept()
		void read_json();

		//! Returns the subtree from the parent ptree.
		//!
		//! Simply, it returns the subtree which is to be created if not existing.
		//! It tries Ptree::get_child(), and tries Ptree::put_child() if thrown.\n
		//! Use subtree(const std::string&) for the root.
		//! @param parent reference of ptree
		//! @param key path string of subtree
		//! @sa subtree(const std::string&)
		static Ptree& subtree(Ptree& parent, const std::string& key) noexcept;
		
		//! Returns the subtree from the root.
		//!
		//! @param parent reference of Ptree
		//! @param key path string of subtree
		//! @sa subtree(Ptree&, const std::string&)
		Ptree& subtree(const std::string& key) noexcept
		{
			return subtree(ptree_, key);
		}

		//! Returns reference of the ptree.
		Ptree& root() noexcept
		{
			return ptree_;
		}

		//! Writes JSON, able to throw.
		//!
		//! Use gui::config_gui_helper::write_json_noexcept() instead for safety.
		//! @throws JsonParserError in case of error
		//! @sa gui::config_gui_helper::write_json_noexcept()
		void write_json();

	private:
		//! Applies the language setting got from the ptree.
		void _apply_language();

		//! Creates folders containing JSON file if not existing.
		void _create_json_path_if_not_existing();

		Ptree ptree_;
	};
}