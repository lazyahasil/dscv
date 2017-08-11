#pragma once

#include "wrapper_panel.hpp"

#include <nana/gui/widgets/scroll.hpp>

namespace dscv
{
	namespace gui
	{
		//! A simple interface for one-side-adapatable widgets.
		//!
		//! ScrollPanel can make use of the SideAdapatable content.\n
		//! If IsWidthAdaptable is false, virtual SideAdaptable<false>::proper_width() is provided,
		//! and if IsWidthAdaptable is true, virtual SideAdaptable<true>::proper_height() is provided.\n
		//! In short, you need to set size of the inadapatable side.
		//! @sa SideAdaptable<false>
		//! @sa SideAdaptable<true>
		template <bool IsWidthAdaptable>
		class SideAdaptable;

		//! A simple interface for height-adapatable widgets.
		//!
		//! @sa SideAdaptable<bool>
		template <>
		class SideAdaptable<false>
		{
		public:
			virtual std::size_t proper_width() const = 0;
		};

		//! A simple interface for width-adapatable widgets.
		//!
		//! @sa SideAdaptable<bool>
		template <>
		class SideAdaptable<true>
		{
		public:
			virtual std::size_t proper_height() const = 0;
		};

		namespace detail
		{
			namespace scroll_panel
			{
				constexpr std::size_t k_scrollbar_step = 32;
				constexpr std::size_t k_scrollbar_weight = 16;
				const nana::color k_focused_border_color{ nana::color_rgb(0x0595e2) };
			}

			//! The base class of all ScrollPanel<>.
			//!
			//! Since Nana 1.5.3 doesn't provide a scrollable panel widget, this one was had to be made.
			//! Based on an old Nana library example (and added some fixes).\n
			//! https://sourceforge.net/p/nanapro/blog/2016/03/nana-example---using-scrollbar/
			class ScrollPanelBase : public nana::panel<true>
			{
			protected:
				using ContentPanelBaseType = WrapperPanelBase<false>;

			public:
				explicit ScrollPanelBase(nana::window wd) : panel<true>(wd) { }
				ScrollPanelBase() : ScrollPanelBase(nullptr) { }

				//! Returns the content's size.
				//!
				//! @sa content_height()
				//! @sa content_width()
				nana::size content_size() const noexcept
				{
					return content_size_;
				}

				//! Sets the content's size.
				//! @sa content_height(std::size_t)
				//! @sa content_width(std::size_t)
				void content_size(nana::size new_size) noexcept
				{
					content_size_ = new_size;
				}

				//! Returns its content as nana::widget type.
				nana::widget& content_widget() const noexcept
				{
					return content_panel_->content_widget();
				}

				//! Returns the content's height.
				//!
				//! @sa content_size()
				std::size_t content_height() const noexcept
				{
					return content_size_.height;
				}

				//! Sets the content's height.
				//!
				//! @sa content_size(nana::size)
				void content_height(std::size_t new_height) noexcept
				{
					content_size_.height = new_height;
				}

				//! Returns the content's width.
				//!
				//! @sa content_size()
				std::size_t content_width() const noexcept
				{
					return content_size_.width;
				}

				//! Sets the content's width.
				//!
				//! @sa content_size(nana::size)
				void content_width(std::size_t new_width) noexcept
				{
					content_size_.width = new_width;
				}

				//! Check if the horizontal scroll bar is forced to show.
				//!
				//! @sa forced_vert_bar()
				bool forced_horz_bar() const noexcept
				{
					return horz_forced_;
				}

				//! Determins to force the horizontal scroll bar to show.
				//!
				//! @sa forced_vert_bar(bool)
				void forced_horz_bar(bool is_forced) noexcept
				{
					horz_forced_ = is_forced;
				}

				//! Check if the horizontal scroll bar is forced to show.
				//!
				//! @sa forced_horz_bar()
				bool forced_vert_bar() const noexcept
				{
					return vert_forced_;
				}

				//! Determins to force the horizontal scroll bar to show.
				//!
				//! @sa forced_horz_bar(bool)
				void forced_vert_bar(bool is_forced) noexcept
				{
					vert_forced_ = is_forced;
				}

				//! Horizontally scrolls to the position.
				//!
				//! If you want max(end), just put SIZE_MAX from <climits> since it contains a range check.
				//! @param pos the position of the content page
				//! @sa vert_scroll_to()
				void horz_scroll_to(std::size_t pos);

				//! Invokes the horizontal scroll bar's make_scroll().
				//!
				//! @sa make_vert_scroll()
				void make_horz_scroll(bool is_upward)
				{
					scroll_horz_.make_scroll(!is_upward);
				}

				//! Invokes the vertical scroll bar's make_scroll().
				//!
				//! @sa make_horz_scroll()
				void make_vert_scroll(bool is_upward)
				{
					scroll_vert_.make_scroll(!is_upward);
				}

