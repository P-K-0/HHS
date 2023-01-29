#pragma once

#include "version.h"

namespace Actors {

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

		[[nodiscard]] const bool& IsFemale() const noexcept { return isFemale; }
		[[nodiscard]] const bool& IsPlayer() const noexcept { return isPlayer; }
		[[nodiscard]] const bool& IsEnabled() const noexcept { return isEnabled; }
		[[nodiscard]] const bool& IsRaceCompatible() const noexcept { return isRaceCompatible; }

		[[nodiscard]] Actor* GetActorPtr() noexcept { return actor; }
		[[nodiscard]] std::uint32_t GetIDFromScriptKwrd() noexcept;
		[[nodiscard]] float GetHeightFromMod(const std::uint32_t& slot) noexcept;
		[[nodiscard]] float GetHeightFromWornItem(const std::uint32_t& slot = 0, const std::uint32_t& id = 0, const bool equipped = false) noexcept;
		[[nodiscard]] std::uint64_t GetHandle() noexcept;
		[[nodiscard]] bool GetEquipData(const std::uint32_t& slot, std::uint32_t& id, std::string& filename) noexcept;
		[[nodiscard]] TESObjectREFR* GetFurnitureReference() noexcept;

		[[nodiscard]] operator bool() const noexcept { return isEnabled && actor; }

		template<typename T>
		[[nodiscard]] T* As() noexcept { return DYNAMIC_CAST(actor, Actor, T); }

	private:

		Actor* actor{ nullptr };
		
		bool isFemale{};
		bool isEnabled{};
		bool isPlayer{};
		bool isRaceCompatible{};

		void Init() noexcept;
	};

	[[nodiscard]] extern std::uint32_t GetSlotMaskByID(const std::uint32_t& id) noexcept;
}
