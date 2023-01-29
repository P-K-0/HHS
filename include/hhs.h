#pragma once

#include "version.h"

#include "f4se_plugin.h"
#include "node.h"
#include "actors.h"
#include "camera.h"
#include "skeleton.h"
#include "fixes.h"
#include "externals.h"

namespace hhs {

	enum class Error {

		Unknown = -9,
		ComOverride,
		Runtime,
		ActorDisabled,
		Override,
		SetHeight,
		Race,
		HeightTxt,
		Arguments,
		Success
	};

	template<typename RetT = int>
	[[nodiscard]] inline constexpr RetT cast_error(const Error& err) noexcept { return static_cast<RetT>(err); }

	class System : 
		public Node::Transform {

	public:

		System(const System& sys) = delete;
		System(System&& sys) = delete;

		System& operator=(const System&) = delete;
		System& operator=(System&&) = delete;

		System() noexcept {}
		~System() noexcept {}

		[[nodiscard]] Error SetHeight(const float& Height) noexcept;
		[[nodiscard]] Error SetHeight(const std::uint32_t& slot = 0, const std::uint32_t& id = 0, const bool equipped = false) noexcept;

		[[nodiscard]] Error ResetHeight() noexcept;

		[[nodiscard]] Error Start() noexcept;
		[[nodiscard]] Error Stop(const bool& stopAAF = false) noexcept;

		[[nodiscard]] Error Swim(const bool& swim) noexcept;

		void SetActorUtil(Actors::Utility& utility) noexcept;
		void SetOverride(const bool& value) noexcept { hasOverride = value; }

		[[nodiscard]] const float& GetHeight() const noexcept { return height; }
		
		[[nodiscard]] const bool& HasOverride() const noexcept { return hasOverride; }
		[[nodiscard]] const bool HasHeight() const noexcept { return height > MinValue; }
		
		[[nodiscard]] const bool& IsAAF() const noexcept { return isAAF; }
		[[nodiscard]] const bool& IsStop() const noexcept { return isStop; }
		[[nodiscard]] const bool& IsSwimming() const noexcept { return isSwimming; }
		[[nodiscard]] const bool& IsSitting() const noexcept { return isSitting; }

		[[nodiscard]] Actors::Utility& GetActorUtil() noexcept { return util; }

		void EnableFix(TESObjectREFR* furniture) noexcept;
		void DisableFix() noexcept;

	private:

		bool isStop{};
		bool isAAF{};
		bool isSwimming{};
		bool isSitting{};

		bool hasOverride{};
		
		float height{};

		Actors::Utility util;
	};

	class Map {

	public:

		using iterator = std::unordered_map<std::uint64_t, System>::iterator;

		[[nodiscard]] static Map& GetInstance() noexcept { return instance; }

		template<typename T, typename Fn = std::function<Error(System&)>>
		[[nodiscard]] Error visit(const bool& bOverride, T t, Fn fn) noexcept
		{
			if (!f4se::Plugin::GetInstance().IsRuntimeValid())
				return Error::Runtime;

			Actors::Utility util{ t };

			if (!util)
				return Error::ActorDisabled;

			if (!util.IsRaceCompatible())
				return Error::Race;

			auto handle = util.GetHandle();

			if (handle == 0)
				return Error::Unknown;

			auto& it = map.find(handle);

			if (it != map.end()) {
	
				if (it->second.HasOverride() && !bOverride)
					return Error::Override;

				it->second.SetActorUtil(util);

				return fn(it->second);
			}

			auto& m = map[handle];

			m.SetActorUtil(util);

			return fn(m);
		}

		[[nodiscard]] iterator begin() noexcept { return map.begin(); }
		[[nodiscard]] iterator end() noexcept { return map.end(); }

	private:

		Map(const Map&) = delete;
		Map(Map&&) = delete;

		Map& operator=(const Map&) = delete;
		Map& operator=(Map&&) = delete;

		Map() noexcept {}
		~Map() noexcept {}

		std::unordered_map<std::uint64_t, System> map;

		static Map instance;
	};
}
