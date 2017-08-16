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
	}

	class ConfigHandler : public Singleton<ConfigHandler>
	{
	public:
		using Ptree = boost::property_tree::ptree;
		using JsonParserError = boost::property_tree::json_parser::json_parser_error;

		Ptree& subtree(const std::string& key) noexcept;

		//! @throws JsonParserError in case of error
		void read_json();

		Ptree& root() noexcept
		{
			return ptree_;
		}

		//! @throws JsonParserError in case of error
		void write_json();

	private:
		//! Sets the version infomation.
		//!
		//! It Overwrites if it exists.
		void _set_version();

		Ptree ptree_;
	};
}