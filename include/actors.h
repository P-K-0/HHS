#pragma once

#include "version.h"

namespace Actors {

	using _ActorIsInPowerArmor = bool(*)(Actor*);

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
#if CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163
	static RelocAddr<_ActorIsInPowerArmor> ActorIsInPowerArmor(0x09CC980);
#elif CURRENT_RELEASE_RUNTIME == RUNTIME_VERSION_1_10_980
	static RelocAddr<_ActorIsInPowerArmor> ActorIsInPowerArmor(0x0935920);
#elif CURRENT_RELEASE_RUNTIME == RUNTIME_VERSION_1_10_984
	static RelocAddr<_ActorIsInPowerArmor> ActorIsInPowerArmor(0x0935950);
#endif
#else
	static RelocAddr<_ActorIsInPowerArmor> ActorIsInPowerArmor(0x09BF5D0);
#endif

	class Utility {

	public:

		Utility() noexcept {}
		Utility(Actor* act) noexcept;
		Utility(TESObjectREFR* refr) noexcept;
		Utility(TESForm* frm) noexcept;
		Utility(std::uint32_t id) noexcept;
		~Utility() noexcept {}

		Utility(const Utility& util) noexcept { *this = util; }
		Utility(Utility&& util) noexcept { *this = std::move(util); }

		Utility& operator=(const Utility& util) noexcept;
		Utility& operator=(Utility&& util) noexcept;

		[[nodiscard]] bool IsFemale() const noexcept { return isFemale; }
		[[nodiscard]] bool IsPlayer() const noexcept { return isPlayer; }
		[[nodiscard]] bool IsEnabled() const noexcept { return isEnabled; }
		[[nodiscard]] bool IsRaceCompatible() const noexcept { return isRaceCompatible; }

		[[nodiscard]] Actor* GetActorPtr() noexcept { return actor; }
		[[nodiscard]] std::uint32_t GetIDFromScriptKwrd() noexcept;
		[[nodiscard]] float GetHeightFromMod(std::uint32_t slot) noexcept;
		[[nodiscard]] float GetHeightFromWornItem(std::uint32_t slot = 0, std::uint32_t id = 0, bool equipped = false) noexcept;
		[[nodiscard]] std::uint64_t GetHandle() noexcept;
		[[nodiscard]] bool GetEquipData(std::uint32_t slot, std::uint32_t& id, std::string& filename) noexcept;
		[[nodiscard]] TESObjectREFR* GetFurnitureReference() noexcept;

		[[nodiscard]] operator bool() const noexcept { return isEnabled && actor; }

	private:

		Actor* actor{ nullptr };
		
		bool isFemale{};
		bool isEnabled{};
		bool isPlayer{};
		bool isRaceCompatible{};

		void Init() noexcept;
	};

	[[nodiscard]] extern std::uint32_t GetSlotMaskByID(std::uint32_t id) noexcept;
}
