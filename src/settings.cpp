
#include "settings.h"

namespace Settings {

	enum class Section : std::uint32_t {

		Main,
		Slot,
		AnimEvent,
		Race,
		In_Game,
		Keys
	};

	const std::vector<std::string> Sections{
		"Main",
		"Slot",
		"AnimEvent",
		"Race",
		"In-Game",
		"Keys"
	};

	template<typename T, typename Func = std::function<T(const std::string&)>>
	inline [[nodiscard]] T GetValue(CSimpleIni& ini, const Section& section, const std::string& key, T def, Func fn) noexcept
	{
		return fn(ini.GetValue(Sections[static_cast<int>(section)].c_str(), key.c_str(), std::to_string(def).c_str()));
	}

	template<typename T>
	inline [[nodiscard]] SI_Error SetValue(CSimpleIni& ini, const Section& section, const std::string& key, T value) noexcept
	{
		return ini.SetValue(Sections[static_cast<int>(section)].c_str(), key.c_str(), std::to_string(value).c_str());
	}

	inline [[nodiscard]] SI_Error SetValue(CSimpleIni& ini, const Section& section, const std::string& key, std::string& value) noexcept
	{
		return ini.SetValue(Sections[static_cast<int>(section)].c_str(), key.c_str(), value.c_str());
	}

	inline [[nodiscard]] float stof(const std::string& str) noexcept { return std::stof(str); }

	inline [[nodiscard]] int stoi(const std::string& str) noexcept { return std::stoi(str); }

	inline [[nodiscard]] bool stob(const std::string& str) noexcept { return std::stoi(str); }

	inline [[nodiscard]] std::uint32_t stou(const std::string& str) noexcept { return std::stoul(str); }

	inline [[nodiscard]] float GetValue(CSimpleIni& ini, const Section& section, const std::string& key, float def) noexcept
	{
		return GetValue<>(ini, section, key, def, stof);
	}

	inline [[nodiscard]] bool GetValue(CSimpleIni& ini, const Section& section, const std::string& key, bool def) noexcept
	{
		return GetValue<>(ini, section, key, def, stob);
	}

	inline [[nodiscard]] int GetValue(CSimpleIni& ini, const Section& section, const std::string& key, int def) noexcept
	{
		return GetValue<>(ini, section, key, def, stoi);
	}

	inline [[nodiscard]] std::uint32_t GetValue(CSimpleIni& ini, const Section& section, const std::string& key, std::uint32_t def) noexcept
	{
		return GetValue<>(ini, section, key, def, stou);
	}

	inline [[nodiscard]] std::string GetValue(CSimpleIni& ini, const Section& section, const std::string& key, const char* def) noexcept
	{
		return ini.GetValue(Sections[static_cast<int>(section)].c_str(), key.c_str(), def);
	}

	inline [[nodiscard]] std::string GetValue(CSimpleIni& ini, const Section& section, const std::string& key, const std::string& def) noexcept
	{
		return ini.GetValue(Sections[static_cast<int>(section)].c_str(), key.c_str(), def.c_str());
	}

	inline [[nodiscard]] Gender GetValue(CSimpleIni& ini, const Section& section, const std::string& key, const Gender& def) noexcept
	{
		return gender_cast(GetValue(ini, section, key, static_cast<int>(def)));
	}

	inline [[nodiscard]] Furniture GetValue(CSimpleIni& ini, const Section& section, const std::string& key, const Furniture& def) noexcept
	{
		return furniture_cast(GetValue(ini, section, key, static_cast<int>(def)));
	}

	inline [[nodiscard]] Race GetValue(CSimpleIni& ini, const Section& section, const std::string& key, const Race& def) noexcept
	{
		return race_cast(GetValue(ini, section, key, static_cast<int>(def)));
	}

	inline [[nodiscard]] Reference GetValue(CSimpleIni& ini, const Section& section, const std::string& key, const Reference& def) noexcept
	{
		return reference_cast(GetValue(ini, section, key, static_cast<int>(def)));
	}

	void ParseString(const std::string& str, std::vector<std::string>& vStr) noexcept
	{
		std::size_t found{};
		std::string s{ str };

		vStr.clear();

		while (!s.empty()) {

			found = s.find(",");

			if (found != std::string::npos) {

				vStr.push_back(s.substr(0, found));
				s = s.substr(found + 1);
			}
			else {

				vStr.push_back(str);
				s.clear();
			}
		}

		if (!vStr.empty()) {

			sort(vStr);
			vStr.erase(unique(vStr), vStr.end());
		}
	}

	[[nodiscard]] bool ParseString(CSimpleIni& ini, const Section& section, const std::string& key, const std::string& def, std::vector<std::string>& vStr) noexcept
	{
		std::string str = GetValue(ini, section, key, def);

		if (str.empty())
			return false;

		ParseString(str, vStr);

		return true;
	}

