#include "config_handler.hpp"

#include "version.hpp"
#include "gui/i18n_helper.hpp"

#include <boost/property_tree/json_parser.hpp>

#include <iostream>

namespace b_ptree = boost::property_tree;

namespace dscv
{
	using namespace config_handler;

	void ConfigHandler::put_version()
	{
		ptree_.put(str_path::k_version, k_version_str);
	}

	void ConfigHandler::read_json()
	{
		try
		{
			b_ptree::read_json(k_json_path, ptree_);
		}
		catch (b_ptree::json_parser_error& e)
		{
			put_version(); // Correct the version
			throw e; // Error propagation
		}
		put_version(); // Correct the version
		_apply_language();
	}

	ConfigHandler::Ptree& ConfigHandler::subtree(ConfigHandler::Ptree& some_ptree, const std::string& key) noexcept
	{
		try
		{
			return some_ptree.get_child(key);
		}
		catch (b_ptree::ptree_bad_path&)
		{
			// Make one if not existing
			return some_ptree.put_child(key, b_ptree::ptree{});
		}
	}

	void ConfigHandler::write_json()
	{
		b_ptree::write_json(k_json_path, ptree_);
	}

	void ConfigHandler::_apply_language()
	{
		auto lang_str = ptree_.get(str_path::k_language, gui::i18n_helper::lang::k_en_us);
		gui::i18n_helper::load_language(lang_str);
	}
}