#pragma once

#include "widgets/wrapper_panel.hpp"

#include <nana/gui.hpp>

#include <mutex>

namespace dscv
{
	namespace gui
	{
		class PageBase;

		//! A global window for any configuration.
		//!
		//! It's contained by MainWindow. And it stores a single widget like WrapperPanel.
		class ConfigWindow : public nana::form
		{
		public:
			explicit ConfigWindow(nana::window wd);

			ConfigWindow() : ConfigWindow(nullptr) { }

			//! Creates the content from arguments.
			//!
			//! @param args the arguments forwarded to the content's constructor without the parent's handle
			//! @throws std::runtime_error if std::make_unique<ContentT>() fails
			template <typename ContentT, typename ...Args>
			void content(PageBase& page, Args&&... args);

			//! Returns its content as nana::widget type.
			//!
			//! @throws std::runtime_error if the content is nullptr
			nana::widget& content_widget()
			{
				if (!content_ptr_)
					throw std::runtime_error{ "ConfigWindow's content is nullptr!" };
				return *content_ptr_;
			}

			bool page_empty()
			{
				return !page_ptr_;
			}

			void prevent_invalid_ref(PageBase& page);

		private:
			nana::place plc_{ *this };

			std::unique_ptr<nana::widget> content_ptr_;
			PageBase* page_ptr_{ nullptr };
			std::recursive_mutex mutex_;
		};

		template <typename ContentT, typename ...Args>
		void ConfigWindow::content(PageBase& page, Args&&... args)
		{
			static_assert(std::is_base_of<nana::widget, ContentT>::value,
				"ConfigWindow::content() requires ContentT which inherits nana::widget.");

			std::lock_guard<std::recursive_mutex> lock{ mutex_ };

			content_ptr_ = std::make_unique<ContentT>(*this, std::forward<Args>(args)...);
			page_ptr_ = page;

			if (!content_ptr_)
				throw std::runtime_error{ "ConfigWindow failed to create the content's instance!" };

			// Set place
			plc_["content"] << content_ptr_;
			plc_.collocate();
		}
	}
}