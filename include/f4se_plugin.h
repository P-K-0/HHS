#pragma once

#include "version.h"

#include "util.h"

namespace f4se {

	class Plugin :
		public util::Singleton<Plugin> {
		friend class util::Singleton<Plugin>;

	public:

		[[nodiscard]] bool Query(const F4SEInterface* a_f4se, PluginInfo* a_info) noexcept;
		[[nodiscard]] bool Load(const F4SEInterface* a_f4se) noexcept;

		void AddTask(ITaskDelegate* tsk) noexcept;

		[[nodiscard]] bool IsRuntimeValid() const noexcept { return isValid; }

	private:

		Plugin() noexcept = default;
		~Plugin() noexcept = default;

		static void MsgCallback(F4SEMessagingInterface::Message* msg) noexcept;

		[[nodiscard]] bool LoadInterfaces(const F4SEInterface* f4se) noexcept;
		[[nodiscard]] bool Register() noexcept;

		[[nodiscard]] bool CheckPluginVersion(const F4SEInterface* f4se) noexcept;
		[[nodiscard]] std::string GetVersionString(std::uint32_t version) noexcept;
		void PrintInfoPlugin(const std::string& version) noexcept;
		void ErrMessageBox(const F4SEInterface* f4se, const std::string& version) noexcept;

		F4SEMessagingInterface* messaging{ nullptr };
		F4SETaskInterface* task{ nullptr };
		F4SEPapyrusInterface* papyrus{ nullptr };
		F4SEScaleformInterface* scaleform{ nullptr };
	
		bool isValid{};

		PluginHandle pluginHandle;
		IDebugLog iLog;
	};
}
