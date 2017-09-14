#include "judge_stream.hpp"

namespace dscv
{
	namespace judge
	{
		namespace judge_stream
		{
			StreamType file_type_from_str(const std::string& str)
			{
				if (str == k_array_in_files)
					return StreamType::in_file;
				if (str == k_array_out_files)
					return StreamType::out_file;
				if (str == k_array_in_files)
					return StreamType::inout_file;
				return StreamType::error;
			}

			FileMediaType file_media_type_from_str(const std::string& str)
			{
				if (str == file_media::k_text)
					return FileMediaType::text;
				if (str == file_media::k_binary)
					return FileMediaType::binary;
				return FileMediaType::error;
			}
		}
	}
}