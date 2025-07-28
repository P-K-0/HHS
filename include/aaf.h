#pragma once

#include "version.h"

#include "externals.h"
#include "settings.h"
#include "hhs.h"
#include "util.h"

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME

namespace Aaf {

	using _SendCustomEvent = void (*)(VirtualMachine*, std::uint64_t, VMIdentifier*, const BSFixedString*, VMValue*);

	static _SendCustomEvent SendCustomEvent_Original;

	constexpr char* PluginAAF = "AAF.esm";
	constexpr char* PluginFO4HHS = "FO4HHS_AAF.esp";

	constexpr std::uint32_t MaskId = 0xffffffff;

	enum class AAFEventName : std::size_t {

		OnSceneInit = "aaf:aaf_api_OnSceneInit"_hash,
		OnAnimationStart = "aaf:aaf_api_OnAnimationStart"_hash,
		OnAnimationChange = "aaf:aaf_api_OnAnimationChange"_hash,
		OnSceneEnd = "aaf:aaf_api_OnSceneEnd"_hash
	};

	class VMArgs : 
		public util::NoCopyable,
		public util::NoMoveable,
		public util::NoPointer {

	public:

		VMArgs() = delete;
		VMArgs(VMValue* args) noexcept;
		~VMArgs() noexcept = default; 

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

	class Scene :
		public util::Singleton<Scene> {
		friend class util::Singleton<Scene>;

	public:

		void ProcessEvent(const BSFixedString* name, VMValue* args) noexcept;

	private:

		Scene() noexcept = default;
		~Scene() noexcept = default;

		void StartStop(std::uint64_t Handle, bool bStop, bool bTag) noexcept;

		void OnSceneInit(VMValue* args) noexcept;
		void OnSceneEnd(VMValue* args, bool stop = false) noexcept;

		std::uint64_t uDoppelganger;
	};

	class Event :
		public util::Singleton<Event> {
		friend class util::Singleton<Event>;

	public:
	
		void Hook() noexcept;

	private:

		Event() noexcept = default;
		~Event() noexcept = default;

		[[nodiscard]] bool CheckPluginsInstalled() noexcept;

		static void CustomEvent(VirtualMachine* vm, std::uint64_t unk1, VMIdentifier* sender, const BSFixedString* eventName, VMValue* args) noexcept;

		bool hooked{};
		bool hasPlugin{};
		bool hasAAF{};
	};
}

#endif
