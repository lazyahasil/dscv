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
		//! @throws boost::property_tree::json_parser_error in case of error
		void read_json();

		//! @throws boost::property_tree::json_parser_error in case of error
		void write_json();

		boost::property_tree::ptree& root() noexcept
		{
			return ptree_;
		}

	private:
		//! Sets the version infomation.
		//!
		//! It Overwrites if it exists.
		void _set_version();

		boost::property_tree::ptree ptree_;
	};
}