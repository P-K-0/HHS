
#include "aaf.h"

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME

namespace Aaf {

	VMArgs::VMArgs(const VMValue* args) noexcept
	{
		UnpackValue(&vmVar, (VMValue*)args);
	}

	template<typename T>
	T VMArgs::As(std::uint32_t index) noexcept
	{
		if (index >= vmVar.Length())
			return T{};

		T value{};

		Get(vmVar, index, value);

		return value;
	}

	template<typename T>
	std::vector<T> VMArgs::AsArray(std::uint32_t index) noexcept
	{
		if (index >= vmVar.Length())
			return std::vector<T>{};

		VMVariable ret{};

		vmVar.Get(&ret, index);

		auto& v = ret.GetValue();

		if (!v.data.arr)
			return std::vector<T>{};

		std::vector<T> vValues;

		for (std::uint32_t idx{}; idx < v.data.arr->arr.count; idx++) {

			auto value = v.data.arr->arr[idx];

			if (value.GetTypeEnum() == VMValue::kType_Variable && value.data.var) {

				Push(vValues, (*value.data.var));
			
				continue;
			}

			Push(vValues, value);
		}

		return vValues;
	}

	void VMArgs::Get(VMArray<VMVariable>& var, std::uint32_t index, int& value) noexcept
	{
		VMVariable ret{};

		var.Get(&ret, index);

		auto& v = ret.GetValue();

		value = v.data.i;
	}

	void VMArgs::Get(VMArray<VMVariable>& var, std::uint32_t index, float& value) noexcept
	{
		VMVariable ret{};

		var.Get(&ret, index);

		auto& v = ret.GetValue();

		value = v.data.f;
	}

	void VMArgs::Get(VMArray<VMVariable>& var, std::uint32_t index, bool& value) noexcept
	{
		VMVariable ret{};

		var.Get(&ret, index);

		auto& v = ret.GetValue();

		value = v.data.b;
	}

	void VMArgs::Get(VMArray<VMVariable>& var, std::uint32_t index, std::uint64_t& value) noexcept
	{
		VMVariable ret{};

		var.Get(&ret, index);

		auto& v = ret.GetValue();

		auto id = v.data.id;

		if (!id) {

			value = 0;

			return;
		}

		value = id->GetHandle();
	}

	void VMArgs::Get(VMArray<VMVariable>& var, std::uint32_t index, std::string& value) noexcept
	{
		VMVariable ret{};

		var.Get(&ret, index);

		auto& v = ret.GetValue();

		auto str = v.data.GetStr();

		if (!str) {

			value.clear();

			return;
		}

		value = str->c_str();
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

		if (!id)
			return;

		vector.push_back(id->GetHandle());
	}

	void VMArgs::Push(std::vector<std::string>& vector, VMValue& value) noexcept
	{
		auto str = value.data.GetStr();

		if (!str)
			return;

		vector.push_back(str->c_str());
	}

	void VMArgs::Push(std::vector<bool>& vector, VMValue& value) noexcept
	{
		vector.push_back(value.data.b);
	}

	void Scene::StartStop(const std::uint64_t Handle, const bool& bStop, const bool& bTag) noexcept
	{
		std::uint32_t id = static_cast<std::uint32_t>(Handle & MaskId);

		if (id == 0 || id == MaskId)
			return;

		bool isStop{ bTag ? false : bStop };

		hhs::Map::GetInstance().visit(false, id, [&](hhs::System& sys) {

			return isStop ? sys.Stop(true) : sys.Start();
		});
	}

	void Scene::OnSceneInit(const VMValue* args) noexcept
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

	void Scene::OnSceneEnd(const VMValue* args, const bool& stop) noexcept
	{
		VMArgs vmArgs{ args };

		auto status = vmArgs.As<int>(0);

		auto actors = vmArgs.AsArray<std::uint64_t>(1);

		auto position = vmArgs.As<std::string>(2);

		auto tags = vmArgs.AsArray<std::string>(3);

		bool hasTag{};

		for (auto tag : tags) {

			if (Settings::Ini::GetInstance().CheckTagAAF(tag)) {

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
		auto key = std::hash<std::string>{}(eventName->c_str());

		switch (key) {

		case "aaf:aaf_api_OnSceneInit"_hash:

			OnSceneInit(args);

			break;

		case "aaf:aaf_api_OnAnimationStart"_hash:
		case "aaf:aaf_api_OnAnimationChange"_hash:

			OnSceneEnd(args, true);

			break;

		case "aaf:aaf_api_OnSceneEnd"_hash:

			OnSceneEnd(args);

			break;
		}
	}

	void Event::CustomEvent(VirtualMachine* vm, std::uint64_t unk1, VMIdentifier* sender, const BSFixedString* eventName, VMValue* args) noexcept
	{
		if (eventName && args && Settings::Ini::GetInstance().Get_bEnableAAF())
			Scene::GetInstance().ProcessEvent(eventName, args);

		SendCustomEvent_Original(vm, unk1, sender, eventName, args);
	}

	bool Event::CheckPluginsInstalled() noexcept
	{
		_DMESSAGE("Looking for %s", PluginAAF);

		VisitMods([&](ModInfo* mod) {

			if (_strcmpi(mod->name, PluginAAF) == 0)
				hasAAF = true;

			if (_strcmpi(mod->name, PluginFO4HHS) == 0)
				hasPlugin = true;

			return hasAAF && hasPlugin;
		});

		_DMESSAGE("%s %sfound!", PluginAAF, hasAAF ? "" : "not ");

		if (!hasAAF)
			return false;

		if (hasPlugin) {

			_DMESSAGE("%s found... skip injection code!", PluginFO4HHS);

			return false;
		}
		
		_DMESSAGE("%s not found... init injection code!", PluginFO4HHS);

		return true;
	}

	void Event::Hook() noexcept
	{
		if (hasPlugin || hooked)
			return;

		if (!CheckPluginsInstalled())
			return;

		std::size_t len{ 65536 }; 

		if (!g_branchTrampoline.Create(len)) {

			_ERROR("Branch Trampoline init error!");

			return;
		}

		if (!g_localTrampoline.Create(len, g_moduleHandle)) {

			_ERROR("Codegen buffer init error!");

			return;
		}

		struct SendCustomEvent_Code : Xbyak::CodeGenerator {

			SendCustomEvent_Code(void* buf)
				: Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(rax, rsp);
				mov(ptr[rax + 0x08], rbx);

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(SendCustomEvent_Internal.GetUIntPtr() + 0x07);
			}
		};

		void* codeBuf = g_localTrampoline.StartAlloc();

		SendCustomEvent_Code code(codeBuf);

		g_localTrampoline.EndAlloc(code.getCurr());

		SendCustomEvent_Original = (_SendCustomEvent)codeBuf;

		if (g_branchTrampoline.Write5Branch(SendCustomEvent_Internal.GetUIntPtr(), (uintptr_t)CustomEvent)) {

			_DMESSAGE("Code injected successfully!");
		}

		return;
	}
	
	Scene Scene::instance;

	Event Event::instance;
}

#endif
