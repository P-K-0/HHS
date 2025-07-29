#pragma once

#include "version.h"

namespace Papyrus {

	extern [[nodiscard]] bool Register(VirtualMachine* vm) noexcept;

	inline void Notification(const char* message) noexcept { CallGlobalFunctionNoWait1("Debug", "Notification", BSFixedString(message)); }

	inline void Notification(const char* message, std::uint32_t id) noexcept 
	{ 
		std::stringstream ss;

		ss << message << " " << std::hex << std::uppercase << id;

		CallGlobalFunctionNoWait1("Debug", "Notification", BSFixedString(ss.str().c_str())); 
	}

	inline void Message_Box(const char* message) noexcept { CallGlobalFunctionNoWait1("Debug", "MessageBox", BSFixedString(message)); }
}
