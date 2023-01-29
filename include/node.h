#pragma once

#include "version.h"

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
	inline [[nodiscard]] Flags flags_cast(const T& value) noexcept {
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

	class Transform {

	public:

		Transform(const Transform&) = delete;
		Transform(Transform&&) = delete;

		Transform& operator=(const Transform&) = delete;
		Transform& operator=(Transform&&) = delete;

		Transform() noexcept {}

		Transform(Actor* actor) noexcept
			: act{ actor } {}

		Transform(Actor* actor,  const bool& first_person = false) noexcept
			: firstPerson{ first_person }, act{ actor } {}

		~Transform() noexcept {}

		void SetActor(Actor* actor) noexcept { act = actor; }
		void SetFirstPerson(const bool& first_person) noexcept { firstPerson = first_person; }

		[[nodiscard]] float GetTransform(const std::string& sNode, const Flags& flags) noexcept;	
		[[nodiscard]] std::int32_t SetTransform(const std::string& sNode, const Flags& flags, const float& value) noexcept;
		[[nodiscard]] std::int32_t ResetTransform(const std::string& sNode, const Flags& flags) noexcept;
		void ResetTransform(const std::string& sNode) noexcept;

		[[nodiscard]] std::uint32_t GetID() noexcept { return act ? act->formID : 0; }

		[[nodiscard]] Actor* GetActorPtr() noexcept { return act; }

		[[nodiscard]] float GetEulerAngle(NiTransform& niTransform, const Angle& angle) noexcept;

		void SetEulerAngle(NiTransform& src, NiTransform& dst, const Angle& angle, const float& value = 0.0f) noexcept;

	protected:

		Actor* act{ nullptr };

		bool firstPerson{};

		std::unordered_map<std::string, std::pair<bool, std::pair<NiTransform, vector_flags>>> map;

		template<typename Func = std::function<void(NiAVObject*)>>
		[[nodiscard]] std::int32_t Visit(const std::string& sNode, const bool noupdate, Func fn) noexcept;
	};
}
