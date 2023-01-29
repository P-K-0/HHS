#pragma once

#include "version.h"

namespace Papyrus {

	extern [[nodiscard]] bool Register(VirtualMachine* vm) noexcept;

	inline void Notification(const std::string& message) noexcept { CallGlobalFunctionNoWait1("Debug", "Notification", BSFixedString(message.c_str())); }

	inline void Notification(const std::string& message, const std::uint32_t& id) noexcept 
	{ 
		std::stringstream ss;

		ss << message << " " << std::hex << std::uppercase << id;

		CallGlobalFunctionNoWait1("Debug", "Notification", BSFixedString(ss.str().c_str())); 
	}

	inline void Message_Box(const std::string& message) noexcept { CallGlobalFunctionNoWait1("Debug", "MessageBox", BSFixedString(message.c_str())); }
}