	inline void DbgMessage(const std::string& type, const std::string& name, const std::uint32_t& id, const std::string& other_msg = "") noexcept
	{
		_DMESSAGE("%s found : EditorID = %40s\t\t\tformID = 0x%.8X %s", type.c_str(), name.c_str(), id, other_msg);
	}

	inline void DbgValue(const char* msg, const float& value) noexcept { _DMESSAGE("%s = %f", msg, value); }
	inline void DbgValue(const char* msg, const bool& value) noexcept { _DMESSAGE("%s = %i", msg, value); }
	inline void DbgValue(const char* msg, const int& value) noexcept { _DMESSAGE("%s = %i", msg, value); }
	inline void DbgValue(const char* msg, const std::uint32_t& value) noexcept { _DMESSAGE("%s = %u", msg, value); }
	inline void DbgValue(const char* msg, const Gender& value) noexcept { _DMESSAGE("%s = %u", msg, value); }
	inline void DbgValue(const char* msg, const Furniture& value) noexcept { _DMESSAGE("%s = %u", msg, value); }
	inline void DbgValue(const char* msg, const Race& value) noexcept { _DMESSAGE("%s = %u", msg, value); }
	inline void DbgValue(const char* msg, const Reference& value) noexcept { _DMESSAGE("%s = %u", msg, value); }

	void Ini::InitFurnitureKeywords(CSimpleIni& ini) noexcept
	{
		auto data = (*g_dataHandler);

		if (!data)
			return;

		std::vector<std::string> vStr;

		if (!ParseString(ini, Section::Main, "sFurnitureKeyword", "", vStr))
			return;

		std::uint32_t cnt{};

		vFurnitureKeyword.clear();

		_DMESSAGE("Loading keywords :");

		for (std::uint32_t iKey{}; iKey < data->arrKYWD.count; iKey++) {

			auto key = data->arrKYWD[iKey];

			if (!key)
				continue;

			for (const auto& s : vStr) {

				if (_strcmpi(s.c_str(), key->keyword.c_str()) == 0) {

					vFurnitureKeyword.push_back(key->formID);
					
					cnt++;
					
					DbgMessage("Keyword", key->keyword.c_str(), key->formID);
					
					break;
				}
			}
		}

		if (cnt == 0) {

			_DMESSAGE("Error init furniture Keywords!");
			
			return;
		}

		std::sort(vFurnitureKeyword);
		vFurnitureKeyword.erase(std::unique(vFurnitureKeyword), vFurnitureKeyword.end());
	}

