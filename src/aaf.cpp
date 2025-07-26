
#include "aaf.h"

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME

namespace Aaf {

	VMArgs::VMArgs(VMValue* args) noexcept
	{
		UnpackValue(&vmVar, args);
	}

	template<typename T>
	T VMArgs::As(std::uint32_t index) noexcept
	{
		if (index >= vmVar.Length()) {
			return T{};
		}

		T value{};

		Get(vmVar, index, value);

		return value;
	}

	template<typename T>
	std::vector<T> VMArgs::AsArray(std::uint32_t index) noexcept
	{
		if (index >= vmVar.Length()) {
			return std::vector<T>{};
		}

		VMVariable ret{};

		vmVar.Get(&ret, index);

		auto& v = ret.GetValue();

		if (!v.data.arr) {
			return std::vector<T>{};
		}

		std::vector<T> vValues;

		for (std::uint32_t idx{}; idx < v.data.arr->arr.count; idx++) {

			auto value = v.data.arr->arr[idx];

			if (value.GetTypeEnum() == VMValue::kType_Variable && value.data.var) {

				Push(vValues, *value.data.var);
			}
			else {

				Push(vValues, value);
			}
		}

		return vValues;
	}

	void VMArgs::Get(VMArray<VMVariable>& var, std::uint32_t index, int& value) noexcept
	{
		VMVariable ret{};

		var.Get(&ret, index);

		value = ret.GetValue().data.i;
	}

	void VMArgs::Get(VMArray<VMVariable>& var, std::uint32_t index, float& value) noexcept
	{
		VMVariable ret{};

		var.Get(&ret, index);

		value = ret.GetValue().data.f;
	}

	void VMArgs::Get(VMArray<VMVariable>& var, std::uint32_t index, bool& value) noexcept
	{
		VMVariable ret{};

		var.Get(&ret, index);

		value = ret.GetValue().data.b;
	}

	void VMArgs::Get(VMArray<VMVariable>& var, std::uint32_t index, std::uint64_t& value) noexcept
	{
		value = 0;

		VMVariable ret{};

		var.Get(&ret, index);

		auto id = ret.GetValue().data.id;

		if (id) {
			value = id->GetHandle();
		}
	}

	void VMArgs::Get(VMArray<VMVariable>& var, std::uint32_t index, std::string& value) noexcept
	{
		value.clear();

		VMVariable ret{};

		var.Get(&ret, index);

		auto str = ret.GetValue().data.GetStr();

		if (str && str->c_str()) {
			value = str->c_str();
		}
	}

	void VMArgs::Push(std::vector<int>& vector, VMValue& value) noexcept
	{
		vector.push_back(value.data.i);
	}

	void VMArgs::Push(std::vector<float>& vector, VMValue& value) noexcept
	{
		vector.push_back(value.data.f);
	}

	void VMArgs::Push(std::vector<std::uint64_t>& vector, VMValue& value) noexcept
	{
		auto id = value.data.id;

		if (id) {
			vector.push_back(id->GetHandle());
		}
	}

	void VMArgs::Push(std::vector<std::string>& vector, VMValue& value) noexcept
	{
		auto str = value.data.GetStr();

		if (str && str->c_str()) {
			vector.push_back(str->c_str());
		}
	}

	void VMArgs::Push(std::vector<bool>& vector, VMValue& value) noexcept
	{
		vector.push_back(value.data.b);
	}

	void Scene::StartStop(std::uint64_t Handle, bool bStop, bool bTag) noexcept
	{
		std::uint32_t id = static_cast<std::uint32_t>(Handle & MaskId);

		if (id == 0 || id == MaskId) {
			return;
		}

		bool isStop{ bTag ? false : bStop };

		hhs::Map::GetSingleton().visit(hhs::VisitFlags::None, id, [&](hhs::System& sys) {
			return isStop ? sys.Stop(true) : sys.Start();
		});
	}

	void Scene::OnSceneInit(VMValue* args) noexcept
	{
		VMArgs vmArgs{ args };

		auto status = vmArgs.As<int>(0);

		auto actors = vmArgs.AsArray<std::uint64_t>(1);

		auto locationformID = vmArgs.As<int>(3);

		auto coords = vmArgs.AsArray<float>(4);

		for (auto actor : actors) {

			StartStop(actor, true, false);

			if ((actor & MaskId) == PlayerID) {

       			uDoppelganger = vmArgs.As<std::uint64_t>(2);
			
				StartStop(uDoppelganger, true, false);
			}
		}
	}

