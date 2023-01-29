#pragma once

#include "std_libraries.h"
#include "f4se_libraries.h"

namespace Version {

	constexpr char* Name = "Fallout 4 High Heels System";
	constexpr char* ShortName = "FO4HHS";
	
	constexpr std::uint32_t Major = 1;
	constexpr std::uint32_t Minor = 8;	

	constexpr const float GetVersion() { return static_cast<float>(Major) + static_cast<float>(Minor) / 10.0f; }
}
