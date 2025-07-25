#pragma once

#include "version.h"

#include "boost/filesystem.hpp"

namespace File {

	class Reader {

	public:

		Reader() = delete;
		Reader(const std::string& Filename) noexcept : filename(Filename) {}
		~Reader() noexcept {}

		Reader(const Reader&) = delete;
		Reader(Reader&&) = delete;

		Reader& operator=(const Reader&) = delete;
		Reader& operator=(Reader&&) = delete;

		[[nodiscard]] bool Read(std::string& dst) noexcept;

		[[nodiscard]] const std::string& Filename() const noexcept { return filename; }

	private:

		std::string filename;
		std::uint32_t len{};

		[[nodiscard]] std::uint32_t InputStreamRead(std::string& dst, std::uint32_t len) noexcept;
		[[nodiscard]] std::uint32_t BSResourceNiBinaryStreamRead(std::string& dst, std::uint32_t len) noexcept;
	};

	extern [[nodiscard]] std::string GetRelativeDir(const std::string& str) noexcept;
}
