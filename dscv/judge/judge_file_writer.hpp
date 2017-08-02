#pragma once

#include <array>
#include <string>

namespace dscv
{
	namespace judge
	{
		namespace file_writer
		{
			namespace bom // Byte Order Mark
			{
				constexpr std::array<unsigned char, 3> k_u8{ 0xEF, 0xBB, 0xBF };
				constexpr std::array<unsigned char, 2> k_u16_le{ 0xFF, 0xFE };
				constexpr std::array<unsigned char, 2> k_u16_be{ 0xFE, 0xFF };
			}

			namespace newline
			{
				constexpr std::array<unsigned char, 2> k_ascii_cr_lf{ 0x0D, 0x0A };
				constexpr std::array<unsigned char, 4> k_u16le_cr_lf{ 0x0D, 0x00, 0x0A, 0x00 };
				//constexpr std::array<unsigned char, 1> k_ascii_lf{ 0x0A };
				//constexpr std::array<unsigned char, 2> k_u16le_lf{ 0x0A, 0x00 };

				inline const auto& ascii() { return k_ascii_cr_lf; } // returns default ascii newline
				inline const auto& u16le() { return k_u16le_cr_lf; } // returns default u16le newline
			}

			template <class StringBuffer>
			bool write_text_file(
				const std::wstring& file_path,
				const std::string& str,
				const StringBuffer& bom = std::string{}
			)
			{
				write_text_file(file_path, str, bom.data(), bom.size());
			}

			bool write_text_file(
				const std::wstring& file_path,
				const std::string& str,
				const unsigned char* bom_data = nullptr,
				std::size_t bom_bytes = 0
			);
		}
	}
}