				//! Vertically scrolls to the position.
				//!
				//! If you want max(end), just put SIZE_MAX from <climits> since it contains a range check.
				//! @param pos the position of the content page
				//! @sa horz_scroll_to()
				void vert_scroll_to(std::size_t pos);

			protected:
				//! Returns a maximum value of the horizontal scroll bar.
				//!
				//! @sa _vert_scroll_max_value()
				std::size_t _horz_scroll_max_value() const noexcept
				{
					return content_size_.width - content_panel_->size().width;
				}

				//! Initiates any ScrollPanel<>.
				//!
				//! @param content_panel a valid std::unique_ptr of WrapperPanel<>
				void _init(std::unique_ptr<ContentPanelBaseType>&& content_panel);

				//! Returns a maximum value of the vertical scroll bar.
				//!
				//! @sa _horz_scroll_max_value()
				std::size_t _vert_scroll_max_value() const noexcept
				{
					return content_size_.height - content_panel_->size().height;
				}

			private:
				nana::place plc_{ *this };
				nana::scroll<false> scroll_horz_{ *this, true };
				nana::scroll<true> scroll_vert_{ *this, true };

				bool horz_forced_{ false };
				bool vert_forced_{ false };

				std::unique_ptr<ContentPanelBaseType> content_panel_;
				nana::point content_pos_;
				nana::size content_size_{ 500, 500 };
			};

			//! The base class of ScrollPanel<ContentT, true>.
			//!
			//! It provides nested classes and an interface for the SideAdaptable content.
			class ScrollPanelForAdaptableBase : public ScrollPanelBase
			{
			protected:
				//! It provides a side-adaptable event interface.
				class SideAdaptableEventToggle;

				//! It provides a size applying interface.
				class SideAdaptableSizeApplierBase;

				//! @sa SideAdaptableSizeApplierBase
				template <bool IsWidthAdaptable>
				class SideAdaptableSizeApplier;

			public:
				//! A template type constant to see whether the type is SideAdaptable.
				template <typename T>
				using IsSideAdaptable = std::integral_constant<
					bool,
					std::is_base_of<SideAdaptable<false>, T>::value || std::is_base_of<SideAdaptable<true>, T>::value
				>;

			public:
				explicit ScrollPanelForAdaptableBase(nana::window wd) : ScrollPanelBase(wd) { }
				ScrollPanelForAdaptableBase() : ScrollPanelForAdaptableBase(nullptr) { }

				virtual void apply_adaptable_side() = 0;
				virtual void apply_inadaptable_side() = 0;
				std::size_t content_height_for_adaption();
				std::size_t content_width_for_adaption();
				virtual bool side_adaptable() const noexcept = 0; 
				virtual void side_adaptable(bool is_adaptable) = 0;
			};

			class ScrollPanelForAdaptableBase::SideAdaptableEventToggle
			{
			public:
				explicit SideAdaptableEventToggle(ScrollPanelForAdaptableBase& scroll_panel)
					: scroll_panel_(scroll_panel)
				{ }

				//! Returns whether the side-adaptable event exists
				bool side_adaptable() const noexcept
				{
					return event_handle_ != false;
				}

				//! Toggles the side-adaptable event
				void side_adaptable(bool is_adaptable);

			private:
				ScrollPanelForAdaptableBase& scroll_panel_;
				nana::event_handle event_handle_{ nullptr };
			};

			class ScrollPanelForAdaptableBase::SideAdaptableSizeApplierBase
			{
			public:
				explicit SideAdaptableSizeApplierBase(ScrollPanelForAdaptableBase& scroll_panel)
					: scroll_panel_(scroll_panel)
				{ }

				virtual void apply_adaptable_side() = 0;
				virtual void apply_inadaptable_side() = 0;

			protected:
				ScrollPanelForAdaptableBase& scroll_panel_;
			};

			template <>
			class ScrollPanelForAdaptableBase::SideAdaptableSizeApplier<false>
				: ScrollPanelForAdaptableBase::SideAdaptableSizeApplierBase
			{
			public:
				explicit SideAdaptableSizeApplier(
					ScrollPanelForAdaptableBase& scroll_panel, const SideAdaptable<false>& content
				) : SideAdaptableSizeApplierBase(scroll_panel), content_(content)
				{ }

				void apply_adaptable_side()
				{
					scroll_panel_.content_height(scroll_panel_.content_height_for_adaption());
				}

				void apply_inadaptable_side()
				{
					scroll_panel_.content_width(content_.proper_width());
				}

			private:
				const SideAdaptable<false>& content_;
			};

