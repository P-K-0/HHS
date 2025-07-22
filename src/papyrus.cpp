
#include "papyrus.h"

#include "f4se_plugin.h"
#include "externals.h"
#include "events.h"
#include "settings.h"
#include "hhs.h"
#include "ingame.h"

namespace Papyrus {

	float GetVersion_HHS(StaticFunctionTag* base)
	{
		return Version::GetVersion();
	}

	bool IsValidF4SE_HHS(StaticFunctionTag* base)
	{
		return f4se::Plugin::GetSingleton().IsRuntimeValid();
	}
	
	SInt32 SetFloatHeight_HHS(StaticFunctionTag* base, TESObjectREFR* akActor, float height)
	{
		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			auto err = sys.SetHeight(height);

			if (err == hhs::Error::Success) {
				sys.SetOverride(true);
			}

			return err;
		});

		return hhs::cast_error<SInt32>(ret);
	}

	bool RemoveHeight_HHS(StaticFunctionTag* base, TESObjectREFR* akActor)
	{
		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			sys.SetOverride(false);

			return hhs::Error::Success;
		});

		return ret == hhs::Error::Success;
	}

	bool StartHeight_HHS(StaticFunctionTag* base, TESObjectREFR* akActor)
	{
		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			return sys.Start();
		});

		return ret == hhs::Error::Success;
	}

	bool StopHeight_HHS(StaticFunctionTag* base, TESObjectREFR* akActor)
	{
		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			return sys.Stop();
		});

		return ret == hhs::Error::Success;
	}

	bool EnableFix(StaticFunctionTag* base, TESObjectREFR* akActor, TESObjectREFR* furniture)
	{
		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			sys.EnableFix(furniture);

			return hhs::Error::Success;
		});

		return ret == hhs::Error::Success;
	}

	bool DisableFix(StaticFunctionTag* base, TESObjectREFR* akActor)
	{
		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			sys.DisableFix();

			return hhs::Error::Success;
		});

		return ret == hhs::Error::Success;
	}

	bool StartHeightAAF_HHS(StaticFunctionTag* base, TESObjectREFR* akActor)
	{
		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			return sys.Start();
		});

		return ret == hhs::Error::Success;
	}

	bool StopHeightAAF_HHS(StaticFunctionTag* base, TESObjectREFR* akActor)
	{
		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			return sys.Stop(true);
		});

		return ret == hhs::Error::Success;
	}

	bool HasTagAAF_HHS(StaticFunctionTag* base, BSFixedString tag)
	{
		return Settings::Ini::GetSingleton().CheckTagAAF(tag.c_str());
	}

	float GetFloatHeight_HHS(StaticFunctionTag* base, TESObjectREFR* akActor)
	{
		float h{ ZeroValue };

		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			h = sys.GetHeight();
		
			return hhs::Error::Success;
		});

		return h;
	}

	bool HasHeight_HHS(StaticFunctionTag* base, TESObjectREFR* akActor)
	{
		bool hasHeight{};

		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			hasHeight = sys.HasHeight();

			return hhs::Error::Success;
		});

		return hasHeight;
	}

	bool HasOverride_HHS(StaticFunctionTag* base, TESObjectREFR* akActor)
	{
		bool hasOverride{};

		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			hasOverride = sys.HasOverride();

			return hhs::Error::Success;
		});

		return hasOverride;
	}

	bool IsStop_HHS(StaticFunctionTag* base, TESObjectREFR* akActor)
	{
		bool isStop{};

		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			isStop = sys.IsStop();

			return hhs::Error::Success;
		});

		return isStop;
	}

	bool IsSwimming_HHS(StaticFunctionTag* base, TESObjectREFR* akActor)
	{
		if (!akActor) {
			return false;
		}

		auto actor = DYNAMIC_CAST(akActor, TESObjectREFR, Actor);

		if (!actor) {
			return false;
		}

		return actor->IsSwimming();
	}

	bool HasKeywordException_HHS(StaticFunctionTag* base, TESObjectREFR* object)
	{
		return Settings::Ini::GetSingleton().CheckFurnitureBehavior(object);
	}

	void StartStopPlayer_HHS(StaticFunctionTag* base)
	{
		if (!Events::Dispatcher::GetSingleton().GetInputEnabled())
			return;

		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, PlayerID, [&](hhs::System& sys) {

			if (sys.IsStop()) {
				return sys.Start();
			}

			return sys.Stop(true);
		});
	}

	void StartStopAllActorsInCell_HHS(StaticFunctionTag* base)
	{
		if (!Events::Dispatcher::GetSingleton().GetInputEnabled()) {
			return;
		}

		VisitCell([&](TESObjectREFR* refr) {

			hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, refr, [&](hhs::System& sys) {

				if (sys.IsStop()) {

					return sys.Start();
				}

				return sys.Stop(true);
			});
		});
	}

	void ActivateEdit_HHS(StaticFunctionTag* base) 
	{
		if (!Events::Dispatcher::GetSingleton().GetInputEnabled()) {
			return;
		}

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		InGame::HeightEdit::GetSingleton().OnKeyPress(InGame::Key::Activate);
#endif
	}

	void ChangeRefr_HHS(StaticFunctionTag* base)
	{
		if (!Events::Dispatcher::GetSingleton().GetInputEnabled()) {
			return;
		}

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		InGame::HeightEdit::GetSingleton().OnKeyPress(InGame::Key::ChangeReference);
#endif
	}

	void CreateHeight_HHS(StaticFunctionTag* base)
	{
		if (!Events::Dispatcher::GetSingleton().GetInputEnabled()) {
			return;
		}

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		InGame::HeightEdit::GetSingleton().OnKeyPress(InGame::Key::Create);
#endif
	}

	void DeleteHeight_HHS(StaticFunctionTag* base)
	{
		if (!Events::Dispatcher::GetSingleton().GetInputEnabled()) {
			return;
		}

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		InGame::HeightEdit::GetSingleton().OnKeyPress(InGame::Key::Delete);
#endif
	}

	void IncrementHeight_HHS(StaticFunctionTag* base)
	{
		if (!Events::Dispatcher::GetSingleton().GetInputEnabled()) {
			return;
		}

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		InGame::HeightEdit::GetSingleton().OnKeyPress(InGame::Key::Increment);
#endif
	}

	void DecrementHeight_HHS(StaticFunctionTag* base)
	{
		if (!Events::Dispatcher::GetSingleton().GetInputEnabled()) {
			return;
		}

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		InGame::HeightEdit::GetSingleton().OnKeyPress(InGame::Key::Decrement);
#endif
	}

	float GetTransform_HHS(StaticFunctionTag* base, TESObjectREFR* akActor, BSFixedString node, SInt32 flags)
	{
		float value{};

		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			value = sys.GetTransform(node.c_str(), Node::flags_cast(flags));

			return hhs::Error::Success;
		});

		return value;
	}

	SInt32 SetTransform_HHS(StaticFunctionTag* base, TESObjectREFR* akActor, BSFixedString node, SInt32 flags, float value)
	{
		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			if (sys.SetTransform(node.c_str(), Node::flags_cast(flags), value) != 0) {
				return hhs::Error::Unknown;
			}

			return hhs::Error::Success;
		});

		return hhs::cast_error<SInt32>(ret);
	}

	SInt32 ResetTransform_HHS(StaticFunctionTag* base, TESObjectREFR* akActor, BSFixedString node, SInt32 flags)
	{
		auto ret = hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, akActor, [&](hhs::System& sys) {

			if (sys.ResetTransform(node.c_str(), Node::flags_cast(flags)) != 0) {
				return hhs::Error::Unknown;
			}

			return hhs::Error::Success;
		});

		return hhs::cast_error<SInt32>(ret);
	}

	SInt32 GetSAFVersion_HHS(StaticFunctionTag* base, TESObjectREFR* akActor, bool isFemale)
	{
		Skeleton::Reader reader{ akActor, isFemale };

		std::uint32_t version{};

		if (!reader.GetExtraData(ExtraDataSAF, version)) {
			return -1;
		}

		return static_cast<SInt32>(version);
	}

	bool Register(VirtualMachine* vm) noexcept
	{
		if (!Settings::Ini::GetSingleton().Get_bEnableScript()) {
			return false;
		}

#define GET_FUNCTION(func) # func, "HHS", func, vm

		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, float>(GET_FUNCTION(GetVersion_HHS)));

		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, bool>(GET_FUNCTION(IsValidF4SE_HHS)));

		vm->RegisterFunction(new NativeFunction2<StaticFunctionTag, SInt32, TESObjectREFR*, float>(GET_FUNCTION(SetFloatHeight_HHS)));

		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, TESObjectREFR*>(GET_FUNCTION(RemoveHeight_HHS)));

		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, TESObjectREFR*>(GET_FUNCTION(StartHeight_HHS))); 

		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, TESObjectREFR*>(GET_FUNCTION(StopHeight_HHS))); 

		vm->RegisterFunction(new NativeFunction2<StaticFunctionTag, bool, TESObjectREFR*, TESObjectREFR*>(GET_FUNCTION(EnableFix)));

		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, TESObjectREFR*>(GET_FUNCTION(DisableFix)));

		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, TESObjectREFR*>(GET_FUNCTION(StartHeightAAF_HHS)));

		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, TESObjectREFR*>(GET_FUNCTION(StopHeightAAF_HHS)));

		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, BSFixedString>(GET_FUNCTION(HasTagAAF_HHS)));

		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, float, TESObjectREFR*>(GET_FUNCTION(GetFloatHeight_HHS))); 

		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, TESObjectREFR*>(GET_FUNCTION(HasHeight_HHS)));

		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, TESObjectREFR*>(GET_FUNCTION(HasOverride_HHS)));

		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, TESObjectREFR*>(GET_FUNCTION(IsStop_HHS))); 

		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, TESObjectREFR*>(GET_FUNCTION(IsSwimming_HHS)));

		vm->RegisterFunction(new NativeFunction1<StaticFunctionTag, bool, TESObjectREFR*>(GET_FUNCTION(HasKeywordException_HHS)));

		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>(GET_FUNCTION(StartStopPlayer_HHS)));

		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>(GET_FUNCTION(StartStopAllActorsInCell_HHS))); 

		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>(GET_FUNCTION(ActivateEdit_HHS)));

		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>(GET_FUNCTION(CreateHeight_HHS)));
		
		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>(GET_FUNCTION(DeleteHeight_HHS)));

		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>(GET_FUNCTION(ChangeRefr_HHS)));
		
		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>(GET_FUNCTION(IncrementHeight_HHS)));
		
		vm->RegisterFunction(new NativeFunction0<StaticFunctionTag, void>(GET_FUNCTION(DecrementHeight_HHS)));

		vm->RegisterFunction(new NativeFunction3<StaticFunctionTag, float, TESObjectREFR*, BSFixedString, SInt32>(GET_FUNCTION(GetTransform_HHS)));

		vm->RegisterFunction(new NativeFunction4<StaticFunctionTag, SInt32, TESObjectREFR*, BSFixedString, SInt32, float>(GET_FUNCTION(SetTransform_HHS)));

		vm->RegisterFunction(new NativeFunction3<StaticFunctionTag, SInt32, TESObjectREFR*, BSFixedString, SInt32>(GET_FUNCTION(ResetTransform_HHS)));

		vm->RegisterFunction(new NativeFunction2<StaticFunctionTag, SInt32, TESObjectREFR*, bool>(GET_FUNCTION(GetSAFVersion_HHS)));

		_DMESSAGE("Papyrus functions registered successfully!");

		return true;
	}
}
