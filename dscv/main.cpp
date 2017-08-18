#include "config_handler.hpp"
#include "gui.hpp"
#include "gui/config_gui_helper.hpp"
#include "gui/i18n_helper.hpp"

#include <boost/filesystem.hpp>

//! @mainpage DSCV
//! @section Intro
//! Visit the <a href="https://github.com/lazyahasil/dscv">GitHub repository</a>.
//! @section Links
//! - <a href="inherits.html">Class Hierarchy</a>
//! - <a href="namespaces.html">Namespace List</a>

//! Creates a MainWindow and invokes nana::exec().
int main(int argc, const char* argv[])
{
	using namespace dscv;
	namespace fs = boost::filesystem;

	// Set default language
	gui::i18n_helper::load_language(gui::i18n_helper::lang::k_en_us);

	// Read the config JSON file
	if (fs::exists(fs::path{ config_handler::k_json_path }))
		gui::config_gui_helper::read_json_noexcept();
	else
		ConfigHandler::instance().put_version();

	// Open MainWindow
	gui::MainWindow window;
	window.show();
	nana::exec();
	
	return 0;
}