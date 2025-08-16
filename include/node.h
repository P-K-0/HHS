#pragma once

#include "version.h"
#include "util.h"

namespace Node {

	enum class Angle : std::uint32_t {

		Heading,
		Attitude,
		Bank
	};

	enum class Flags {

		None = -1,
		PosX,
		PosY,
		PosZ,
		Scale,
		RotX,
		RotY,
		RotZ,
		Count
	};

	[[nodiscard]] constexpr std::uint8_t GetAllBitFlags() noexcept {
		return ~(1 << static_cast<std::uint8_t>(Flags::Count));
	}

	template<typename T>
	[[nodiscard]] constexpr Flags flags_cast(T value) noexcept {
		return std::clamp(static_cast<Flags>(value), Flags::None, Flags::RotZ);
	}

	class Pool :
		public util::Singleton<Pool> {
		friend class util::Singleton<Pool>;

	public:

		void TryAddingToPool(const char* node) noexcept;
		void TryAddingToPool(const char* node, std::size_t key) noexcept;
		[[nodiscard]] const std::string& GetFromPool(std::size_t key) noexcept;

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

	struct PairValue {

		float value{};
		bool mul{};

		[[nodiscard]] float GetMulValue(float height) noexcept {
			return value * (mul ? height : 1.0f);
		}
	};

	struct NodeValues {

		NodeValues(const char* node) noexcept
			: nodeName(node) {}

		std::string nodeName;
		PairValue x;
		PairValue y;
		PairValue z;
		PairValue heading;
		PairValue attitude;
		PairValue bank;
		PairValue scale;
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

		Transform(Actor* actor, bool firstPerson = false) noexcept
			: act{ actor }, frstPerson{ firstPerson } {}

		[[nodiscard]] float GetTransform(const char* node, Flags flags) noexcept;	
		[[nodiscard]] std::int32_t SetTransform(const char* node, NodeValues& values, float height) noexcept;
		[[nodiscard]] std::int32_t SetTransform(const char* node, Flags flags, float value) noexcept;
		[[nodiscard]] std::int32_t ResetTransform(const char* node, Flags flags) noexcept;
		[[nodiscard]] std::uint32_t GetID() noexcept { return act ? act->formID : 0; }
		[[nodiscard]] Actor* GetActorPtr() noexcept { return act; }
		[[nodiscard]] float GetEulerAngle(NiTransform& niTransform, Angle angle) noexcept;

		void SetActor(Actor* actor) noexcept { act = actor; }
		void SetFirstPerson(bool firstPerson) noexcept { frstPerson = firstPerson; }
		void SetEulerAngle(NiTransform& src, NiTransform& dst, Angle angle, float value = 0.0f) noexcept;
		void ResetTransform(std::size_t key) noexcept;
		void ResetTransform(const char* node) noexcept;

	protected:

		Actor* act{ nullptr };

		bool frstPerson{};

		std::unordered_map<std::size_t, InfoNiTransform> map;

		template<typename Fn> // Fn = void(NiAVObject*)
		[[nodiscard]] std::int32_t Visit(const char* node, std::size_t key, bool update, Fn fn) noexcept;
	};
}
