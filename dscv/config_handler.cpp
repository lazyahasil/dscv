#include "config_handler.hpp"

#include "constants.hpp"

#include <boost/property_tree/json_parser.hpp>

#include <iostream>

namespace b_ptree = boost::property_tree;

namespace dscv
{
	using namespace config_handler;

	void ConfigHandler::read_json()
	{
		try
		{
			b_ptree::read_json(k_json_path, ptree_);
		}
		catch (b_ptree::json_parser_error& e)
		{
			_set_version();
			throw e;
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