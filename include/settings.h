#pragma once

#include "version.h"
#include "externals.h"
#include "skeleton.h"
#include "node.h"

#include "SimpleIni.h"

namespace Settings {

	enum class Gender : std::uint32_t {
		
		BothGender,
		Female,
		Male
	};

	enum class Furniture : std::uint32_t {
		
		Disabled,
		Keywords,
		Enable
	};

	enum class Race : std::uint32_t {
		
		AllRaces,
		FromEditorID,
		FromSkeleton,
		FromIDAndSkeleton
	};

	enum class Reference : std::uint32_t {

		Player,
		CrossHair
	};

	constexpr char* Default_Settings = "Data\\MCM\\Config\\FO4HHS\\Settings.ini";
	constexpr char* Custom_Settings = "Data\\MCM\\Settings\\FO4HHS.ini";

	constexpr bool Default_EnablePlayer = true;
	constexpr bool Default_EnableNPC = true;
	constexpr bool Default_EnableScript = true;
	constexpr bool Default_EnableTextFile = true;
	constexpr bool Default_EnableJsonFile = true;
	constexpr bool Default_EnableExtraData = true;
	constexpr bool Default_EnableDynamicCamera = false;
	constexpr bool Default_Enable1stCamera = true;
	constexpr bool Default_Enable3rdCamera = true;
	constexpr bool Default_Cache = false;
	constexpr bool Default_AltRead = false;
	constexpr bool Default_EnableAAF = false;
	constexpr bool Default_EnableTagAAF = true;
	constexpr bool Default_Looksmenu = true;
	constexpr bool Default_EnableSwimming = true;
	constexpr bool Default_EnableFirstPersonAnim = true;
	constexpr bool Default_EnableFixes = true;
	constexpr bool Default_EnableReloadFixes = false;
	constexpr Gender Default_Gender = Gender::BothGender;
	constexpr Furniture Default_BehaviorFurniture = Furniture::Keywords;
	constexpr Race Default_Race = Race::FromIDAndSkeleton;
	constexpr float Default_Height = 10.0f;
	constexpr float Default_Step = 0.1f;
	constexpr std::uint32_t Default_Slot = 3;
	constexpr bool Default_DirF4SE = false;
	constexpr Reference Default_Reference = Reference::CrossHair;
	constexpr std::uint32_t Default_ReadBufferLen = 131072;

	template<typename T>
	inline [[nodiscard]] Gender gender_cast(const T value) noexcept {
		return std::clamp(static_cast<Gender>(value), Gender::BothGender, Gender::Male);
	}

	template<typename T>
	inline [[nodiscard]] Furniture furniture_cast(const T value) noexcept {
		return std::clamp(static_cast<Furniture>(value), Furniture::Disabled, Furniture::Enable);
	}

	template<typename T>
	inline [[nodiscard]] Race race_cast(const T value) noexcept	{
		return std::clamp(static_cast<Race>(value), Race::AllRaces, Race::FromIDAndSkeleton);
	}

	template<typename T>
	inline [[nodiscard]] Reference reference_cast(const T value) noexcept {
		return std::clamp(static_cast<Reference>(value), Reference::Player, Reference::CrossHair);
	}

	class Ini {

	public:

		using Slots = std::vector<bool>;

		[[nodiscard]] static Ini& GetInstance() noexcept { return instance; }

		[[nodiscard]] const Slots& GetSlots() const { return bEnableSlot; }

#define DECL_FN_GET_SET(t, v) \
		[[nodiscard]] const t& Get_ ## v () const noexcept { return v; } \
		void Set_ ## v (const t value) noexcept { v = value; } 
		
		DECL_FN_GET_SET(bool, bEnablePlayer);
		DECL_FN_GET_SET(bool, bEnableNPCs);

		DECL_FN_GET_SET(bool, bEnableScript);
		DECL_FN_GET_SET(bool, bEnableTextFile);
		DECL_FN_GET_SET(bool, bEnableJsonFile);
		DECL_FN_GET_SET(bool, bEnableExtraData);

		DECL_FN_GET_SET(bool, bEnableDynamicCamera);
		DECL_FN_GET_SET(bool, bEnable1stCamera);
		DECL_FN_GET_SET(bool, bEnable3rdCamera);

		DECL_FN_GET_SET(bool, bCache);

		DECL_FN_GET_SET(bool, bAltRead);
		DECL_FN_GET_SET(std::uint32_t, iReadBufferLen);

		DECL_FN_GET_SET(bool, bEnableAAF);
		DECL_FN_GET_SET(bool, bEnableTagAAF);

		DECL_FN_GET_SET(bool, bLooksmenu);

		DECL_FN_GET_SET(bool, bEnableSwimming);
		DECL_FN_GET_SET(bool, bEnableFirstPersonAnim);

		DECL_FN_GET_SET(bool, bEnableFixes);
		DECL_FN_GET_SET(bool, bEnableReloadFixes);

