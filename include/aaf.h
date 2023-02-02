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

	enum class Scenes : std::size_t {

		OnSceneInit = 0xE79169C1EB02B794,
		OnSceneEnd = 0x99E0925B288714AB,
		OnAnimStart = 0xBA9490EA4EB21FF0,
		OnAnimChange = 0x745E7A622197EEC
	};

	struct Value {

		std::uint8_t Type{};
		std::uint64_t Id{};	
		std::int32_t Int{};
		float Float{};
		bool Bool{};	
		std::string Str;
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

		void Push(const VMValue* value, const bool recursive = false) noexcept;
		void ParseVMValue(const VMValue* args) noexcept;
		void StartStop(const std::uint64_t Handle, const bool& bStop, const bool& bTag) noexcept;

		void OnSceneInit(const VMValue* args) noexcept;
		void OnSceneEnd(const VMValue* args, const bool& stop = false) noexcept;

		static Scene instance;

		std::uint64_t uDoppelganger;
		std::vector<Value> values;
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
	
		void* g_moduleHandle{ nullptr };
		
		bool hooked{};
		bool hasPlugin{};
		bool hasAAF{};
	};
}

#endif
