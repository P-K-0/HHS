#pragma once

#include "version.h"

#include "settings.h"
#include "externals.h"
#include "util.h"

namespace Skeleton {

	class NiStreamPtr :
		public util::NoCopyable,
		public util::NoMoveable,
		public util::NoPointer {

	public:

		NiStreamPtr() noexcept;
		~NiStreamPtr() noexcept;

		[[nodiscard]] NiStream* operator->() noexcept { return niStream; }
		[[nodiscard]] operator bool() noexcept { return niStream != nullptr; }

	private:

		NiStream* niStream{ nullptr };
		std::unique_ptr<std::uint8_t[]> mem;
	};

	class Reader :
		public util::NoCopyable,
		public util::NoMoveable,
		public util::NoPointer {

	public:
		
		Reader() noexcept = default;
		~Reader() noexcept = default;

		Reader(const std::string& Filename) noexcept;
		Reader(Actor* actor, bool isFemale) noexcept;
		Reader(TESObjectREFR* refr, bool isFemale) noexcept;

		[[nodiscard]] bool HasNode(const std::string& name) noexcept;

		[[nodiscard]] bool GetExtraData(const std::string& name, float& value) noexcept;
		[[nodiscard]] bool GetExtraData(const std::string& name, std::uint32_t& value) noexcept;

		[[nodiscard]] operator bool() const noexcept { return isValid; }

	private:

		[[nodiscard]] bool Init(const std::string& Filename) noexcept;

		template<typename Fn> // Fn = bool(NiAVObject*)
		[[nodiscard]] bool Visit(NiAVObject* obj, Fn fn) noexcept;

		template<typename Fn> // Fn = bool(NiAVObject*)
		[[nodiscard]] bool Visit(Fn fn) noexcept;

		NiStreamPtr niStream;

		bool isValid{};
	};

	extern [[nodiscard]] float GetHeightFromSkeleton(const std::string& filename) noexcept;
}
