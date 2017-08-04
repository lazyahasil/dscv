#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/panel.hpp>

namespace dscv
{
	namespace gui
	{
		namespace detail
		{
			//! The base class of WrapperPanel.
			//!
			//! This class was intended to make a pointer to WrapperPanel.
			template <bool HasBackground>
			class WrapperPanelBase : public nana::panel<HasBackground>
			{
			public:
				explicit WrapperPanelBase(nana::window wd) : nana::panel<HasBackground>(wd)
				{
					plc_.div("<content>");
				}

				WrapperPanelBase() : WrapperPanelBase(nullptr) { }

				//! Returns its content as nana::widget type
				nana::widget& content_widget() const noexcept
				{
					return *content_ptr_.get();
				}

				void fix_content_height(std::size_t fixed_height)
				{
					plc_.div((
						std::string{} + ""
						"vert"
						"<weight=" + std::to_string(fixed_height) + " content>"
						).c_str()
					);
					make_content_div();
				}

				void fix_content_width(std::size_t fixed_width)
				{
					plc_.div((
						std::string{} + ""
						"<weight=" + std::to_string(fixed_width) + " content>"
						).c_str()
					);
					make_content_div();
				}

				//! Puts its content to nana::place.
				//!
				//! This is done by calling nana::place::field_interface::operator<<().
				//! @sa umake_content_div()
				void make_content_div()
				{
					if (div_is_made_)
						return;
					plc_["content"] << *content_ptr_;
					plc_.collocate();
					div_is_made_ = true;
				}

				//! Erases its content from nana::place.
				//!
				//! This is done by calling nana::place::erase().
				//! @sa make_content_div()
				void umake_content_div()
				{
					if (!div_is_made_)
						return;
					plc_.erase(*content_ptr_);
					plc_.collocate();
					div_is_made_ = false;
				}

			protected:
				nana::place plc_{ *this };
				bool div_is_made_{ false };
				std::unique_ptr<nana::widget> content_ptr_;
			};
		}

		//! A wrapping panel for any widgets.
		//!
		//! This class prevents its content widget to get out of desired area.
		//! The content type must be taken as nana::widget or derived class of that.
		template <bool HasBackground, typename ContentT>
		class WrapperPanel : public detail::WrapperPanelBase<HasBackground>
		{
			static_assert(std::is_base_of<nana::widget, ContentT>::value,
				"WrapperPanel requires ContentT which inherits nana::widget.");

		public:
			//! The constructor.
			//!
			//! Basically, make_content_div() is called.
			//! @param wd the parent's handle
			//! @param args the arguments forwarded to the content's constructor without the parent's handle
			//! @throws std::runtime_error if std::make_unique<ContentT>() fails
			template <typename ...Args>
			explicit WrapperPanel(nana::window wd, Args&&... args) : detail::WrapperPanelBase<HasBackground>(wd)
			{
				// Must make_unique() here in order to initiate WrapperPanelBase first
				content_ptr_ = std::make_unique<ContentT>(*this, std::forward<Args>(args)...);
				if (!content_ptr_)
					throw std::runtime_error{ "WrapperPanel failed to create the content's instance!" };
				make_content_div();
			}

			WrapperPanel() : WrapperPanel(nullptr) { }

			//! Returns its content as its own type.
			ContentT& content() const noexcept
			{
				return *dynamic_cast<ContentT*>(content_ptr_.get());
			}
		};
	}
}