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
	inline [[nodiscard]] Flags flags_cast(T value) noexcept {
		return std::clamp(static_cast<Flags>(value), Flags::PosX, Flags::RotZ);
	}

	class vector_flags :
		public std::vector<bool> {

	public:

		vector_flags() noexcept { assign(static_cast<std::size_t>(Flags::Count), false); }

		std::vector<bool>::reference operator[](Flags flags) noexcept
		{
			return std::vector<bool>::operator[](static_cast<std::size_t>(flags));
		}
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

		Transform(Actor* actor,  const bool& first_person = false) noexcept
			: act{ actor }, firstPerson{ first_person } {}

		[[nodiscard]] float GetTransform(const std::string& sNode, Flags flags) noexcept;	
		[[nodiscard]] std::int32_t SetTransform(const std::string& sNode, Flags flags, float value) noexcept;
		[[nodiscard]] std::int32_t ResetTransform(const std::string& sNode, Flags flags) noexcept;
		[[nodiscard]] std::uint32_t GetID() noexcept { return act ? act->formID : 0; }
		[[nodiscard]] Actor* GetActorPtr() noexcept { return act; }
		[[nodiscard]] float GetEulerAngle(NiTransform& niTransform, Angle angle) noexcept;

		void SetActor(Actor* actor) noexcept { act = actor; }
		void SetFirstPerson(bool first_person) noexcept { firstPerson = first_person; }
		void SetEulerAngle(NiTransform& src, NiTransform& dst, Angle angle, float value = 0.0f) noexcept;
		void ResetTransform(const std::string& sNode) noexcept;

	protected:

		Actor* act{ nullptr };

		bool firstPerson{};

		std::unordered_map<std::string, std::pair<bool, std::pair<NiTransform, vector_flags>>> map;

		template<typename Func = std::function<void(NiAVObject*)>>
		[[nodiscard]] std::int32_t Visit(const std::string& sNode, bool update, Func fn) noexcept;
	};
}
