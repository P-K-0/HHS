#pragma once

#include "version.h"

#include "hhs.h"

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME

namespace InGame {

	enum class Key {

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

		static [[nodiscard]] HeightEdit& GetInstance() noexcept { return instance; }

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

		static HeightEdit instance;

		std::uint32_t refID{ PlayerID };

		[[nodiscard]] bool Process(const Key& key) noexcept;

		void IncHeight() noexcept;
		void DecHeight() noexcept;
		void ShowError(const hhs::Error& error) noexcept;

		bool enabled{};
	};
}

#endif
