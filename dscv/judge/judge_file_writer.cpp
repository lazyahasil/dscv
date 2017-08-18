#include "judge_file_writer.hpp"

#include <fstream>
#include <sstream>

namespace dscv
{
	namespace judge
	{
		namespace file_writer
		{
			bool write_text_file(
				const std::wstring& file_path,
				const std::string& str,
				const unsigned char* bom_data,
				std::size_t bom_bytes
			)
			{
				std::ofstream file{ file_path };

				if (!file)
					return false;

				// Write BOM
				if (bom_data)
					file.write(reinterpret_cast<const char*>(bom_data), bom_bytes);

				std::stringstream ss{ str };
				std::string buf;

				if (std::getline(ss, buf))
					file << buf;
				while (std::getline(ss, buf))
					file << std::endl << buf;

				// Check if there's a newline at str's back
				if (str.back() == '\n' || str.back() == '\r')
					file << std::endl;

				return true;
			}
		}
	}
}