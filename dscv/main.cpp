#include "gui.hpp"
#include "gui/i18n_helper.hpp"

//! @mainpage DSCV
//! @section Intro
//! Visit the <a href="https://github.com/lazyahasil/dscv">GitHub repository</a>.
//! @section Links
//! - <a href="inherits.html">Class Hierarchy</a>
//! - <a href="namespaces.html">Namespace List</a>

//! Creates a MainWindow and invokes nana::exec().
int main(int argc, const char* argv[])
{
	using namespace dscv::gui;
	i18n_helper::load_from_data(i18n_helper::k_data_eng);
	MainWindow window;
	window.show();
	nana::exec();
	return 0;
}