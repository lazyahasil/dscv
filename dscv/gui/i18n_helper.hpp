#pragma once

#include "../misc/make_array.hpp"

#include <utility>

namespace dscv
{
	namespace gui
	{
		namespace i18n_helper
		{
			using I18nData = std::pair<const char*, const char*>;

			//! Loads i18n data from an I18nData container.
			template <typename I18nDataContainer>
			void load_from_data(const I18nDataContainer& arr)
			{
				nana::internationalization i18n;
				for (const auto& idata : arr)
					i18n.set(idata.first, idata.second);
				// Refresh widgets
				i18n.load("");
			}

			//! Removes i18n data sourced from the I18nData container.
			//! Since Nana doesn't provide i18n removal feature, it simply overwrites msgstr with msgid,
			//! and loads the English data.
			template <typename I18nDataContainer>
			void remove_data_sourced(const I18nDataContainer& arr)
			{
				nana::internationalization i18n;
				for (const auto& idata : arr)
					i18n.set(idata.first, idata.first);
				// Refresh widgets
				load_from_data(k_data_eng);
			}

			//! English i18n data
			constexpr auto k_data_eng = make_array(
				I18nData{ "_dscv_language", u8"en-us" },
				I18nData{ "_msgbox_error_1_arg", u8"A following error was occured:\n%arg0" },
				I18nData{ "_msg_error_asio_broken_pipe", u8"The pipeline disconnected.(Broken Pipe)" },
				I18nData{ "_msg_error_process_termination_ordered", u8"Termination was called by an order.(Termination by Order)" },
				I18nData{ "_msg_error_process_termination_timed_out", u8"Termination was called by time out.(Termination by Time Out)" },
				I18nData{ "_msg_judge_start", u8"Staring judgment..." },
				I18nData{ "_msg_judge_launching_processes", u8"Launching judged processes..." },
				I18nData{ "_msg_test_case_asking_removal", u8"Are you sure to remove the test case?\nAny data not saved will be lost!" },
				I18nData{ "_opt_judge_comp_dont_ignore_consecutive_spaces", u8"Don't ignore consecutive spaces" },
				I18nData{ "_opt_judge_judging_add_endl_to_test_case_input_end", u8"Add a newline to test case input's end if not existing" }
			);

			//! Korean i18n data
			constexpr auto k_data_kor = make_array(
				I18nData{ "Add", u8"추가" },
				I18nData{ "All Files (%arg0)", u8"모든 파일 (%arg0)" },
				I18nData{ "Configuration", u8"설정" },
				I18nData{ "Console Input", u8"콘솔 입력" },
				I18nData{ "Console Output", u8"콘솔 출력" },
				I18nData{ "Copy", u8"복사" },
				I18nData{ "Executable File (%arg0)", u8"실행 파일 (%arg0)" },
				I18nData{ "Error", u8"오류" },
				I18nData{ "Find", u8"찾기" },
				I18nData{ "Judged Process Error", u8"평가 프로세스 오류" },
				I18nData{ "Judging", u8"평가하기" },
				I18nData{ "Judging Configuration", u8"평가 설정" },
				I18nData{ "Log", u8"로그" },
				I18nData{ "Modify", u8"수정" },
				I18nData{ "Paste", u8"붙여넣기" },
				I18nData{ "Program Path", u8"프로그램 경로" },
				I18nData{ "Remove", u8"제거" },
				I18nData{ "Removal", u8"제거" },
				I18nData{ "Remove Case", u8"케이스 제거" },
				I18nData{ "Select All", u8"모두 선택" },
				I18nData{ "Starting Judgment Failed", u8"평가 시작 실패" },
				I18nData{ "Terminate", u8"강제 종료" },
				I18nData{ "Test", u8"테스트" },
				I18nData{ "Test Case %arg0", u8"테스트 케이스 %arg0" },
				I18nData{ "&About...", u8"정보...(&A)" },
				I18nData{ "&Help", u8"도움말(&H)" },
				I18nData{ "&Language", u8"언어(&L)" },
				I18nData{ "&Tools", u8"도구(&T)" },
				I18nData{ "_dscv_language", u8"ko-kr" },
				I18nData{ "_msgbox_error_1_arg", u8"다음과 같은 오류가 발생했습니다:\n%arg0" },
				I18nData{ "_msg_error_asio_broken_pipe", u8"파이프 연결이 끊겼습니다.(Broken Pipe)" },
				I18nData{ "_msg_error_process_termination_ordered", u8"강제 종료 명령을 받았습니다.(Termination by Order)" },
				I18nData{ "_msg_error_process_termination_timed_out", u8"시간 초과로 강제 종료되었습니다.(Termination by Time Out)" },
				I18nData{ "_msg_judge_start", u8"평가를 시작합니다..." },
				I18nData{ "_msg_judge_launching_processes", u8"평가 프로세스를 개시합니다..." },
				I18nData{ "_msg_test_case_asking_removal", u8"해당 케이스를 제거하시겠습니까?\n저장되지 않은 데이터는 사라집니다!" },
				I18nData{ "_opt_judge_comp_dont_ignore_consecutive_spaces", u8"연속된 빈칸을 한 개로 취급하지 않기" },
				I18nData{ "_opt_judge_judging_add_endl_to_test_case_input_end", u8"테스트 케이스 입력의 끝에 줄바꿈이 없으면 넣기" }
			);
		}
	}
}