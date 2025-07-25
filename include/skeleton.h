#pragma once

#include "version.h"

#include "settings.h"
#include "externals.h"

namespace Skeleton {

	class NiStreamPtr {

	public:

		NiStreamPtr() noexcept;
		~NiStreamPtr() noexcept;

		NiStreamPtr(const NiStreamPtr&) = delete;
		NiStreamPtr(NiStreamPtr&&) = delete;

		NiStreamPtr& operator=(const NiStreamPtr&) = delete;
		NiStreamPtr& operator=(NiStreamPtr&&) = delete;

		void* operator new(std::size_t) = delete;
		void* operator new[](std::size_t) = delete;

		void operator delete(void*) = delete;
		void operator delete[](void*) = delete;

		[[nodiscard]] NiStream* operator->() noexcept { return niStream; }
		[[nodiscard]] operator bool() noexcept { return niStream != nullptr; }

	private:

		NiStream* niStream{ nullptr };
		std::unique_ptr<std::uint8_t[]> mem;
	};

	class Reader {

	public:
		
		Reader() noexcept {}
		~Reader() noexcept {}

		Reader(const Reader&) = delete;
		Reader(Reader&&) = delete;

		Reader& operator=(const Reader&) = delete;
		Reader& operator=(Reader&&) = delete;

		Reader(const std::string& Filename) noexcept;
		Reader(Actor* actor, bool isFemale) noexcept;
		Reader(TESObjectREFR* refr, bool isFemale) noexcept;

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
