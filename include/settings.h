#pragma once

#include "version.h"
#include "externals.h"
#include "skeleton.h"
#include "node.h"
#include "util.h"

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

	enum class Section : std::uint32_t {

		Main,
		Slot,
		AnimEvent,
		Race,
		InGame,
		Keys
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
	constexpr bool Default_EnableCustomCameraPatch = true;
	constexpr bool Default_Cache = false;
	constexpr bool Default_AltRead = false;
	constexpr bool Default_EnableAAF = false;
	constexpr bool Default_EnableTagAAF = true;
	constexpr bool Default_Looksmenu = true;
	constexpr bool Default_Terminal = true;
	constexpr bool Default_EnableSwimming = true;
	constexpr bool Default_EnableBleedOut = true;
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
	inline [[nodiscard]] Gender gender_cast(T value) noexcept {
		return std::clamp(static_cast<Gender>(value), Gender::BothGender, Gender::Male);
	}

	template<typename T>
	inline [[nodiscard]] Furniture furniture_cast(T value) noexcept {
		return std::clamp(static_cast<Furniture>(value), Furniture::Disabled, Furniture::Enable);
	}

	template<typename T>
	inline [[nodiscard]] Race race_cast(T value) noexcept	{
		return std::clamp(static_cast<Race>(value), Race::AllRaces, Race::FromIDAndSkeleton);
	}

	template<typename T>
	inline [[nodiscard]] Reference reference_cast(T value) noexcept {
		return std::clamp(static_cast<Reference>(value), Reference::Player, Reference::CrossHair);
	}

	class Ini :
		public util::Singleton<Ini> {
		friend class util::Singleton<Ini>;

	public:

		[[nodiscard]] bool GetEnablePlayer() const noexcept { return bEnablePlayer; }
		[[nodiscard]] bool GetEnableNPCs() const noexcept { return bEnableNPCs; }
		[[nodiscard]] bool GetEnableScript() const noexcept { return bEnableScript; }
		[[nodiscard]] bool GetEnableTextFile() const noexcept { return bEnableTextFile; }
		[[nodiscard]] bool GetEnableJsonFile() const noexcept { return bEnableJsonFile; }
		[[nodiscard]] bool GetEnableExtraData() const noexcept { return bEnableExtraData; }
		[[nodiscard]] bool GetEnableDynamicCamera() const noexcept { return bEnableDynamicCamera; }
		[[nodiscard]] bool GetEnable1stCamera() const noexcept { return bEnable1stCamera; }
		[[nodiscard]] bool GetEnable3rdCamera() const noexcept { return bEnable3rdCamera; }
		[[nodiscard]] bool GetEnableCustomCameraPatch() const noexcept { return bEnableCustomCameraPatch; }
		[[nodiscard]] bool GetCache() const noexcept { return bCache; }
		[[nodiscard]] bool GetAltRead() const noexcept { return bAltRead; }
		[[nodiscard]] std::uint32_t GetReadBufferLen() const noexcept { return iReadBufferLen; }
		[[nodiscard]] bool GetEnableAAF() const noexcept { return bEnableAAF; }
		[[nodiscard]] bool GetEnableTagAAF() const noexcept { return bEnableTagAAF; }
		[[nodiscard]] bool GetLooksmenu() const noexcept { return bLooksmenu; }
		[[nodiscard]] bool GetTerminal() const noexcept { return bTerminal; }
		[[nodiscard]] Gender GetGender() const noexcept { return iGender; }
		[[nodiscard]] Furniture GetBehaviorFurniture() const noexcept { return iBehaviorFurniture; }
		[[nodiscard]] std::uint32_t GetSlotFlags() const noexcept { return iSlotFlags; }
		[[nodiscard]] Race GetRace() const noexcept { return iRace; }
		[[nodiscard]] bool GetEnableSwimming() const noexcept { return bEnableSwimming; }
		[[nodiscard]] bool GetEnableBleedOut() const noexcept { return bEnableBleedOut; }
		[[nodiscard]] bool GetEnableFirstPersonAnim() const noexcept { return bEnableFirstPersonAnim; }
		[[nodiscard]] bool GetEnableFixes() const noexcept { return bEnableFixes; }
		[[nodiscard]] bool GetEnableReloadFixes() const noexcept { return bEnableReloadFixes; }
		[[nodiscard]] float GetHeight() const noexcept { return fHeight; }
		[[nodiscard]] float GetStep() const noexcept { return fStep; }
		[[nodiscard]] std::uint32_t GetSlot() const noexcept { return iSlot; }
		[[nodiscard]] bool GetDirF4SE() const noexcept { return iDirF4SE; }
		[[nodiscard]] Reference GetReference() const noexcept { return iReference; }
		[[nodiscard]] std::uint32_t GetKeyStartStopPlayer() const noexcept { return iKeyStartStopPlayer; }
		[[nodiscard]] std::uint32_t GetKeyStartStopAll() const noexcept { return iKeyStartStopAll; }
		[[nodiscard]] std::uint32_t GetKeyActivateEdit() const noexcept { return iKeyActivateEdit; }
		[[nodiscard]] std::uint32_t GetKeyChangeRefr() const noexcept { return iKeyChangeRefr; }
		[[nodiscard]] std::uint32_t GetKeyCreateHeight() const noexcept { return iKeyCreateHeight; }
		[[nodiscard]] std::uint32_t GetKeyDeleteHeight() const noexcept { return iKeyDeleteHeight; }
		[[nodiscard]] std::uint32_t GetKeyIncrementHeight() const noexcept { return iKeyIncrementHeight; }
		[[nodiscard]] std::uint32_t GetKeyDecrementHeight() const noexcept { return iKeyDecrementHeight; }
		[[nodiscard]] std::uint32_t GetSlotAt(std::size_t index) const noexcept { return aSlot[index]; }
		[[nodiscard]] std::uint32_t GetCountSlot() const noexcept { return iCountSlot; }

		void SetEnablePlayer(bool value) noexcept { bEnablePlayer = value; }
		void SetEnableNPCs(bool value) noexcept { bEnableNPCs = value; }
		void SetEnableScript(bool value) noexcept { bEnableScript = value; }
		void SetEnableTextFile(bool value) noexcept { bEnableTextFile = value; }
		void SetEnableJsonFile(bool value) noexcept { bEnableJsonFile = value; }
		void SetEnableExtraData(bool value) noexcept { bEnableExtraData = value; }
		void SetEnableDynamicCamera(bool value) noexcept { bEnableDynamicCamera = value; }
		void SetEnable1stCamera(bool value) noexcept { bEnable1stCamera = value; }
		void SetEnable3rdCamera(bool value) noexcept { bEnable3rdCamera = value; }
		void SetEnableCustomCameraPatch(bool value) noexcept { bEnableCustomCameraPatch = value; }
		void SetCache(bool value) noexcept { bCache = value; }
		void SetAltRead(bool value) noexcept { bAltRead = value; }
		void SetReadBufferLen(std::uint32_t value) noexcept { iReadBufferLen = value; }
		void SetEnableAAF(bool value) noexcept { bEnableAAF = value; }
		void SetEnableTagAAF(bool value) noexcept { bEnableTagAAF = value; }
		void SetLooksmenu(bool value) noexcept { bLooksmenu = value; }
		void SetTerminal(bool value) noexcept { bTerminal = value; }
		void SetGender(Gender value) noexcept { iGender = value; }
		void SetBehaviorFurniture(Furniture value) noexcept { iBehaviorFurniture = value; }
		void SetRace(Race value) noexcept { iRace = value; }
		void SetEnableSwimming(bool value) noexcept { bEnableSwimming = value; }
		void SetEnableBleedOut(bool value) noexcept { bEnableBleedOut = value; }
		void SetEnableFirstPersonAnim(bool value) noexcept { bEnableFirstPersonAnim = value; }
		void SetEnableFixes(bool value) noexcept { bEnableFixes = value; }
		void SetEnableReloadFixes(bool value) noexcept { bEnableReloadFixes = value; }
		void SetHeight(float value) noexcept { fHeight = value; }
		void SetStep(float value) noexcept { fStep = value; }
		void SetSlot(std::uint32_t value) noexcept { iSlot = std::clamp(value, MinSlot, MaxSlot); }
		void SetDirF4SE(bool value) noexcept { iDirF4SE = value; }
		void SetReference(Reference value) noexcept { iReference = value; }
		void SetKeyStartStopPlayer(std::uint32_t value) noexcept { iKeyStartStopPlayer = value; }
		void SetKeyStartStopAll(std::uint32_t value) noexcept { iKeyStartStopAll = value; }
		void SetKeyActivateEdit(std::uint32_t value) noexcept { iKeyActivateEdit = value; }
		void SetKeyChangeRefr(std::uint32_t value) noexcept { iKeyChangeRefr = value; }
		void SetKeyCreateHeight(std::uint32_t value) noexcept { iKeyCreateHeight = value; }
		void SetKeyDeleteHeight(std::uint32_t value) noexcept { iKeyDeleteHeight = value; }
		void SetKeyIncrementHeight(std::uint32_t value) noexcept { iKeyIncrementHeight = value; }
		void SetKeyDecrementHeight(std::uint32_t value) noexcept { iKeyDecrementHeight = value; }
		void SetEnableSlot(std::uint32_t index, bool value) noexcept;

		void ReadAllSettings() noexcept;

		[[nodiscard]] bool CheckFurnitureBehavior(TESObjectREFR* refr) noexcept;
		[[nodiscard]] bool CheckRace(TESRace* race) noexcept;
		[[nodiscard]] bool CheckTagAAF(const std::string& tag) noexcept;

	private:

		Ini() noexcept = default;
		~Ini() noexcept = default;

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
		bool bEnableCustomCameraPatch{ Default_EnableCustomCameraPatch };
		bool bCache{ Default_Cache };
		bool bAltRead{ Default_AltRead };
		std::uint32_t iReadBufferLen{ Default_ReadBufferLen };
		bool bEnableAAF{ Default_EnableAAF };
		bool bEnableTagAAF{ Default_EnableTagAAF };
		std::vector<std::string> vTagAAF;
		bool bLooksmenu{ Default_Looksmenu };
		bool bTerminal{ Default_Terminal };
		Gender iGender{ Default_Gender };
		Furniture iBehaviorFurniture{ Default_BehaviorFurniture };
		std::vector<std::uint32_t> vFurnitureKeyword;

		// [Slot]
		bool bEnableSlot[MaxSlot];
		std::uint32_t aSlot[MaxSlot];
		std::uint32_t iCountSlot{};
		std::uint32_t iSlotFlags{ 0xffffffff };

		// [Race]
		Race iRace{ Default_Race };
		std::vector<std::uint32_t> vRace;
		
		// [AnimEvent]
		bool bEnableSwimming{ Default_EnableSwimming };
		bool bEnableBleedOut{ Default_EnableBleedOut };
		bool bEnableFirstPersonAnim{ Default_EnableFirstPersonAnim };
		bool bEnableFixes{ Default_EnableFixes };
		bool bEnableReloadFixes{ Default_EnableReloadFixes };

		// [In-Game]
		float fHeight{ Default_Height };
		float fStep{ Default_Step };
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

		const std::array<std::string_view, 6> vSection{
			"Main",
			"Slot",
			"AnimEvent",
			"Race",
			"In-Game",
			"Keys"
		};

		void InitFurnitureKeywords(CSimpleIni& ini) noexcept;
		void InitRace(CSimpleIni& ini) noexcept;

		[[nodiscard]] bool HasRaceKeyword(TESRace* race) noexcept;

		void ParseString(const std::string& str, std::vector<std::string>& vStr) noexcept;
		[[nodiscard]] bool ParseString(CSimpleIni& ini, Section section, const char* key, std::vector<std::string>& vStr) noexcept;

		void DbgMessage(const char* type, const char* name, std::uint32_t id, const char* other_msg = "") noexcept
		{ _DMESSAGE("%s found : EditorID = %40s\t\t\tformID = 0x%.8X %s", type, name, id, other_msg); }

		void DMsg(const char* msg, float value) noexcept { _DMESSAGE("%s = %f", msg, value); }
		void DMsg(const char* msg, double value) noexcept { _DMESSAGE("%s = %f", msg, value); }
		void DMsg(const char* msg, bool value) noexcept { _DMESSAGE("%s = %i", msg, value); }
		void DMsg(const char* msg, int value) noexcept { _DMESSAGE("%s = %i", msg, value); }
		void DMsg(const char* msg, std::uint32_t value) noexcept { _DMESSAGE("%s = %u", msg, value); }
		void DMsg(const char* msg, Gender value) noexcept { _DMESSAGE("%s = %u", msg, value); }
		void DMsg(const char* msg, Furniture value) noexcept { _DMESSAGE("%s = %u", msg, value); }
		void DMsg(const char* msg, Race value) noexcept { _DMESSAGE("%s = %u", msg, value); }
		void DMsg(const char* msg, Reference value) noexcept { _DMESSAGE("%s = %u", msg, value); }

		void GetValue(CSimpleIni& ini, Section section, const char* key, bool& value) noexcept;
		void GetValue(CSimpleIni& ini, Section section, const char* key, float& value) noexcept;

		template<typename T>
		void GetValue(CSimpleIni& ini, Section section, const char* key, T& value) noexcept;
	};
}
