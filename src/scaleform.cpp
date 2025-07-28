
#include "scaleform.h"

#include "externals.h"
#include "settings.h"
#include "hhs.h"
#include "cache.h"
#include "ingame.h"

namespace Scaleform {

	template<typename RetT>
	[[nodiscard]] RetT GetValue(GFxFunctionHandler::Args* args, std::uint32_t index) noexcept
	{
		if (!args) {
			return RetT{};
		}

		if (index >= args->numArgs) {
			return RetT{};
		}

		auto& arg = args->args[index];

		switch (arg.GetType()) {

		case GFxValue::kType_Bool:
			return arg.GetBool();

		case GFxValue::kType_Int: 
			return arg.GetInt();

		case GFxValue::kType_Number: 
			return arg.GetNumber();
		}

		return RetT{};
	}

	void EnablePlayer(GFxFunctionHandler::Args* args) noexcept
	{
		auto enabled = GetValue<bool>(args, 1);

		auto& settings = Settings::Ini::GetSingleton();

		if (enabled) {

			settings.SetEnablePlayer(enabled);
		}

		auto err = hhs::Map::GetSingleton().visit(hhs::VisitFlags::None, PlayerID, [&](hhs::System& sys) {

			if (enabled) {

				return sys.SetHeight();
			}

			auto ret = sys.ResetHeight();

			settings.SetEnablePlayer(enabled);

			return ret;	
		});
	}

	void EnableNPCs(GFxFunctionHandler::Args* args) noexcept
	{
		auto enabled = GetValue<bool>(args, 1);

		auto& settings = Settings::Ini::GetSingleton();

		if (enabled) {

			settings.SetEnableNPCs(enabled);
		}

		VisitCell([&](TESObjectREFR* refr) {

			hhs::Map::GetSingleton().visit(hhs::VisitFlags::None, refr, [&](hhs::System& sys) {

				if (sys.GetActorUtil().IsPlayer())
					return hhs::Error::Success;

				if (enabled) {

					return sys.SetHeight();
				}

				auto ret = sys.ResetHeight();

				settings.SetEnableNPCs(enabled);

				return ret;
			});
		});
	}

	void EnableScript(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableScript(GetValue<bool>(args, 1));
	}

	void EnableTextFile(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableTextFile(GetValue<bool>(args, 1));
	}

	void EnableJsonFile(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableTextFile(GetValue<bool>(args, 1));
	}

	void EnableExtraData(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableExtraData(GetValue<bool>(args, 1));
	}

