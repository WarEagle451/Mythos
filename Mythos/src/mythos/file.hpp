#pragma once
#include "defines.hpp"

#include <string>
#include <filesystem>
#include <fstream>

namespace myl {
	template<class CharT = char>
	MYL_NO_DISCARD std::basic_string<CharT> load_into_memory(const std::filesystem::path& file, std::ios_base::openmode mode = std::ios::binary) {
		if (std::basic_ifstream<CharT> in(file, mode); in.is_open())
			return std::basic_string<CharT>(std::istreambuf_iterator<CharT>(in), std::istreambuf_iterator<CharT>());
		return std::basic_string<CharT>();
	}
}