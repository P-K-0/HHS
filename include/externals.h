#pragma once

constexpr char* ComOverride = "COM_Override";
constexpr char* ExtraDataSAF = "SAF_Version";
constexpr char* ExtraDataHHS = "HHS";

constexpr auto DirData = "data\\"_sv;
constexpr auto DirMeshes = "meshes\\"_sv;
constexpr auto DirF4SE = "f4se\\plugins\\hhs\\"_sv;

constexpr std::uint32_t PlayerID = 0x14;

constexpr float MinValue = -1000.0f;
constexpr float MaxValue = 1000.0f;
constexpr float ZeroValue = 0.0f;

constexpr std::uint32_t InvalidSlot = 0;
constexpr std::uint32_t ReservedSlots = 12;
constexpr std::uint32_t MinSlot = 0;
constexpr std::uint32_t MaxSlot = (ActorEquipData::kMaxSlots - ReservedSlots);

class Trampoline {

public:

	[[nodiscard]] static Trampoline& GetSingleton() noexcept
	{
		static Trampoline instance;
		return instance;
	}

	void Create() noexcept
	{
		if (created) {
			return;
		}

		_DMESSAGE("Trampoline Space allocated : %i bytes", len);

		if (!g_branchTrampoline.Create(len)) {

			_ERROR("Branch Trampoline init error!");

			return;
		}

		if (!g_localTrampoline.Create(len, g_moduleHandle)) {

			_ERROR("Codegen buffer init error!");

			return;
		}

		created = true;
	}

	[[nodiscard]] bool IsCreated() const noexcept { return created; }

	void PrintSpaceLeft() noexcept
	{
		_DMESSAGE("Trampoline space left : %i/%i bytes (%.2f %%)",
			g_localTrampoline.Remain(),
			len,
			(100.0 * static_cast<double>(g_localTrampoline.Remain()) / static_cast<double>(len)));
	}

	template<typename T, typename Fn>
	void Alloc(Fn& fn)
	{
		void* codeBuf = g_localTrampoline.StartAlloc();
		T code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		PrintSpaceLeft();

		fn = (decltype(fn))codeBuf;
	}

private:

	Trampoline() noexcept {}
	virtual ~Trampoline() noexcept {}

	Trampoline(const Trampoline&) = delete;
	Trampoline(Trampoline&&) = delete;

	Trampoline& operator=(const Trampoline&) = delete;
	Trampoline& operator=(Trampoline&&) = delete;

	void* g_moduleHandle{ nullptr };
	std::size_t len{ 65536 };
	bool created{};
};

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

	if (!frm) {
		return;
	}

	auto actor = DYNAMIC_CAST(frm, TESForm, Actor);

	if (!actor || !actor->parentCell)
		return;

	for (std::uint32_t index{}; index < actor->parentCell->objectList.count; index++) {
	
		auto refr = actor->parentCell->objectList[index];

		if (refr) {
			func(refr);
		}
	}
}

template<typename Func = std::function<bool(ModInfo*)>>
void VisitMods(Func func)
{
	auto data = (*g_dataHandler);

	if (!data) {
		return;
	}

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME

	for (std::uint32_t itype{}; itype < 2; itype++) {

		std::uint32_t count_mods{ itype == 0 ? data->modList.loadedMods.count : data->modList.lightMods.count };

		for (std::uint32_t imods{}; imods < count_mods; imods++) {

			auto modinfo{ itype == 0 ? data->modList.loadedMods[imods] : data->modList.lightMods[imods] };

			if (modinfo && func(modinfo)) {
				return;
			}
		}
	}

#else

	std::uint32_t count_mods{ data->modList.loadedModCount };

	for (std::uint32_t imods{}; imods < count_mods; imods++) {

		auto modinfo = data->modList.loadedMods[imods];

		if (modinfo && func(modinfo)) {
			return;
		}
	}

#endif
}