	inline [[nodiscard]] bool operator&(const Race& a, const Race& b) noexcept
	{
		return (static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b)) != 0;
	}

	inline [[nodiscard]] bool HasKeywordRace(TESRace* race) noexcept
	{
		for (std::uint32_t i{}; i < race->keywordForm.numKeywords; i++) {

			auto keyRaceHHS = race->keywordForm.keywords[i];

			if (keyRaceHHS && _strcmpi("ActorType_HHS", keyRaceHHS->keyword.c_str()) == 0)
				return true;
		}

		return false;
	}

	void Ini::InitRace(CSimpleIni& ini) noexcept
	{
		auto data = (*g_dataHandler);

		if (!data || iRace == Race::AllRaces)
			return;

		std::vector<std::string> vStr;

		if (!ParseString(ini, Section::Race, "sRace", "", vStr))
			return;

		std::uint32_t cnt{};


		vRace.clear();

		_DMESSAGE("Loading races :");

		for (std::uint32_t idx{}; idx < data->arrRACE.count; idx++) {

			auto race = data->arrRACE[idx];

			if (!race)
				continue;

			if (iRace & Race::FromSkeleton) {

				for (std::uint32_t isFemale{}; isFemale < 2; isFemale++) {

					Skeleton::Reader skeleton{ race->models[isFemale].GetModelName() };

					if (!skeleton || !skeleton.HasNode(ComOverride))
						continue;

					vRace.push_back(race->formID);

					cnt++;

					DbgMessage("Race", race->editorId.c_str(), race->formID, "from Skeleton");

					break;
				}
			}

			if (iRace & Race::FromEditorID) {

				for (const auto& s : vStr) {

					bool hasKeyword{ HasKeywordRace(race) };

					if (_strcmpi(s.c_str(), race->editorId.c_str()) == 0 || hasKeyword) {

						vRace.push_back(race->formID);

						cnt++;

						DbgMessage("Race", race->editorId.c_str(), race->formID, "from " + std::string(hasKeyword ? "Keyword" : "Settings"));

						break;
					}
				}
			}
		}

		if (cnt == 0) {

			_DMESSAGE("Error init Races!");

			return;
		}

		std::sort(vRace);
		vRace.erase(std::unique(vRace), vRace.end());
	}

	void Ini::Set_bEnableSlot(const std::uint32_t& index, const bool& value) noexcept
	{
		if (index < MinSlot || index >= MaxSlot)
			return;

		bEnableSlot[index] = value;

		if (value) {

			uSlotFlags |= (1 << index);
		}
		else {

			uSlotFlags &= ~(1 << index);
		}
	}

	void Ini::SetSlotFlags() noexcept
	{
		for (std::uint32_t idx{ MinSlot }; idx < MaxSlot; idx++) {

			if (bEnableSlot[idx])
				uSlotFlags |= (1 << idx);
		}
	}

	bool Ini::CheckFurnitureBehavior(TESObjectREFR* refr) noexcept
	{
		TESForm* frm{};

		if (!refr || !(frm = refr->baseForm))
			return false;

		if (iBehaviorFurniture == Furniture::Disabled)
			return true;

		if (iBehaviorFurniture == Furniture::Keywords) {

			TESFurniture* furn{ nullptr };

			if (vFurnitureKeyword.empty() || !(furn = DYNAMIC_CAST(frm, TESForm, TESFurniture)))
				return false;

			for (std::uint32_t idx{}; idx < furn->keywordForm.numKeywords; idx++) {

				auto key = furn->keywordForm.keywords[idx];

				if (key && std::binary_search(vFurnitureKeyword, key->formID))
					return true;
			}
		}

		return false;
	}

	bool Ini::CheckRace(TESRace* race) noexcept
	{
		if (iRace == Race::AllRaces)
			return true;

		return std::binary_search(vRace, race->formID);
	}

	bool Ini::CheckTagAAF(const std::string& tag) noexcept
	{
		if (!bEnableTagAAF)
			return false;

		for (auto& sTag : vTagAAF) {

			if (sTag == tag)
				return true;
		}

		return false;
	}

	void Ini::ReadAllSettings() noexcept
	{
		ReadSettings(Default_Settings);
		ReadSettings(Custom_Settings);
	}

	void Ini::ReadSettings(const std::string& Filename) noexcept
	{
		CSimpleIni ini;

		ini.SetUnicode();
		auto error = ini.LoadFile(Filename.c_str());

		_DMESSAGE("File %s %sloaded!", Filename.c_str(), error != 0 ? "not ": "");

		if (error != 0) 
			return;

#define GETVALUE(val, section) \
		val = GetValue(ini, section, # val, val); \
		DbgValue(# val, val)

		GETVALUE(bEnablePlayer, Section::Main);

		GETVALUE(bEnableNPCs, Section::Main);

		GETVALUE(bEnableScript, Section::Main);

		GETVALUE(bEnableTextFile, Section::Main);

		GETVALUE(bEnableJsonFile, Section::Main);

		GETVALUE(bEnableExtraData, Section::Main);

		GETVALUE(bEnableDynamicCamera, Section::Main);

		GETVALUE(bEnable1stCamera, Section::Main);

		GETVALUE(bEnable3rdCamera, Section::Main);

		GETVALUE(bCache, Section::Main);

		GETVALUE(bAltRead, Section::Main);

		GETVALUE(bEnableAAF, Section::Main);

		GETVALUE(bEnableTagAAF, Section::Main);

		ParseString(ini, Section::Main, "sTagAAF", "", vTagAAF);

		GETVALUE(bLooksmenu, Section::Main);

		GETVALUE(iGender, Section::Main);

		GETVALUE(iBehaviorFurniture, Section::Main);

		InitFurnitureKeywords(ini);

		for (std::uint32_t index{ MinSlot }; index < MaxSlot; index++) 
			Set_bEnableSlot(index, GetValue(ini, Section::Slot, std::string("bEnableSlot") + std::to_string(index + 30), true));

		GETVALUE(bEnableSwimming, Section::AnimEvent);

		GETVALUE(bEnableFirstPersonAnim, Section::AnimEvent);

		GETVALUE(bEnableFixes, Section::AnimEvent);

		GETVALUE(bEnableReloadFixes, Section::AnimEvent);

		GETVALUE(iRace, Section::Race);

		InitRace(ini);

		GETVALUE(fHeight, Section::In_Game);

		GETVALUE(fStep, Section::In_Game);

		GETVALUE(iSlot, Section::In_Game);

		GETVALUE(iDirF4SE, Section::In_Game);

		GETVALUE(iReference, Section::In_Game);

		GETVALUE(iKeyStartStopPlayer, Section::Keys);

		GETVALUE(iKeyStartStopAll, Section::Keys);

		GETVALUE(iKeyActivateEdit, Section::Keys);

		GETVALUE(iKeyChangeRefr, Section::Keys);

		GETVALUE(iKeyCreateHeight, Section::Keys);

		GETVALUE(iKeyDeleteHeight, Section::Keys);

		GETVALUE(iKeyIncrementHeight, Section::Keys);

		GETVALUE(iKeyDecrementHeight, Section::Keys);
	}

	Ini Ini::instance;
}	
