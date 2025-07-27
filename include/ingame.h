#pragma once

#include "hhs.h"
#include "util.h"

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME

namespace InGame {

	enum class Key : std::uint32_t {

		Activate = 0,
		Test,
		Create,
		Delete,
		Increment,
		Decrement,
		ChangeReference
	};

	class HeightEdit :
		public BSTEventSink<ViewCasterUpdateEvent>, 
		public util::Singleton<HeightEdit> {
		friend class util::Singleton<HeightEdit>;

	public:

		[[nodiscard]] bool Test() noexcept;
		[[nodiscard]] bool Create() noexcept;
		[[nodiscard]] bool Delete() noexcept;

		void OnKeyPress(const Key& key) noexcept;

		virtual	EventResult	ReceiveEvent(ViewCasterUpdateEvent* evn, void* dispatcher);

		void Register() noexcept;

	private:

		HeightEdit() noexcept = default;
		~HeightEdit() noexcept = default;

		[[nodiscard]] std::uint32_t GetFormIDByHandle(UInt32 handle) noexcept;

		[[nodiscard]] bool Process(Key key) noexcept;

		void IncHeight() noexcept;
		void DecHeight() noexcept;

		void ShowError(hhs::Error error) noexcept;

		bool enabled{};

		decltype(ViewCasterUpdateEvent::handle) handle{};
	};
}

#endif
