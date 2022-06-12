#pragma once
#include "defines.hpp"

#include <string>
#include <filesystem>
#include <fstream>
#include <vector>

namespace myl {
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

	template<typename IntT = u8>
	MYL_NO_DISCARD std::vector<IntT> load_into_memory_binary(const std::filesystem::path& file) {
		if (std::ifstream in(file, std::ios::binary); in) {
			std::vector<IntT> binary{};
			auto size = std::filesystem::file_size(file);

			binary.resize(size / sizeof(IntT));
			in.read(reinterpret_cast<std::ifstream::char_type*>(binary.data()), size);
			return binary;
		}

		return std::vector<IntT>{};
	}
}