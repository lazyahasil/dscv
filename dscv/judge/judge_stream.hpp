#pragma once

#include <string>
#include <vector>
#include <limits>

namespace dscv
{
	namespace judge
	{
		//! String constants of JSON path str
		namespace judge_stream
		{
			enum class StreamType : std::size_t
			{
				con_stdin,
				con_stdout,
				in_file,
				out_file,
				inout_file,
				error = std::numeric_limits<std::size_t>::max()
			};

			StreamType file_type_from_str(const std::string& str);

			enum class FileMediaType : std::size_t
			{
				text,
				binary,
				error = std::numeric_limits<std::size_t>::max()
			};

			FileMediaType file_media_type_from_str(const std::string& str);

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
				constexpr const char* k_binary = "binary";
			}
		}
	}
}