		DECL_FN_GET_SET(Gender, iGender);

		DECL_FN_GET_SET(Furniture, iBehaviorFurniture);

		void Set_bEnableSlot(const std::uint32_t& index, const bool& value) noexcept;
		[[nodiscard]] const Slots& Get_bEnableSlot() noexcept { return bEnableSlot; }
		void SetSlotFlags() noexcept;

		DECL_FN_GET_SET(std::uint32_t, uSlotFlags);

		DECL_FN_GET_SET(std::uint32_t, iKeyStartStopPlayer);
		DECL_FN_GET_SET(std::uint32_t, iKeyStartStopAll);

		DECL_FN_GET_SET(std::uint32_t, iKeyActivateEdit);
		DECL_FN_GET_SET(std::uint32_t, iKeyChangeRefr);
		DECL_FN_GET_SET(std::uint32_t, iKeyCreateHeight);
		DECL_FN_GET_SET(std::uint32_t, iKeyDeleteHeight);
		DECL_FN_GET_SET(std::uint32_t, iKeyIncrementHeight);
		DECL_FN_GET_SET(std::uint32_t, iKeyDecrementHeight);

		DECL_FN_GET_SET(Race, iRace);

		DECL_FN_GET_SET(float, fHeight);
		DECL_FN_GET_SET(float, fStep);
		DECL_FN_GET_SET(Reference, iReference);

		[[nodiscard]] const std::uint32_t& Get_iSlot() const noexcept { return iSlot; }
		void Set_iSlot(const std::uint32_t value) noexcept { iSlot = std::clamp(value, MinSlot, MaxSlot); }

		DECL_FN_GET_SET(bool, iDirF4SE);

		void ReadAllSettings() noexcept;

		void InitFurnitureKeywords(CSimpleIni& ini) noexcept;
		void InitRace(CSimpleIni& ini) noexcept;

		[[nodiscard]] bool CheckFurnitureBehavior(TESObjectREFR* refr) noexcept;
		[[nodiscard]] bool CheckRace(TESRace* race) noexcept;
		[[nodiscard]] bool CheckTagAAF(const std::string& tag) noexcept;

	private:

		Ini() noexcept { bEnableSlot.assign(MaxSlot, true); }
		~Ini() noexcept {}

		Ini(const Ini&) = delete;
		Ini(Ini&&) = delete;

		Ini& operator=(const Ini&) = delete;
		Ini& operator=(Ini&&) = delete;

		void ReadSettings(const std::string& Filename) noexcept;

		// [Main]
		bool bEnablePlayer{ Default_EnablePlayer };
		bool bEnableNPCs{ Default_EnableNPC };

		bool bEnableScript{ Default_EnableScript };
		bool bEnableTextFile{ Default_EnableTextFile };
		bool bEnableJsonFile{ Default_EnableJsonFile };
		bool bEnableExtraData{ Default_EnableExtraData };

		bool bEnableDynamicCamera{ Default_EnableDynamicCamera };
		bool bEnable1stCamera{ Default_Enable1stCamera };
		bool bEnable3rdCamera{ Default_Enable3rdCamera };

		bool bCache{ Default_Cache };

		bool bAltRead{ Default_AltRead };
		std::uint32_t iReadBufferLen{ Default_ReadBufferLen };

		bool bEnableAAF{ Default_EnableAAF };
		bool bEnableTagAAF{ Default_EnableTagAAF };
		std::vector<std::string> vTagAAF;

		bool bLooksmenu{ Default_Looksmenu };

		Gender iGender{ Default_Gender };

		Furniture iBehaviorFurniture{ Default_BehaviorFurniture };

		std::vector<std::uint32_t> vFurnitureKeyword;

		// [AnimEvent]
		bool bEnableSwimming{ Default_EnableSwimming };
		bool bEnableFirstPersonAnim{ Default_EnableFirstPersonAnim };
		bool bEnableFixes{ Default_EnableFixes };
		bool bEnableReloadFixes{ Default_EnableReloadFixes };

		// [Slot]
		Slots bEnableSlot;
		std::uint32_t uSlotFlags{ 0xffffffff };

		// [Race]
		Race iRace{ Default_Race };
		std::vector<std::uint32_t> vRace;

		// [In-Game]
		float fHeight{ Default_Height };
		float fStep{ 0.1f };
		std::uint32_t iSlot{ Default_Slot };
		bool iDirF4SE{ Default_DirF4SE };
		Reference iReference{ Default_Reference };

		// [Keys]
		std::uint32_t iKeyStartStopPlayer{};
		std::uint32_t iKeyStartStopAll{};
		std::uint32_t iKeyActivateEdit{};
		std::uint32_t iKeyChangeRefr{};
		std::uint32_t iKeyCreateHeight{};
		std::uint32_t iKeyDeleteHeight{};
		std::uint32_t iKeyIncrementHeight{};
		std::uint32_t iKeyDecrementHeight{};

		static Ini instance;
	};
}