	void Scene::OnSceneEnd(VMValue* args, bool stop) noexcept
	{
		VMArgs vmArgs{ args };

		auto status = vmArgs.As<int>(0);

		auto actors = vmArgs.AsArray<std::uint64_t>(1);

		auto position = vmArgs.As<std::string>(2);

		auto tags = vmArgs.AsArray<std::string>(3);

		bool hasTag{};

		for (auto tag : tags) {

			if (Settings::Ini::GetSingleton().CheckTagAAF(tag)) {

				hasTag = true;

				break;
			}
		}

		for (auto actor : actors) {

			StartStop(actor, stop, hasTag);

			if ((actor & MaskId) == PlayerID) {

				StartStop(uDoppelganger, stop, hasTag);
			}
		}
	}

	void Scene::ProcessEvent(const BSFixedString* eventName, VMValue* args) noexcept
	{
		auto key = hash<AAFEventName>(eventName->c_str());

		switch (key) {

		case AAFEventName::OnSceneInit:

			OnSceneInit(args);
			break;

		case AAFEventName::OnAnimationStart:
		case AAFEventName::OnAnimationChange:

			OnSceneEnd(args, true);
			break;

		case AAFEventName::OnSceneEnd:

			OnSceneEnd(args);
			break;
		}
	}

	void Event::CustomEvent(VirtualMachine* vm, std::uint64_t unk1, VMIdentifier* sender, const BSFixedString* eventName, VMValue* args) noexcept
	{
		SendCustomEvent_Original(vm, unk1, sender, eventName, args);

		if (Settings::Ini::GetSingleton().Get_bEnableAAF() && vm && sender && eventName && eventName->c_str() && args) {
			Scene::GetSingleton().ProcessEvent(eventName, args);
		}
	}

	bool Event::CheckPluginsInstalled() noexcept
	{
		_DMESSAGE("Looking for %s", PluginAAF);

		VisitMods([&](ModInfo* mod) {

			if (_strcmpi(mod->name, PluginAAF) == 0) {
				hasAAF = true;
			}

			if (_strcmpi(mod->name, PluginFO4HHS) == 0) {
				hasPlugin = true;
			}

			return hasAAF && hasPlugin;
		});

		_DMESSAGE("%s %sfound!", PluginAAF, hasAAF ? "" : "not ");

		if (!hasAAF) {
			return false;
		}

		if (hasPlugin) {

			_DMESSAGE("%s found... skip injection code!", PluginFO4HHS);

			return false;
		}
		
		_DMESSAGE("%s not found... init injection code!", PluginFO4HHS);

		return true;
	}

	void Event::Hook() noexcept
	{
		if (hasPlugin || hooked) {
			return;
		}

		if (!CheckPluginsInstalled()) {
			return;
		}

		struct SendCustomEvent_Code : Xbyak::CodeGenerator {

			SendCustomEvent_Code(void* buf)
				: Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

#if CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163

				//.text:00000001413D9460	mov rax, rsp
				//.text:00000001413D9463	mov[rax + 8], rbx
				//.text:00000001413D9467	mov[rax + 10h], rbp

				mov(rax, rsp);
				mov(ptr[rax + 0x08], rbx);

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(SendCustomEvent_Internal.GetUIntPtr() + 0x07);
#else
				//.text:00000001410AF220	mov[rsp + 8], rbx
				//.text:00000001410AF225	mov[rsp + 10h], rbp

				mov(ptr[rsp + 0x08], rbx);

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(SendCustomEvent_Internal.GetUIntPtr() + 0x05);
#endif
			}
		};

		Trampoline::GetSingleton().Alloc<SendCustomEvent_Code>(SendCustomEvent_Original);

		if (g_branchTrampoline.Write5Branch(SendCustomEvent_Internal.GetUIntPtr(), (uintptr_t)CustomEvent)) {
			_DMESSAGE("Code injected successfully!");
		}

		return;
	}
}
#endif
