#pragma once

#include "../widgets/code_textbox.hpp"

#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>

namespace dscv
{
	namespace gui
	{
		namespace judge_page
		{
			class TestStreamBoxBase : public nana::panel<false>
			{
			public:
				explicit TestStreamBoxBase(nana::window wd) : nana::panel<false>(wd) { }

				TestStreamBoxBase() : TestStreamBoxBase(nullptr) { }

				virtual bool case_empty() = 0;
				virtual void set_min_or_max_vert_scrolled_func(std::function<void(bool)> functor) noexcept = 0;
				virtual void set_stream_num(std::size_t num) = 0;

			protected:
				nana::place plc_{ *this };
			};

			class TestInStreamBox : public TestStreamBoxBase
			{
			public:
				TestInStreamBox(nana::window wd, const std::string& label_str = "", std::size_t num = 0U);

				TestInStreamBox() : TestInStreamBox(nullptr) { }

				bool case_empty() override;

				void i18n_name_label(nana::i18n_eval&& eval)
				{
					label_name_.i18n(std::move(eval));
				}

				void set_min_or_max_vert_scrolled_func(std::function<void(bool)> functor) noexcept override;
				void set_stream_num(std::size_t num) override;

				std::string text_case() const noexcept
				{
					return ctb_case_.tb().caption();
				}

			private:
				nana::label label_num_{ *this };
				nana::label label_name_{ *this };
				nana::button btn_save_{ *this };
				CodeTextbox ctb_case_{ *this };
			};

			class TestOutStreamBox : public TestInStreamBox
			{
			public:
				TestOutStreamBox(nana::window wd, const std::string& label_str = "", std::size_t num = 0U);

				TestOutStreamBox() : TestOutStreamBox(nullptr) { }

				virtual void clear_result();
				void set_min_or_max_vert_scrolled_func(std::function<void(bool)> functor) noexcept override;

				void text_result_append(const std::string& str)
				{
					ctb_result_.tb().append(str, false);
				}

			private:
				CodeTextbox ctb_result_{ *this };
			};

			class TestInOutStreamBox : public TestOutStreamBox
			{
			public:
				TestInOutStreamBox(nana::window wd, const std::string& label_str = "", std::size_t num = 0U);

				TestInOutStreamBox() : TestInOutStreamBox(nullptr) { }

				bool case_empty() override;
				void clear_result() override;
				void set_min_or_max_vert_scrolled_func(std::function<void(bool)> functor) noexcept override;

				std::string text_case_out() const noexcept
				{
					return ctb_case_out_.tb().caption();
				}

			private:
				CodeTextbox ctb_case_out_{ *this };
			};
		}
	}
}
