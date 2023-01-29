#pragma once

#include "version.h"

#include "externals.h"

namespace Text {

	extern [[nodiscard]] float Parser(const std::string& str) noexcept;
	extern [[nodiscard]] float GetHeightFromText(const std::string& filename) noexcept;
	extern [[nodiscard]] bool CreateHeightFile(const std::string& Filename, const float value) noexcept;
	extern [[nodiscard]] bool RemoveHeightFile(const std::string& Filename) noexcept;
}