	void EnableCamera() noexcept
	{
		hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, PlayerID, [&](hhs::System& sys) {

			auto h = sys.GetHeight();

			Camera::Player::GetSingleton().SetCameraHeight(sys.GetActorPtr(), h);

			return hhs::Error::Success;
		});
	}

	void EnableDynamicCamera(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableDynamicCamera(GetValue<bool>(args, 1));

		EnableCamera();
	}

	void EnableFirstPersonCamera(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnable1stCamera(GetValue<bool>(args, 1));

		EnableCamera();
	}

	void EnableThirdPersonCamera(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnable3rdCamera(GetValue<bool>(args, 1));

		EnableCamera();
	}

	void EnableCustomCameraPatch(GFxFunctionHandler::Args* args) noexcept
	{
		auto value = GetValue<bool>(args, 1);

		Settings::Ini::GetSingleton().SetEnableCustomCameraPatch(value);

		if (!value) {
			Camera::Player::GetSingleton().ResetCameraSettings();
		}

		EnableCamera();
	}

	void EnableCache(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetCache(GetValue<bool>(args, 1));

		Cache::Map::GetSingleton().Load();
	}

	void EnableAltRead(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetAltRead(GetValue<bool>(args, 1));
	}

	void EnableAAF(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableAAF(GetValue<bool>(args, 1));
	}

	void EnableTagAAF(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableTagAAF(GetValue<bool>(args, 1));
	}

	void EnableLooksMenu(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetLooksmenu(GetValue<bool>(args, 1));
	}

	void SetGender(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetGender(Settings::gender_cast(GetValue<std::int32_t>(args, 1)));
	}

	void SetBehaviorFurniture(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetBehaviorFurniture(Settings::furniture_cast(GetValue<std::int32_t>(args, 1)));
	}

	void EnableSlot(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableSlot(GetValue<std::uint32_t>(args, 1), GetValue<bool>(args, 2));
	}

	void SetRace(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetRace(Settings::race_cast(GetValue<std::int32_t>(args, 1)));
	}

	void SetHeight(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetHeight(GetValue<float>(args, 1));
	}

	void SetStep(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetStep(GetValue<float>(args, 1));
	}

	void SetSlot(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetSlot(GetValue<std::uint32_t>(args, 1));
	}

	void SetDirF4SE(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetDirF4SE(GetValue<bool>(args, 1));
	}

	void SetReference(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetReference(Settings::reference_cast(GetValue<std::int32_t>(args, 1)));
	}

	void SetSwimming(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableSwimming(GetValue<bool>(args, 1));
	}

	void SetBleedOut(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableBleedOut(GetValue<bool>(args, 1));
	}

	void SetFirstPersonAnim(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableFirstPersonAnim(GetValue<bool>(args, 1));
	}

	void SetFixes(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableFixes(GetValue<bool>(args, 1));
	}

	void SetReloadFixes(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableReloadFixes(GetValue<bool>(args, 1));
	}

	using SettingFn = void (*)(GFxFunctionHandler::Args*);

	const std::vector<std::pair<std::string, SettingFn>> vFuncs{
		{ "bEnablePlayer", EnablePlayer },
		{ "bEnableNPCs", EnableNPCs },
		{ "bEnableScript", EnableScript },
		{ "bEnableTextFile", EnableTextFile },
		{ "bEnableJsonFile", EnableJsonFile },
		{ "bEnableExtraData", EnableExtraData },
		{ "bEnableDynamicCamera", EnableDynamicCamera },
		{ "bEnable1stCamera", EnableFirstPersonCamera },
		{ "bEnable3rdCamera", EnableThirdPersonCamera },
		{ "bEnableCustomCameraPatch", EnableCustomCameraPatch },
		{ "bCache", EnableCache },
		{ "bAltRead", EnableAltRead },
		{ "bEnableAAF", EnableAAF },
		{ "bEnableTagAAF", EnableTagAAF },
		{ "bLooksmenu", EnableLooksMenu },
		{ "iGender", SetGender },
		{ "iBehaviorFurniture", SetBehaviorFurniture },
		{ "bEnableSlot", EnableSlot },
		{ "iRace", SetRace },
		{ "fHeight", SetHeight },
		{ "fstep", SetStep },
		{ "iSlot", SetSlot },
		{ "iDirF4SE", SetDirF4SE },
		{ "iReference", SetReference },
		{ "bEnableSwimming", SetSwimming },
		{ "bEnableBleedOut", SetBleedOut },
		{ "bEnableFirstPersonAnim", SetFirstPersonAnim },
		{ "bEnableFixes", SetFixes },
		{ "bEnableReloadFixes", SetReloadFixes }
	};

	void CallFnSettings(const std::string& name, GFxFunctionHandler::Args* args) noexcept
	{
		for (auto& fn : vFuncs) {

			if (name == fn.first) {

				fn.second(args);

				break;
			}
		}
	}

	void SetSetting_HHS::Invoke(Args* args)
	{
		CallFnSettings(args->args[0].GetString(), args);
	}

	void TestHeightInGame_HHS::Invoke(Args* args)
	{
#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		InGame::HeightEdit::GetSingleton().Test();
#endif
	}

	void CreateTxtInGame_HHS::Invoke(Args* args)
	{
#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		InGame::HeightEdit::GetSingleton().Create();
#endif
	}

	void DeleteTxtInGame_HHS::Invoke(Args* args)
	{
#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		InGame::HeightEdit::GetSingleton().Delete();
#endif
	}

	bool Register(GFxMovieView* view, GFxValue* value) noexcept
	{
		if (!view || !value) {
			return false;
		}

#define REG_FUNC(cls) RegisterFunction<cls>(value, view->movieRoot, # cls);

		REG_FUNC(SetSetting_HHS);
		REG_FUNC(TestHeightInGame_HHS);
		REG_FUNC(CreateTxtInGame_HHS);
		REG_FUNC(DeleteTxtInGame_HHS);

		return true;
	}
}
