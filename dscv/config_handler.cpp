#include "config_handler.hpp"

#include "version.hpp"

#include <boost/property_tree/json_parser.hpp>

#include <iostream>

namespace b_ptree = boost::property_tree;

namespace dscv
{
	using namespace config_handler;

	ConfigHandler::Ptree& ConfigHandler::subtree(const std::string& key) noexcept
	{
		try
		{
			return ptree_.get_child(key);
		}
		catch (b_ptree::ptree_bad_path&)
		{
			// Make one if not existing
			return ptree_.put_child(key, b_ptree::ptree{});
		}
	}

	void ConfigHandler::read_json()
	{
		try
		{
			b_ptree::read_json(k_json_path, ptree_);
		}
		catch (b_ptree::json_parser_error& e)
		{
			_set_version();
			throw e; // Error propagation
		}
		_set_version();
	}

	void ConfigHandler::write_json()
	{
		b_ptree::write_json(k_json_path, ptree_);
	}

	void ConfigHandler::_set_version()
	{
		ptree_.put("dscv.version", k_version_str);
	}
}