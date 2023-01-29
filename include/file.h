#pragma once

#include "version.h"

namespace File {

	class Reader {

	public:

		Reader() noexcept {}
		Reader(const std::string& Filename) noexcept : filename{ Filename } {}
		~Reader() noexcept {}

		Reader(const Reader&) = delete;
		Reader(Reader&&) = delete;

		Reader& operator=(const Reader&) = delete;
		Reader& operator=(Reader&&) = delete;

		[[nodiscard]] std::uint32_t Read(std::string& dst, const std::uint32_t len = -1) noexcept;

	private:

		std::string filename;

		[[nodiscard]] std::uint32_t InputStreamRead(std::string& dst, const std::uint32_t len) noexcept;
		[[nodiscard]] std::uint32_t BSResourceNiBinaryStreamRead(std::string& dst, const std::uint32_t len) noexcept;
	};
}
