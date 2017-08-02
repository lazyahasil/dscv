#pragma once

#include <string>
#include <vector>

namespace dscv
{
	namespace judge
	{
		struct JudgeStreamInfo
		{
			struct FileInfo
			{
				bool is_text{ true };
				std::string filename;
			};

			bool has_stdin{ true };
			bool has_stdout{ true };
			std::vector<FileInfo> in_files;
			std::vector<FileInfo> out_files;
			std::vector<FileInfo> inout_files;
		};
	}
}