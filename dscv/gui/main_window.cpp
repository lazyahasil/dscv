#include "main_window.hpp"

#include "../version.hpp"
#include "about_window.hpp"
#include "i18n_helper.hpp"
#include "test_window.hpp"

using namespace nana;

namespace dscv
{
	namespace gui
	{
		MainWindow::MainWindow(nana::window wd)
			: form(wd, API::make_center(640, 640),
			  appear::decorate<appear::sizable, appear::minimize, appear::maximize>())
		{
			// Set the form window's title
			caption(std::string{ "DSCV v" } + k_version_str);

			// Set minimum window size
			API::track_window_size(*this, { 480, 100 }, false);

			plc_.div(
				"vert"
				"<weight=26 menubar_panel>"
				"<judge_page>"
			);
			plc_["judge_page"] << judge_page_;

			judge_page_.bgcolor(colors::white);
			judge_page_.content().set_scroll_panel(judge_page_);

			judge_page_.forced_vert_bar(true);
			judge_page_.side_adaptable(true);

			_make_menubar();

			plc_.collocate();
			// Don't know why, collocate() causes judge_page_ to scroll to bottom
			judge_page_.vert_scroll_to(0);
		}

		void MainWindow::_make_menubar()
		{
			if (menubar_panel_)
				menubar_panel_.reset();

			menubar_panel_ = std::make_unique<WrapperPanel<false, nana::menubar>>(*this);
			
			auto& mbar = menubar_panel_->content();
			internationalization i18n;

			auto& tools_m = mbar.push_back(i18n("&Tools"));

			tools_m.append(i18n("&Language"));
			auto* lang_m = tools_m.create_sub_menu(0);
			lang_m->append(u8"English", [&](menu::item_proxy& ip) {
				if (internationalization{}.get("_dscv_language") == "ko-kr")
					i18n_helper::remove_data_sourced(i18n_helper::k_data_kor);
				CodeTextbox::apply_i18n();
				_call_and_collocate(&MainWindow::_make_menubar);
			});
			lang_m->append(u8"한국어", [&](menu::item_proxy& ip) {
				i18n_helper::load_from_data(i18n_helper::k_data_kor);
				CodeTextbox::apply_i18n();
				_call_and_collocate(&MainWindow::_make_menubar);
			});

			auto& help_m = mbar.push_back(i18n("&Help"));
#ifdef _DEBUG
			help_m.append(i18n("Test"), [&](menu::item_proxy& ip) {
				TestWindow w{ *this };
				w.show();
				w.modality();
			});
#endif
			help_m.append(i18n("&About..."), [&](menu::item_proxy& ip) {
				AboutWindow w{ *this };
				w.show();
				w.modality();
			});

			plc_["menubar_panel"] << *menubar_panel_;
		}
	}
}