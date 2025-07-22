#pragma once

#include "version.h"

namespace f4se {

	class Plugin {

	public:

		[[nodiscard]] static Plugin& GetSingleton() noexcept
		{
			static Plugin instance;
			return instance;
		}

		[[nodiscard]] bool Query(const F4SEInterface* a_f4se, PluginInfo* a_info) noexcept;
		[[nodiscard]] bool Load(const F4SEInterface* a_f4se) noexcept;

		void AddTask(ITaskDelegate* task) noexcept;

		[[nodiscard]] const F4SEPapyrusInterface& GetPapyrusInterface() noexcept { return *f4se_papyrus_interface; }

		[[nodiscard]] bool IsRuntimeValid() const noexcept { return isValid; }

	private:

		Plugin() noexcept {}
		~Plugin() noexcept {}

		Plugin(const Plugin&) = delete;
		Plugin(Plugin&&) = delete;

		Plugin& operator=(const Plugin&) = delete;
		Plugin& operator=(Plugin&&) = delete;

		static void MsgCallback(F4SEMessagingInterface::Message* msg) noexcept;

		[[nodiscard]] static bool CheckPluginVersion(const F4SEInterface* f4se) noexcept;
		[[nodiscard]] static std::string GetVersionString(std::uint32_t version) noexcept;

		F4SEMessagingInterface* f4se_msg_interface{ nullptr };
		F4SETaskInterface* f4se_task_interface{ nullptr };
		F4SEPapyrusInterface* f4se_papyrus_interface{ nullptr };
		F4SEScaleformInterface* f4se_scaleform_interface{ nullptr };
	
		bool isValid{};

		PluginHandle hPlug;
		IDebugLog iLog;
	};
}
