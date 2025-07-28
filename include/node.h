#pragma once

#include "version.h"

#include "util.h"

namespace Node {

	enum class Angle : std::uint32_t {

		Heading,
		Attitude,
		Bank
	};

	enum class Flags : std::uint32_t {

		PosX,
		PosY,
		PosZ,
		Scale,
		RotX,
		RotY,
		RotZ,
		Count
	};

	template<typename T>
	constexpr Flags flags_cast(T value) noexcept {
		return std::clamp(static_cast<Flags>(value), Flags::PosX, Flags::RotZ);
	}

	class Pool :
		public util::Singleton<Pool> {
		friend class util::Singleton<Pool>;

	public:

		void TryAddingToPool(const char* sNode) noexcept;
		[[nodiscard]] const std::string& GetFromPool(std::size_t hash) noexcept;

	private:

		Pool() noexcept = default;
		~Pool() noexcept = default;

		std::unordered_map<std::size_t, std::string> pool;
		std::string strEmpty{};
	};

	struct InfoNiTransform {

		bool isSet{};
		std::uint8_t flags{};
		NiTransform transform{};
	};

	class Transform : 
		public util::NoCopyable,
		public util::NoMoveable,
		public util::NoPointer {

	public:

		Transform() noexcept = default;
		~Transform() noexcept = default;

		Transform(Actor* actor) noexcept
			: act{ actor } {}

		Transform(Actor* actor, bool first_person = false) noexcept
			: act{ actor }, firstPerson{ first_person } {}

		[[nodiscard]] float GetTransform(const char* sNode, Flags flags) noexcept;	
		[[nodiscard]] std::int32_t SetTransform(const char* sNode, Flags flags, float value) noexcept;
		[[nodiscard]] std::int32_t ResetTransform(const char* sNode, Flags flags) noexcept;
		[[nodiscard]] std::uint32_t GetID() noexcept { return act ? act->formID : 0; }
		[[nodiscard]] Actor* GetActorPtr() noexcept { return act; }
		[[nodiscard]] float GetEulerAngle(NiTransform& niTransform, Angle angle) noexcept;

		void SetActor(Actor* actor) noexcept { act = actor; }
		void SetFirstPerson(bool first_person) noexcept { firstPerson = first_person; }
		void SetEulerAngle(NiTransform& src, NiTransform& dst, Angle angle, float value = 0.0f) noexcept;
		void ResetTransform(std::size_t hash) noexcept;
		void ResetTransform(const char* sNode) noexcept;

	protected:

		Actor* act{ nullptr };

		bool firstPerson{};

		std::unordered_map<std::size_t, InfoNiTransform> map;

		template<typename Func = std::function<void(NiAVObject*)>>
		[[nodiscard]] std::int32_t Visit(const char* sNode, bool update, Func fn) noexcept;
	};
}
