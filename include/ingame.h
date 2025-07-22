#pragma once

#include "version.h"

#include "hhs.h"

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
		public BSTEventSink<ViewCasterUpdateEvent> {

	public:

		static [[nodiscard]] HeightEdit& GetSingleton() noexcept {
			static HeightEdit instance;
			return instance; 
		}

		[[nodiscard]] bool Test() noexcept;
		[[nodiscard]] bool Create() noexcept;
		[[nodiscard]] bool Delete() noexcept;

		void OnKeyPress(const Key& key) noexcept;

		virtual	EventResult	ReceiveEvent(ViewCasterUpdateEvent* evn, void* dispatcher);

		void Register() noexcept;

	private:

		HeightEdit() noexcept {}
		~HeightEdit() noexcept {}

		HeightEdit(const HeightEdit&) = delete;
		HeightEdit(HeightEdit&&) = delete;

		HeightEdit& operator=(const HeightEdit&) = delete;
		HeightEdit& operator=(HeightEdit&&) = delete;

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
