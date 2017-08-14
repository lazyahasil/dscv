#pragma once

#include "judge_page.hpp"
#include "widgets/wrapper_panel.hpp"

#include <nana/gui/widgets/menubar.hpp>

namespace dscv
{
	namespace gui
	{
		//! The main window.
		class MainWindow : public nana::form
		{
		public:
			explicit MainWindow(nana::window wd);

			MainWindow() : MainWindow(nullptr) { }

		private:
			//! Calls MainWindow's method and nana::place::collocate().
			//!
			//! This is needed when the object's lambda deletes itself.
			//! It prevents dangling reference in lambda. 
			//! @sa _make_menubar()
			template <typename FuncReturnT, typename ...FuncArgs, typename ...Args>
			void _call_and_collocate(FuncReturnT (MainWindow::*functor)(FuncArgs...), Args&&... args)
			{
				(this->*functor)(std::forward<Args>(args)...);
				plc_.collocate();
			}

			//! Makes menubar.
			void _make_menubar();

			nana::place plc_{ *this };
			
			//! The menubar panel upside.
			//!
			//! It's contained in WrapperPanel because of nana::menubar widget's thickness.\n
			//! And since it's harsh to rename texts for i18n, it simply re-create the menubar.
			//! That's why this is in std::unique_ptr<>.
			std::unique_ptr<WrapperPanel<false, nana::menubar>> menubar_panel_;

			ScrollPanel<JudgePage> judge_page_{ *this };
		};
	}
}