#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <regex>
#include <set>

#include <shlobj.h>

#include <boost/filesystem.hpp>

namespace std {

	template<typename C, typename T>
	[[nodiscard]] bool binary_search(C& c, const T& val) noexcept { return std::binary_search(c.begin(), c.end(), val); }

	template<typename C>
	void sort(C& c) noexcept { std::sort(c.begin(), c.end()); }

	template<typename C, typename FwIt = C::iterator>
	[[nodiscard]] FwIt unique(C& c) noexcept { return std::unique(c.begin(), c.end()); }
}

namespace std_boost {

	struct less {

		[[nodiscard]] const bool operator()(const std::string& _Left, const std::string& _Right) const {
			return _strcmpi(boost::filesystem::path(_Left).lexically_normal().string().c_str(),
							boost::filesystem::path(_Right).lexically_normal().string().c_str()) < 0;
		}
	};

	struct equali_to {

		[[nodiscard]] const bool operator()(const std::string& _Left, const std::string& _Right) const {
			return _strcmpi(_Left.c_str(), _Right.c_str()) == 0;
		}
	};

	using set = std::set<std::string, less>;
}

constexpr std::size_t operator"" _hash(const char* str, const std::size_t size)
{
	std::size_t val = std::_FNV_offset_basis;

	for (std::size_t idx{}; idx < size; ++idx) {
		val ^= static_cast<std::size_t>(str[idx]);
		val *= std::_FNV_prime;
	}

	return val;
}

constexpr std::string_view operator"" _sv(const char* str, const std::size_t size)
{
	return std::basic_string_view(str, size);
}
