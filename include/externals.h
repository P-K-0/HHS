#pragma once

constexpr char* ComOverride = "COM_Override";
constexpr char* ExtraDataSAF = "SAF_Version";
constexpr char* ExtraDataHHS = "HHS";

constexpr char* DirData = "Data\\";
constexpr char* DirMeshes = "Data\\Meshes\\";
constexpr char* DirF4SE = "Data\\F4SE\\Plugins\\HHS\\";

constexpr std::uint32_t PlayerID = 0x14;

constexpr float MinValue = 0.0f;
constexpr float MaxValue = 1000.0f;
constexpr float InvalidValue = MinValue - 1.0f;

constexpr std::uint32_t InvalidSlot = 0;
constexpr std::uint32_t ReservedSlots = 12;
constexpr std::uint32_t MinSlot = 0;
constexpr std::uint32_t MaxSlot = (ActorEquipData::kMaxSlots - ReservedSlots);

template<typename T>
[[nodiscard]] char* as_bytes(T& t)
{
	void* addr = &t;

	return static_cast<char*>(addr);
}

template<typename Func = std::function<void(TESObjectREFR*)>>
void VisitCell(Func func)
{
	auto frm = LookupFormByID(PlayerID);

	if (!frm)
		return;

	auto actor = DYNAMIC_CAST(frm, TESForm, Actor);

	if (!actor || !actor->parentCell)
		return;

	for (std::uint32_t index{}; index < actor->parentCell->objectList.count; index++) {
	
		auto refr = actor->parentCell->objectList[index];

		if (!refr)
			continue;

		func(refr);
	}
}

template<typename Func = std::function<bool(ModInfo*)>>
void VisitMods(Func func)
{
	auto data = (*g_dataHandler);

	if (!data)
		return;

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME

	for (std::uint32_t itype{}; itype < 2; itype++) {

		std::uint32_t count_mods{ itype == 0 ? data->modList.loadedMods.count : data->modList.lightMods.count };

		for (std::uint32_t imods{}; imods < count_mods; imods++) {

			auto modinfo{ itype == 0 ? data->modList.loadedMods[imods] : data->modList.lightMods[imods] };

			if (modinfo && func(modinfo))
				return;
		}
	}

#else

	std::uint32_t count_mods{ data->modList.loadedModCount };

	for (std::uint32_t imods{}; imods < count_mods; imods++) {

		auto modinfo = data->modList.loadedMods[imods];

		if (modinfo && func(modinfo))
			return;
	}

#endif
}
