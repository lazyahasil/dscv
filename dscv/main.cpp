#include "gui.hpp"
#include "gui/i18n_helper.hpp"

int main(int argc, const char* argv[])
{
	using namespace dscv::gui;
	i18n_helper::load_from_data(i18n_helper::k_data_eng);
	MainWindow window;
	window.show();
	nana::exec();
	return 0;
}