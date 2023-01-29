#pragma once

#include "version.h"

#include "settings.h"
#include "hhs.h"
#include "ingame.h"

namespace Events {

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
	using _unknownE21090 = void (*)(void*, BSAnimationGraphEvent*);

	static RelocAddr<_unknownE21090> reloc_unknownE21090(0xE21090);

	static _unknownE21090 o_unknownE21090;
#else
	using _unknownE752E0 = void (*)(void*, BSAnimationGraphEvent*);

	static RelocAddr<_unknownE752E0> reloc_unknownE752E0(0xE752E0);

	static _unknownE752E0 o_unknownE752E0;
#endif

	enum class AnimEvents : std::size_t {

		Null = 0,
		AnimObjLoad = 0x9FBED844753AA659,
		AnimObjUnequip = 0x2A7279A7B1D0132A,
		idleChairSittingNoPerspectiveSwitch = 0x981DD818CEDE57D7,
		SoundPlay = 0x8F5D5112961ECB9E,
		sneakState = 0x917422C34FCC9EC8,
		ReevaluateGraphState = 0xFAE82DB0460A23B2,
		initiateStart = 0xABA08FC372322A52
	};

	class Dispatcher :
		public BSTEventSink<TESFurnitureEvent>,
		public BSTEventSink<TESDeathEvent>,
		public BSTEventSink<TESEquipEvent>,
		public BSTEventSink<TESRaceSwitchEvent>,
		public BSTEventSink<TESObjectLoadedEvent>,
		public BSTEventSink<TESLoadGameEvent>,
		public BSTEventSink<TESCellAttachEvent>,
		public BSTEventSink<TESInitScriptEvent>,
		public BSTEventSink<MenuOpenCloseEvent>,
		public PlayerInputHandler { 

	public:

		[[nodiscard]] static Dispatcher& GetInstance() noexcept { return instance; }

		virtual void OnButtonEvent(ButtonEvent* inputEvent);

		virtual EventResult ReceiveEvent(TESFurnitureEvent* evn, void* dispatcher);
		virtual EventResult ReceiveEvent(TESDeathEvent* evn, void* dispatcher);
		virtual EventResult ReceiveEvent(TESEquipEvent* evn, void* dispatcher);
		virtual EventResult ReceiveEvent(TESRaceSwitchEvent* evn, void* dispatcher);
		virtual EventResult ReceiveEvent(TESObjectLoadedEvent* evn, void* dispatcher);
		virtual EventResult ReceiveEvent(TESLoadGameEvent* evn, void* dispatcher);
		virtual EventResult ReceiveEvent(TESCellAttachEvent* evn, void* dispatcher);
		virtual EventResult ReceiveEvent(TESInitScriptEvent* evn, void* dispatcher);
		virtual EventResult ReceiveEvent(MenuOpenCloseEvent* evn, void* dispatcher);

		void Register() noexcept;

		void EnableAllInputs(const bool& value) noexcept { inputEnabled = value; }

		[[nodiscard]] const bool& GetInputEnabled() const noexcept { return inputEnabled; }

	private:

		Dispatcher() noexcept {}
		~Dispatcher() noexcept {}

		Dispatcher(const Dispatcher&) = delete;
		Dispatcher(Dispatcher&&) = delete;

		Dispatcher& operator=(const Dispatcher&) = delete;
		Dispatcher& operator=(Dispatcher&&) = delete;

		void RegisterInput() noexcept;
		void RegisterAnimationGraphEvent() noexcept;

		template<typename Tevent>
		void AddEvent() noexcept { GetEventDispatcher<Tevent>()->AddEventSink(&instance); }

		static void AnimObjFirstPerson(hhs::System& sys, const bool& stop) noexcept;

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		static void unknownE21090(void*, BSAnimationGraphEvent*);
#else
		static void unknownE752E0(void*, BSAnimationGraphEvent*);
#endif

		template<typename T>
		void LoadEvent(T t) noexcept
		{
			hhs::Map::GetInstance().visit(false, t, [&](hhs::System& sys) {

				auto actor = sys.GetActorPtr();

				sys.DisableFix();

				auto ret = sys.SetHeight();

				if (actor->IsDead())
					return sys.ResetHeight();

				auto furniture = sys.GetActorUtil().GetFurnitureReference();

				if (Settings::Ini::GetInstance().CheckFurnitureBehavior(furniture))
					return hhs::Error::Success;

				if (actor->IsSitting()) {

					sys.EnableFix(furniture);

					return sys.Stop();
				}

				if (actor->IsSwimming())
					return sys.Swim(true);

				return ret;
			});
		}

		bool inputEnabled{ true };

		void* g_moduleHandle{ nullptr };

		static Dispatcher instance;
	};
}
