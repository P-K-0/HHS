#pragma once

#include "std_libraries.h"
#include "f4se_libraries.h"

namespace Version {

	constexpr auto Name = "Fallout 4 High Heels System"_sv;
	constexpr auto ShortName = "FO4HHS"_sv;
	
	constexpr std::uint32_t Major = 1;
	constexpr std::uint32_t Minor = 8;
	constexpr std::uint32_t Build = 9;

	constexpr const float GetVersion() { return static_cast<float>(Major) + static_cast<float>(Minor) / 10.0f; }
}
