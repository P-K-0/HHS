
#include "aaf.h"

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME

namespace Aaf {

	void Scene::Push(const VMValue* value, const bool recursive) noexcept
	{
		if (!value) 
			return;

		Value val;
		
		val.Type = value->GetTypeEnum();

		switch (val.Type) {

		case VMValue::kType_Int: 

			val.Int = value->data.i;

			break;

		case VMValue::kType_Bool: 

			val.Bool = value->data.b; 

			break;

		case VMValue::kType_Float:
			
			val.Float = value->data.f;

			break;

		case VMValue::kType_String:

			if (value->data.GetStr()) {

				val.Str = value->data.GetStr()->c_str();
			}
			else {

				return;
			}

			break;

		case VMValue::kType_Identifier:
		{
			VMIdentifier* id = value->data.id;

			if (id) {

				val.Id = id->GetHandle();
			}
			else {

				return;
			}

			break;
		}
		case VMValue::kType_Variable:

			if (recursive) return;

			Push(value->data.var, true);

			break;

		//case VMValue::kType_Struct: 

		default: 

			return;
		}

		values.push_back(val);
	}

	void Scene::ParseVMValue(const VMValue* args) noexcept
	{
		if (!args)
			return;

		values.clear();
		
		VMArray<VMVariable> var;
		VMVariable vmvar;

		UnpackValue(&var, (VMValue*)args);

		for (std::uint32_t idx{}; idx < var.Length(); idx++) {

			var.Get(&vmvar, idx);

			auto value = vmvar.GetValue();

			switch (value.GetTypeEnum()) {

			case VMValue::kType_VariableArray:
			case VMValue::kType_IntArray:
			case VMValue::kType_BoolArray:
			case VMValue::kType_FloatArray:
			case VMValue::kType_StringArray:
			case VMValue::kType_StructArray:
			case VMValue::kType_IdentifierArray:

				if (value.data.arr) {

					for (std::uint32_t iarr{}; iarr < value	.data.arr->arr.count; iarr++) {

						auto valueArray = value.data.arr->arr[iarr];

						Push(&valueArray);
					}
				}

				break;

			default: 

				Push(&value);

				break;
			}
		}
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
		ParseVMValue(args);

		if (values.empty() || values.size() <= 1)
			return;

		auto status = values[0].Int;
		
		if (values[0].Type == VMValue::kType_Float)
			status = static_cast<std::int32_t>(values[0].Float);

		if (status != 0)
			return;

		uDoppelganger = InvalidSlot;

		for (auto& v = values.end(); v != values.begin(); v--) {

			if (v->Type == VMValue::kType_Identifier) {

				if (uDoppelganger == InvalidSlot)
					uDoppelganger = v->Id;

				StartStop(v->Id, true, false);
			}
		}
	}

	void Scene::OnSceneEnd(const VMValue* args, const bool& stop) noexcept
	{
		bool hasTag{};

		for (auto& v = values.end(); v != values.begin(); v--) {

			if (v->Type == VMValue::kType_String && Settings::Ini::GetInstance().CheckTagAAF(v->Str))
				hasTag = true;

			if (v->Type == VMValue::kType_Identifier)
				StartStop(v->Id, stop, hasTag);
		}

		StartStop(uDoppelganger, stop, hasTag);
	}

	void Scene::ProcessEvent(const BSFixedString* eventName, VMValue* args) noexcept
	{
		auto key = static_cast<Scenes>(std::hash<std::string>{}(eventName->c_str()));

		switch (key) {

		case Scenes::OnSceneInit:

			OnSceneInit(args);

			break;

		case Scenes::OnAnimStart:
		case Scenes::OnAnimChange:

			OnSceneEnd(args, true);

			break;

		case Scenes::OnSceneEnd:

			OnSceneEnd(args);

			break;

		default:

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
				mov(ptr[rax + 0x10], rbp);
				mov(ptr[rax + 0x20], rdi);
				push(r14);

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(SendCustomEvent_Internal.GetUIntPtr() + 0x11);
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