			template <>
			class ScrollPanelForAdaptableBase::SideAdaptableSizeApplier<true>
				: ScrollPanelForAdaptableBase::SideAdaptableSizeApplierBase
			{
			public:
				explicit SideAdaptableSizeApplier(
					ScrollPanelForAdaptableBase& scroll_panel, const SideAdaptable<true>& content
				) : SideAdaptableSizeApplierBase(scroll_panel), content_(content)
				{ }

				void apply_adaptable_side()
				{
					scroll_panel_.content_width(scroll_panel_.content_width_for_adaption());
				}

				void apply_inadaptable_side()
				{
					scroll_panel_.content_height(content_.proper_height());
				}

			private:
				const SideAdaptable<true>& content_;
			};
		}

		// The scrolling panel for any widget.
		//!
		// @sa detail::ScrollPanelBase
		template <
			typename ContentT,
			bool IsSideAdaptableValue = detail::ScrollPanelForAdaptableBase::IsSideAdaptable<ContentT>::value
		>
		class ScrollPanel;

		//! The scrolling panel for any widget.
		//!
		//! ScrollPanel's template specialization which forbids SideAdaptable.
		//! @sa detail::ScrollPanelBase
		template <typename ContentT>
		class ScrollPanel<ContentT, false> : public detail::ScrollPanelBase
		{
			static_assert(std::is_base_of<nana::widget, ContentT>::value,
				"ScrollPanel requires ContentT which inherits nana::widget.");

		protected:
			using ContentPanelType = WrapperPanel<false, ContentT>;

		public:
			//! @param wd the parent's handle
			//! @param args the arguments forwarded to the content's constructor without the parent's handle
			//! @throws std::runtime_error if std::make_unique<ContentT>() fails
			template <typename ...Args>
			explicit ScrollPanel(nana::window wd, Args&&... args) : ScrollPanelBase(wd)
			{
				// Must call std::make_unique() here in order to create this(ScrollPanel) first
				_init(std::make_unique<ContentPanelType>(*this, std::forward<Args>(args)...));
			}

			ScrollPanel() : ScrollPanel(nullptr) { }

			//! Returns its content as its own type.
			ContentT& content() const noexcept
			{
				return dynamic_cast<ContentT&>(content_widget());
			}
		};

		//! The scrolling panel for any widget.
		//!
		//! ScrollPanel's template specialization which requires SideAdaptable.
		//! @sa detail::ScrollPanelBase
		//! @sa ScrollPanelForAdaptableBase
		template <typename ContentT>
		class ScrollPanel<ContentT, true> : public detail::ScrollPanelForAdaptableBase
		{
			static_assert(std::is_base_of<nana::widget, ContentT>::value,
				"ScrollPanel requires ContentT which inherits nana::widget.");
			static_assert(IsSideAdaptable<ContentT>::value,
				"ScrollPanel requires ContentT which inherits SideAdaptable<bool>.");

		protected:
			using ContentPanelType = WrapperPanel<false, ContentT>;
			using SideAdaptableSizeApplierType
				= SideAdaptableSizeApplier<std::is_base_of<SideAdaptable<true>, ContentT>::value>;

		public:
			//! @param wd the parent's handle
			//! @param args the arguments forwarded to the content's constructor without the parent's handle
			//! @throws std::runtime_error if any std::make_unique<...>() fails
			template <typename ...Args>
			explicit ScrollPanel(nana::window wd, Args&&... args) : ScrollPanelForAdaptableBase(wd)
			{
				// Must call std::make_unique() here in order to create this(ScrollPanel) first
				_init(std::make_unique<ContentPanelType>(*this, std::forward<Args>(args)...));

				event_toggle_ = std::make_unique<SideAdaptableEventToggle>(*this);
				size_applier_ = std::make_unique<SideAdaptableSizeApplierType>(*this, content());
				if (!event_toggle_ || !size_applier_)
					throw std::runtime_error{ "WrapperPanel for SideAdaptable failed to create the instance!" };

				size_applier_->apply_inadaptable_side();
			}

			ScrollPanel() : ScrollPanel(nullptr) { }

			void apply_adaptable_side() override
			{
				size_applier_->apply_adaptable_side();
				nana::API::refresh_window(*this);
			}

			void apply_inadaptable_side() override
			{
				size_applier_->apply_inadaptable_side();
				nana::API::refresh_window(*this);
			}

			//! Returns its content as its own type.
			ContentT& content() const noexcept
			{
				return dynamic_cast<ContentT&>(content_widget());
			}

			bool side_adaptable() const noexcept override
			{
				return event_toggle_->side_adaptable();
			}

			void side_adaptable(bool is_adaptable) override
			{
				event_toggle_->side_adaptable(is_adaptable);
			}

		private:
			std::unique_ptr<SideAdaptableEventToggle> event_toggle_;
			std::unique_ptr<SideAdaptableSizeApplierType> size_applier_;
		};
	}
}