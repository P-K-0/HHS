#pragma once

#include "version.h"

#include "settings.h"
#include "externals.h"

namespace Skeleton {

	class NiStreamPtr {

	public:

		NiStreamPtr(const NiStreamPtr&) = delete;
		NiStreamPtr(NiStreamPtr&&) = delete;

		NiStreamPtr& operator=(const NiStreamPtr&) = delete;
		NiStreamPtr& operator=(NiStreamPtr&&) = delete;

		NiStreamPtr() noexcept;
		~NiStreamPtr() noexcept;

		[[nodiscard]] NiStream* operator->() noexcept { return niStream; }
		[[nodiscard]] operator bool() noexcept { return niStream != nullptr; }

	private:

		NiStream* niStream{ nullptr };

		std::uint8_t* mem{ nullptr };
	};

	class Reader {

	public:
		
		Reader(const Reader&) = delete;
		Reader(Reader&&) = delete;

		Reader& operator=(const Reader&) = delete;
		Reader& operator=(Reader&&) = delete;

		Reader() noexcept {}
		Reader(const std::string& Filename) noexcept;
		Reader(Actor* actor, const bool& isFemale) noexcept;
		Reader(TESObjectREFR* refr, const bool& isFemale) noexcept;

		~Reader() noexcept {}

		[[nodiscard]] bool HasNode(const std::string& name) noexcept;

		[[nodiscard]] bool GetExtraData(const std::string& name, float& value) noexcept;
		[[nodiscard]] bool GetExtraData(const std::string& name, std::uint32_t& value) noexcept;

		[[nodiscard]] operator bool() const noexcept { return isValid; }

	private:

		[[nodiscard]] bool Init(const std::string& Filename) noexcept;

		template<typename Func = std::function<bool(NiAVObject*)>>
		[[nodiscard]] bool Visit(NiAVObject* obj, Func fn) noexcept;

		template<typename Func = std::function<bool(NiAVObject*)>>
		[[nodiscard]] bool Visit(Func fn) noexcept;

		NiStreamPtr niStream;

		bool isValid{};
	};

	extern [[nodiscard]] float GetHeightFromSkeleton(const std::string& filename) noexcept;
}
