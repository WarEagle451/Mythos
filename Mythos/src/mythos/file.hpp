#pragma once
#include "defines.hpp"

#include <string>
#include <filesystem>
#include <fstream>

namespace myl {
	/// MYBug: This cannot read spv shader binary, why? Need to read it as char then convert to u32
	template<typename CharT = char>
	MYL_NO_DISCARD std::basic_string<CharT> load_into_memory(const std::filesystem::path& file) {
		if (std::basic_ifstream<CharT> in(file, std::ios::binary); in) {
			std::basic_string<CharT> content{};
			content.resize(std::filesystem::file_size(file));
			in.read(content.data(), content.size());
			return content;
		}

		return std::basic_string<CharT>{};
	}
}