#pragma once

#include "judge_page.hpp"
#include "main_window/main_window_base.hpp"

#include <nana/gui/widgets/menubar.hpp>

namespace dscv
{
	namespace gui
	{
		//! The main window.
		class MainWindow : public detail::MainWindowBase
		{
		public:
			using ScrollPanelPage = PageBase::ScrollPanelPage;

			explicit MainWindow(nana::window wd);

			MainWindow() : MainWindow(nullptr) { }

			//! Returns the ConfigWindow
			ConfigWindow& config_window() override
			{
				return config_wd_;
			}

		private:
			void _add_judge_page(const std::string& name);

			//! Makes menubar.
			void _make_menubar();

			//! Sets language.
			void _set_language_and_refresh_menubar(const std::string& lang_str);

			nana::place plc_{ *this };

			std::vector<std::unique_ptr<ScrollPanelPage>> scroll_pages_;
			
			//! The menubar panel upside.
			//!
			//! It's contained in WrapperPanel because of nana::menubar widget's thickness.\n
			//! And since it's harsh to rename the menubar's texts for i18n, MainWindow simply re-creates it.
			//! That's why this is in std::unique_ptr<>.
			std::unique_ptr<WrapperPanel<false, nana::menubar>> menubar_panel_;

			ConfigWindow config_wd_{ *this };
		};
	}
}