
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
		static_assert(std::is_same_v<RetT, bool> ||
			std::is_same_v<RetT, std::uint32_t> ||
			std::is_same_v<RetT, std::int32_t> ||
			std::is_same_v<RetT, float> ||
			std::is_same_v<RetT, std::string_view>, 
			"Unsupported return type for GetValue");

		if (!args || index >= args->numArgs) {
			return RetT{};
		}

		auto& arg = args->args[index];

		if constexpr (std::is_same_v<RetT, bool>) {
			if (arg.GetType() == GFxValue::kType_Bool) {
				return arg.GetBool();
			}
		}
		else if constexpr (std::is_same_v<RetT, std::uint32_t> || std::is_same_v<RetT, std::int32_t>) {
			if (arg.GetType() == GFxValue::kType_Int) {
				return static_cast<RetT>(arg.GetInt());
			}
		}
		else if constexpr (std::is_same_v<RetT, float>) {
			if (arg.GetType() == GFxValue::kType_Number) {
				return arg.GetNumber();
			}
		}
		else if constexpr (std::is_same_v<RetT, std::string_view>) {
			if (arg.GetType() == GFxValue::kType_String) {
				return arg.GetString();
			}
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
		}, hhs::UpdateFlags::Init);
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
			}, hhs::UpdateFlags::Init);
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
		Settings::Ini::GetSingleton().SetEnableJsonFile(GetValue<bool>(args, 1));
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

	void EnableTerminalMenu(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetTerminal(GetValue<bool>(args, 1));
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
		Settings::Ini::GetSingleton().SetBipedSlot(GetValue<std::uint32_t>(args, 1), GetValue<bool>(args, 2));
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

	void EnableSwimming(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableSwimming(GetValue<bool>(args, 1));
	}

	void EnableBleedOut(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableBleedOut(GetValue<bool>(args, 1));
	}

	void EnableFirstPersonAnim(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableFirstPersonAnim(GetValue<bool>(args, 1));
	}

	void EnableFixes(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableFixes(GetValue<bool>(args, 1));
	}

	void EnableReloadFixes(GFxFunctionHandler::Args* args) noexcept
	{
		Settings::Ini::GetSingleton().SetEnableReloadFixes(GetValue<bool>(args, 1));
	}

	using SettingFn = void (*)(GFxFunctionHandler::Args*);
	using PairSettingFn = std::pair<std::string_view, SettingFn>;

	std::array<PairSettingFn, 30> vFuncs{
		PairSettingFn{ "bAltRead", EnableAltRead },
		PairSettingFn{ "bCache", EnableCache },
		PairSettingFn{ "bEnable1stCamera", EnableFirstPersonCamera },
		PairSettingFn{ "bEnable3rdCamera", EnableThirdPersonCamera },
		PairSettingFn{ "bEnableAAF", EnableAAF },
		PairSettingFn{ "bEnableBleedOut", EnableBleedOut },
		PairSettingFn{ "bEnableCustomCameraPatch", EnableCustomCameraPatch },
		PairSettingFn{ "bEnableDynamicCamera", EnableDynamicCamera },
		PairSettingFn{ "bEnableExtraData", EnableExtraData },
		PairSettingFn{ "bEnableFirstPersonAnim", EnableFirstPersonAnim },
		PairSettingFn{ "bEnableFixes", EnableFixes },
		PairSettingFn{ "bEnableJsonFile", EnableJsonFile },
		PairSettingFn{ "bEnableNPCs", EnableNPCs },
		PairSettingFn{ "bEnablePlayer", EnablePlayer },
		PairSettingFn{ "bEnableReloadFixes", EnableReloadFixes },
		PairSettingFn{ "bEnableScript", EnableScript },
		PairSettingFn{ "bEnableSlot", EnableSlot },
		PairSettingFn{ "bEnableSwimming", EnableSwimming },
		PairSettingFn{ "bEnableTagAAF", EnableTagAAF },
		PairSettingFn{ "bEnableTextFile", EnableTextFile },
		PairSettingFn{ "bLooksmenu", EnableLooksMenu },
		PairSettingFn{ "bTerminal", EnableTerminalMenu },
		PairSettingFn{ "fHeight", SetHeight },
		PairSettingFn{ "fstep", SetStep },
		PairSettingFn{ "iBehaviorFurniture", SetBehaviorFurniture },
		PairSettingFn{ "iDirF4SE", SetDirF4SE },
		PairSettingFn{ "iGender", SetGender },
		PairSettingFn{ "iRace", SetRace },
		PairSettingFn{ "iReference", SetReference },
		PairSettingFn{ "iSlot", SetSlot }
	};

	void CallFnSettings(std::string_view name, GFxFunctionHandler::Args* args) noexcept
	{
		auto it = std::lower_bound(vFuncs.begin(), vFuncs.end(), name, [](const PairSettingFn& pair, std::string_view value) {
			return _strcmpi(pair.first.data(), value.data()) < 0;
		});

		if (it != vFuncs.end() && _strcmpi(it->first.data(), name.data()) == 0) {
			it->second(args);
		}
	}

	void SetSetting_HHS::Invoke(Args* args)
	{
		CallFnSettings(GetValue<std::string_view>(args, 0), args);
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

	void SortSettings() noexcept
	{
		static bool sorted{};

		if (sorted) {
			return;
		}

		std::sort(vFuncs.begin(), vFuncs.end(), [](const PairSettingFn& left, const PairSettingFn& right) {
			return _strcmpi(left.first.data(), right.first.data()) < 0;
		});

		sorted = true;
	}

	bool Register(GFxMovieView* view, GFxValue* value) noexcept
	{
		if (!view || !value) {
			return false;
		}

		SortSettings();

#define REG_FUNC(cls) RegisterFunction<cls>(value, view->movieRoot, # cls);

		REG_FUNC(SetSetting_HHS);
		REG_FUNC(TestHeightInGame_HHS);
		REG_FUNC(CreateTxtInGame_HHS);
		REG_FUNC(DeleteTxtInGame_HHS);

		return true;
	}
}
