#pragma once

#include <string>
#include <vector>

namespace dscv
{
	namespace judge
	{
		//! String constants of JSON path str
		namespace judge_stream_info
		{
			constexpr const char* k_path_str = "streams";
			constexpr const char* k_array_inout_files = "inout_files";
			constexpr const char* k_array_in_files = "in_files";
			constexpr const char* k_array_out_files = "out_files";
			constexpr const char* k_has_stdin = "has_stdin";
			constexpr const char* k_has_stdout = "has_stdout";

			namespace file_info
			{
				constexpr const char* k_name = "filename";
				constexpr const char* k_media = "media";
			}

			namespace file_media
			{
				constexpr const char* k_text = "text";
			}
		}
	}
}