
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
		if (!args)
			return RetT{};

		if (index >= args->numArgs)
			return RetT{};

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

		auto& settings = Settings::Ini::GetInstance();

		if (enabled) {

			settings.Set_bEnablePlayer(enabled);
		}

		auto err = hhs::Map::GetInstance().visit(false, PlayerID, [&](hhs::System& sys) {

			if (enabled) {

				return sys.SetHeight();
			}

			auto ret = sys.ResetHeight();

			settings.Set_bEnablePlayer(enabled);

			return ret;	
		});
	}

	void EnableNPCs(GFxFunctionHandler::Args* args) noexcept
	{
		auto enabled = GetValue<bool>(args, 1);

		auto& settings = Settings::Ini::GetInstance();

		if (enabled) {

			settings.Set_bEnableNPCs(enabled);
		}

		VisitCell([&](TESObjectREFR* refr) {

			hhs::Map::GetInstance().visit(false, refr, [&](hhs::System& sys) {

				if (sys.GetActorUtil().IsPlayer())
					return hhs::Error::Success;

				if (enabled) {

					return sys.SetHeight();
				}

				auto ret = sys.ResetHeight();

				settings.Set_bEnableNPCs(enabled);

				return ret;
			});
		});
	}

	void EnableScript(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bEnableScript(GetValue<bool>(args, 1));
	}

	void EnableTextFile(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bEnableTextFile(GetValue<bool>(args, 1));
	}

	void EnableJsonFile(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bEnableTextFile(GetValue<bool>(args, 1));
	}

	void EnableExtraData(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bEnableExtraData(GetValue<bool>(args, 1));
	}

	void EnableCamera() noexcept
	{
		hhs::Map::GetInstance().visit(true, PlayerID, [&](hhs::System& sys) {

			auto h = sys.GetHeight();

			Camera::Player::GetInstance().SetCameraHeight(sys.GetActorPtr(), h);

			return hhs::Error::Success;
		});
	}

	void EnableDynamicCamera(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bEnableDynamicCamera(GetValue<bool>(args, 1));

		EnableCamera();
	}

	void EnableFirstPersonCamera(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bEnable1stCamera(GetValue<bool>(args, 1));

		EnableCamera();
	}

	void EnableThirdPersonCamera(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bEnable3rdCamera(GetValue<bool>(args, 1));

		EnableCamera();
	}

	void EnableCustomCameraPatch(GFxFunctionHandler::Args* args) noexcept
	{
		auto value = GetValue<bool>(args, 1);

		Settings::Ini::GetInstance().Set_bEnableCustomCameraPatch(value);

		if (!value)
			Camera::Player::GetInstance().ResetCameraSettings();

		EnableCamera();
	}

	void EnableCache(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bCache(GetValue<bool>(args, 1));

		Cache::Map::GetInstance().Load();
	}

	void EnableAltRead(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bAltRead(GetValue<bool>(args, 1));
	}

	void EnableAAF(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bEnableAAF(GetValue<bool>(args, 1));
	}

	void EnableTagAAF(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bEnableTagAAF(GetValue<bool>(args, 1));
	}

	void EnableLooksMenu(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bLooksmenu(GetValue<bool>(args, 1));
	}

	void SetGender(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_iGender(Settings::gender_cast(GetValue<std::int32_t>(args, 1)));
	}

	void SetBehaviorFurniture(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_iBehaviorFurniture(Settings::furniture_cast(GetValue<std::int32_t>(args, 1)));
	}

	void EnableSlot(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bEnableSlot(GetValue<std::uint32_t>(args, 1), GetValue<bool>(args, 2));
	}

	void SetRace(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_iRace(Settings::race_cast(GetValue<std::int32_t>(args, 1)));
	}

	void SetHeight(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_fHeight(GetValue<float>(args, 1));
	}

	void SetStep(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_fStep(GetValue<float>(args, 1));
	}

	void SetSlot(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_iSlot(GetValue<std::uint32_t>(args, 1));
	}

	void SetDirF4SE(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_iDirF4SE(GetValue<bool>(args, 1));
	}

	void SetReference(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_iReference(Settings::reference_cast(GetValue<std::int32_t>(args, 1)));
	}

	void SetSwimming(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bEnableSwimming(GetValue<bool>(args, 1));
	}

	void SetFirstPersonAnim(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bEnableFirstPersonAnim(GetValue<bool>(args, 1));
	}

	void SetFixes(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bEnableFixes(GetValue<bool>(args, 1));
	}

	void SetReloadFixes(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetInstance().Set_bEnableReloadFixes(GetValue<bool>(args, 1));
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
		InGame::HeightEdit::GetInstance().Test();
#endif
	}

	void CreateTxtInGame_HHS::Invoke(Args* args)
	{
#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		InGame::HeightEdit::GetInstance().Create();
#endif
	}

	void DeleteTxtInGame_HHS::Invoke(Args* args)
	{
#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		InGame::HeightEdit::GetInstance().Delete();
#endif
	}

	bool Register(GFxMovieView* view, GFxValue* value) noexcept
	{
		if (!view || !value)
			return false;

#define REG_FUNC(cls) RegisterFunction<cls>(value, view->movieRoot, # cls);

		REG_FUNC(SetSetting_HHS);
		REG_FUNC(TestHeightInGame_HHS);
		REG_FUNC(CreateTxtInGame_HHS);
		REG_FUNC(DeleteTxtInGame_HHS);

		return true;
	}
}
