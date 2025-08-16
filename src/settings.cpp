
#include "settings.h"

namespace Settings {

	bool operator&(const Race& a, const Race& b) noexcept
	{
		return (static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b)) != 0;
	}

	void Ini::ParseString(const std::string& str, std::vector<std::string>& vStr) noexcept
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

	bool Ini::ParseString(CSimpleIni& ini, Section section, const char* key, std::vector<std::string>& vStr) noexcept
	{
		std::string str = ini.GetValue(vSection[static_cast<std::uint32_t>(section)].data(), key, "");

		if (str.empty()) {
			return false;
		}

		ParseString(str, vStr);

		return true;
	}

	bool Ini::HasRaceKeyword(TESRace* race) noexcept
	{
		for (std::uint32_t idx{}; idx < race->keywordForm.numKeywords; ++idx) {

			auto keyRaceHHS = race->keywordForm.keywords[idx];

			if (keyRaceHHS && BSCompi(keyRaceHHS->keyword, ActorType_HHS)) {
				return true;
			}
		}

		return false;
	}

	void Ini::InitFurnitureKeywords(CSimpleIni& ini) noexcept
	{
		auto data = (*g_dataHandler);

		if (!data) {
			return;
		}

		std::vector<std::string> vStr;

		if (!ParseString(ini, Section::Main, "sFurnitureKeyword", vStr)) {
			return;
		}

		std::uint32_t cnt{};

		vFurnitureKeyword.clear();

		_DMESSAGE("Loading keywords:");

		for (std::uint32_t iKey{}; iKey < data->arrKYWD.count; iKey++) {

			auto key = data->arrKYWD[iKey];

			if (key) {

				for (const auto& s : vStr) {

					if (BSCompi(key->keyword, s.c_str())) {

						vFurnitureKeyword.push_back(key->formID);
					
						cnt++;
					
						DbgMessage("Keyword", key->keyword.c_str(), key->formID);
					
						break;
					}
				}
			}
		}

		if (cnt == 0) {

			_DMESSAGE("Error during furniture keywords init!");
			return;
		}

		std::sort(vFurnitureKeyword);
		vFurnitureKeyword.erase(std::unique(vFurnitureKeyword), vFurnitureKeyword.end());
	}

	void Ini::InitRace(CSimpleIni& ini) noexcept
	{
		auto data = (*g_dataHandler);

		if (!data || iRace == Race::AllRaces) {
			return;
		}

		std::vector<std::string> vStr;

		if (!ParseString(ini, Section::Race, "sRace", vStr)) {
			return;
		}

		std::uint32_t cnt{};

		vRace.clear();

		_DMESSAGE("Loading races:");

		for (std::uint32_t idx{}; idx < data->arrRACE.count; idx++) {

			auto race = data->arrRACE[idx];

			if (race && race->editorId.c_str()) {

				auto edid = race->editorId.c_str();

				if (iRace & Race::FromSkeleton) {

					for (std::uint32_t isFemale{}; isFemale < 2; isFemale++) {

						Skeleton::Reader skeleton{ race->models[isFemale].GetModelName() };

						if (skeleton && skeleton.HasNode(ComOverride)) {

							vRace.push_back(race->formID);

							cnt++;

							DbgMessage("Race", edid, race->formID, "from Skeleton");

							break;
						}
					}
				}

				if (iRace & Race::FromEditorID) {

					for (const auto& s : vStr) {

						bool hasKeyword{ HasRaceKeyword(race) };

						if (_strcmpi(s.c_str(), edid) == 0 || hasKeyword) {

							vRace.push_back(race->formID);

							cnt++;

							if (hasKeyword) {
								DbgMessage("Race", edid, race->formID, "from Keyword");
							}
							else {
								DbgMessage("Race", edid, race->formID, "from Settings");
							}

							break;
						}
					}
				}
			}
		}

		if (cnt == 0) {

			_DMESSAGE("Error during races init!");
			return;
		}

		std::sort(vRace);
		vRace.erase(std::unique(vRace), vRace.end());
	}

	void Ini::SetEnableSlot(std::uint32_t index, bool value) noexcept
	{
		if (index < MinSlot || index >= MaxSlot) {
			return;
		}

		bEnableSlot[index] = value;

		if (value) {
			iSlotFlags |= (1 << index);
		}
		else {
			iSlotFlags &= ~(1 << index);
		}

		iCountSlot = 0;

		for (std::uint32_t index{}; index < MaxSlot; ++index) {

			if (bEnableSlot[index]) {
				aSlot[iCountSlot] = index;
				iCountSlot++;
			}
		}
	}

	bool Ini::CheckFurnitureBehavior(TESObjectREFR* refr) noexcept
	{
		TESForm* frm{};

		if (!refr || !(frm = refr->baseForm)) {
			return false;
		}

		if (iBehaviorFurniture == Furniture::Disabled) {
			return true;
		}

		if (iBehaviorFurniture == Furniture::Keywords) {

			TESFurniture* furn{ nullptr };

			if (vFurnitureKeyword.empty() || !(furn = DYNAMIC_CAST(frm, TESForm, TESFurniture))) {
				return false;
			}

			for (std::uint32_t idx{}; idx < furn->keywordForm.numKeywords; ++idx) {

				auto key = furn->keywordForm.keywords[idx];

				if (key && 
					std::binary_search(vFurnitureKeyword, key->formID)) {
					return true;
				}
			}
		}

		return false;
	}

	bool Ini::CheckRace(TESRace* race) noexcept
	{
		if (iRace == Race::AllRaces) {
			return true;
		}

		return std::binary_search(vRace, race->formID);
	}

	bool Ini::CheckTagAAF(const std::string& tag) noexcept
	{
		if (!bEnableTagAAF) {
			return false;
		}

		for (auto& sTag : vTagAAF) {

			if (_strcmpi(sTag.c_str(), tag.c_str()) == 0) {
				return true;
			}
		}

		return false;
	}

	void Ini::ReadAllSettings() noexcept
	{
		ReadSettings(Default_Settings);
		ReadSettings(Custom_Settings);
	}

	void Ini::GetValue(CSimpleIni& ini, Section section, const char* key, bool& value) noexcept
	{
		auto ret = ini.GetBoolValue(vSection[static_cast<std::uint32_t>(section)].data(), key, value);
		DMsg(key, ret);
		value = ret;
	}

	void Ini::GetValue(CSimpleIni& ini, Section section, const char* key, float& value) noexcept
	{
		auto ret = ini.GetDoubleValue(vSection[static_cast<std::uint32_t>(section)].data(), key, static_cast<double>(value));
		DMsg(key, ret);
		value = static_cast<float>(ret);
	}

	template<typename T>
	void Ini::GetValue(CSimpleIni& ini, Section section, const char* key, T& value) noexcept
	{
		auto ret = ini.GetLongValue(vSection[static_cast<std::uint32_t>(section)].data(), key, static_cast<std::uint32_t>(value));
		DMsg(key, ret);

		if constexpr (std::is_same_v<T, Gender>) {
			value = gender_cast(ret);
		}
		else if constexpr (std::is_same_v<T, Furniture>) {
			value = furniture_cast(ret);
		}
		else if constexpr (std::is_same_v<T, Race>) {
			value = race_cast(ret);
		}
		else if constexpr (std::is_same_v<T, Reference>) {
			value = reference_cast(ret);
		}
		else {
			value = static_cast<std::uint32_t>(ret);
		}
	}

	void Ini::ReadSettings(const std::string& Filename) noexcept
	{
		CSimpleIni ini;

		ini.SetUnicode();
		auto error = ini.LoadFile(Filename.c_str());

		_DMESSAGE("File %s: %sloaded", Filename.c_str(), error != 0 ? "not " : "");

		if (error != 0) {
			return;
		}

		// [Main]
		GetValue(ini, Section::Main, "bEnablePlayer", bEnablePlayer);
		GetValue(ini, Section::Main, "bEnableNPCs", bEnableNPCs);
		GetValue(ini, Section::Main, "bEnableScript", bEnableScript);
		GetValue(ini, Section::Main, "bEnableTextFile", bEnableTextFile);
		GetValue(ini, Section::Main, "bEnableJsonFile", bEnableJsonFile);
		GetValue(ini, Section::Main, "bEnableExtraData", bEnableExtraData);
		GetValue(ini, Section::Main, "bEnableDynamicCamera", bEnableDynamicCamera);
		GetValue(ini, Section::Main, "bEnable1stCamera", bEnable1stCamera);
		GetValue(ini, Section::Main, "bEnable3rdCamera", bEnable3rdCamera);
		GetValue(ini, Section::Main, "bEnableCustomCameraPatch", bEnableCustomCameraPatch);
		GetValue(ini, Section::Main, "bCache", bCache);
		GetValue(ini, Section::Main, "bAltRead", bAltRead);
		GetValue(ini, Section::Main, "iReadBufferLen", iReadBufferLen);
		GetValue(ini, Section::Main, "bEnableAAF", bEnableAAF);
		GetValue(ini, Section::Main, "bEnableTagAAF", bEnableTagAAF);

		ParseString(ini, Section::Main, "sTagAAF", vTagAAF);

		GetValue(ini, Section::Main, "bLooksmenu", bLooksmenu);
		GetValue(ini, Section::Main, "bTerminal", bTerminal);
		GetValue(ini, Section::Main, "iGender", iGender);
		GetValue(ini, Section::Main, "iBehaviorFurniture", iBehaviorFurniture);

		InitFurnitureKeywords(ini);

		// [Slot]
		for (std::uint32_t index{ MinSlot }; index < MaxSlot; index++) {
			bool value{ true };
			GetValue(ini, Section::Slot, ("bEnableSlot" + std::to_string(index + 30)).c_str(), value);
			SetEnableSlot(index, value);
		}

		// [Race]
		GetValue(ini, Section::Race, "iRace", iRace);

		InitRace(ini);

		// [AnimEvent]
		GetValue(ini, Section::AnimEvent, "bEnableSwimming", bEnableSwimming);
		GetValue(ini, Section::AnimEvent, "bEnableBleedOut", bEnableBleedOut);
		GetValue(ini, Section::AnimEvent, "bEnableFirstPersonAnim", bEnableFirstPersonAnim);
		GetValue(ini, Section::AnimEvent, "bEnableFixes", bEnableFixes);
		GetValue(ini, Section::AnimEvent, "bEnableReloadFixes", bEnableReloadFixes);

		// [In-Game]
		GetValue(ini, Section::InGame, "fHeight", fHeight);
		GetValue(ini, Section::InGame, "fStep", fStep);
		GetValue(ini, Section::InGame, "iSlot", iSlot);
		GetValue(ini, Section::InGame, "iDirF4SE", iDirF4SE);
		GetValue(ini, Section::InGame, "iReference", iReference);

		// [Keys]
		GetValue(ini, Section::Keys, "iKeyStartStopPlayer", iKeyStartStopPlayer);
		GetValue(ini, Section::Keys, "iKeyStartStopAll", iKeyStartStopAll);
		GetValue(ini, Section::Keys, "iKeyActivateEdit", iKeyActivateEdit);
		GetValue(ini, Section::Keys, "iKeyChangeRefr", iKeyChangeRefr);
		GetValue(ini, Section::Keys, "iKeyCreateHeight", iKeyCreateHeight);
		GetValue(ini, Section::Keys, "iKeyDeleteHeight", iKeyDeleteHeight);
		GetValue(ini, Section::Keys, "iKeyIncrementHeight", iKeyIncrementHeight);
		GetValue(ini, Section::Keys, "iKeyDecrementHeight", iKeyDecrementHeight);
	}
}	
