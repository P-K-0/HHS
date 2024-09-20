#pragma once

#include "version.h"

#include "externals.h"
#include "settings.h"
#include "hhs.h"

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME

namespace Aaf {

	using _SendCustomEvent = void (*)(VirtualMachine*, std::uint64_t, VMIdentifier*, const BSFixedString*, VMValue*);

	static _SendCustomEvent SendCustomEvent_Original;

	constexpr char* PluginAAF = "AAF.esm";
	constexpr char* PluginFO4HHS = "FO4HHS_AAF.esp";

	constexpr std::uint32_t MaskId = 0xffffffff;

	class VMArgs {

	public:

		VMArgs() = delete;

		VMArgs(const VMArgs&) = delete;
		VMArgs(VMArgs&&) = delete;

		VMArgs& operator=(const VMArgs&) = delete;
		VMArgs& operator=(VMArgs&&) = delete;

		explicit VMArgs(const VMValue* args) noexcept;
		~VMArgs() noexcept {}

		template<typename T>
		[[nodiscard]] T As(std::uint32_t index) noexcept;

		template<typename T>
		[[nodiscard]] std::vector<T> AsArray(std::uint32_t index) noexcept;

	private:

		void Get(VMArray<VMVariable>& var, std::uint32_t index, int& value) noexcept;
		void Get(VMArray<VMVariable>& var, std::uint32_t index, float& value) noexcept;
		void Get(VMArray<VMVariable>& var, std::uint32_t index, bool& value) noexcept;
		void Get(VMArray<VMVariable>& var, std::uint32_t index, std::uint64_t& value) noexcept;
		void Get(VMArray<VMVariable>& var, std::uint32_t index, std::string& value) noexcept;

		void Push(std::vector<int>& vector, VMValue& value) noexcept;
		void Push(std::vector<float>& vector, VMValue& value) noexcept;
		void Push(std::vector<std::uint64_t>& vector, VMValue& value) noexcept;
		void Push(std::vector<std::string>& vector, VMValue& value) noexcept;
		void Push(std::vector<bool>& vector, VMValue& value) noexcept;

		VMArray<VMVariable> vmVar;
	};

	class Scene {

	public:

		[[nodiscard]] static Scene& GetInstance() noexcept { return instance; }

		void ProcessEvent(const BSFixedString* name, VMValue* args) noexcept;

	private:

		Scene() noexcept {}
		~Scene() noexcept {}

		Scene(const Scene&) = delete;
		Scene(Scene&&) = delete;

		Scene& operator=(const Scene&) = delete;
		Scene& operator=(Scene&&) = delete;

		void StartStop(const std::uint64_t Handle, const bool& bStop, const bool& bTag) noexcept;

		void OnSceneInit(const VMValue* args) noexcept;
		void OnSceneEnd(const VMValue* args, const bool& stop = false) noexcept;

		static Scene instance;

		std::uint64_t uDoppelganger;
	};

	class Event {

	public:

		[[nodiscard]] static Event& GetInstance() noexcept { return instance; }
	
		void Hook() noexcept;

	private:

		Event() noexcept {}
		~Event() noexcept {}

		Event(const Event&) = delete;
		Event(Event&&) = delete;

		Event& operator=(const Event&) = delete;
		Event& operator=(Event&&) = delete;

		[[nodiscard]] bool CheckPluginsInstalled() noexcept;

		static void CustomEvent(VirtualMachine* vm, std::uint64_t unk1, VMIdentifier* sender, const BSFixedString* eventName, VMValue* args) noexcept;

		static Event instance;
		
		bool hooked{};
		bool hasPlugin{};
		bool hasAAF{};
	};
}

#endif
