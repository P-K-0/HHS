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
	[[nodiscard]] inline constexpr RetT cast_error(Error err) noexcept { return static_cast<RetT>(err); }

	class System : 
		public Node::Transform {

	public:

		System() noexcept {}
		~System() noexcept {}

		System(const System& sys) = delete;
		System(System&& sys) = delete;

		System& operator=(const System&) = delete;
		System& operator=(System&&) = delete;

		[[nodiscard]] Error SetHeight(float Height) noexcept;
		[[nodiscard]] Error SetHeight(std::uint32_t slot = 0, std::uint32_t id = 0, bool equipped = false) noexcept;
		[[nodiscard]] Error ResetHeight() noexcept;
		[[nodiscard]] Error Start() noexcept;
		[[nodiscard]] Error Stop(bool stopAAF = false) noexcept;
		[[nodiscard]] Error Swim(bool swim) noexcept;
		[[nodiscard]] float GetHeight() const noexcept { return height; }
		[[nodiscard]] bool HasOverride() const noexcept { return hasOverride; }
		[[nodiscard]] bool HasHeight() const noexcept { return height != ZeroValue; }
		[[nodiscard]] bool IsAAF() const noexcept { return isAAF; }
		[[nodiscard]] bool IsStop() const noexcept { return isStop; }
		[[nodiscard]] bool IsSwimming() const noexcept { return isSwimming; }
		[[nodiscard]] bool IsSkip() const noexcept { return skip; }
		[[nodiscard]] Actors::Utility& GetActorUtil() noexcept { return util; }

		void SetOverride(bool value) noexcept { hasOverride = value; }
		void Skip(bool skip) noexcept { this->skip = skip; }
		void EnableFix(TESObjectREFR* furniture) noexcept;
		void DisableFix() noexcept;

	private:

		bool isStop{};
		bool isAAF{};
		bool isSwimming{};
		bool hasOverride{};
		bool skip{};
		
		float height{};

		Actors::Utility util;
	};

	enum class VisitFlags : std::uint32_t {

		None = 0,
		Override
	};

	class Map {

	public:

		[[nodiscard]] static Map& GetSingleton() noexcept {
			static Map instance;
			return instance;
		}

		template<typename T, typename Fn = std::function<Error(System&)>>
		[[nodiscard]] Error visit(VisitFlags flags, T value, Fn fn) noexcept
		{
			std::lock_guard<std::mutex> lock(mutex);

			if (!f4se::Plugin::GetSingleton().IsRuntimeValid()) {
				return Error::Runtime;
			}

			auto actor = Actors::GetActor(value);

			auto handle = Actors::GetHandle(actor);

			if (handle == 0) {
				return Error::Unknown;
			}

			auto& m = map[handle];

			auto& util = m.GetActorUtil();

			if (!util.Update(actor)) {
				return Error::Unknown;
			}

			if (!util) {
				return Error::ActorDisabled;
			}

			if (!util.IsRaceCompatible()) {
				return Error::Race;
			}

			m.SetActor(actor);

			if (m.HasOverride() && flags == VisitFlags::None) {
				return Error::Override;
			}

			return fn(m);
		}

		template<typename Fn = std::function<void(System&)>>
		void visit_all(Fn fn) noexcept
		{
			std::lock_guard<std::mutex> lock(mutex);

			for (auto& m : map) {
				fn(m.second);
			}
		}

	private:

		Map(const Map&) = delete;
		Map(Map&&) = delete;

		Map& operator=(const Map&) = delete;
		Map& operator=(Map&&) = delete;

		Map() noexcept {}
		~Map() noexcept {}

		std::unordered_map<std::uint64_t, System> map;
	
		std::mutex mutex;
	};
}
