#pragma once

#include "version.h"

#include "settings.h"
#include "hhs.h"
#include "ingame.h"
#include "util.h"

namespace Events {

	using _ActorMediator_ProcessEvent = void* (*)(void*, BSAnimationGraphEvent*, void*);

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
#if CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163
	static RelocAddr<_ActorMediator_ProcessEvent> reloc_ActorMediator_ProcessEvent(0xE21090);
#elif CURRENT_RELEASE_RUNTIME == RUNTIME_VERSION_1_10_980
	static RelocAddr<_ActorMediator_ProcessEvent> reloc_ActorMediator_ProcessEvent(0xC63600);
#elif CURRENT_RELEASE_RUNTIME == RUNTIME_VERSION_1_10_984
	static RelocAddr<_ActorMediator_ProcessEvent> reloc_ActorMediator_ProcessEvent(0xC63990);
#endif
#else
	static RelocAddr<_ActorMediator_ProcessEvent> reloc_ActorMediator_ProcessEvent(0xE752E0);
#endif

	static _ActorMediator_ProcessEvent o_ActorMediator_ProcessEvent;

	enum class AnimationGraphEvent : std::size_t {
		AnimObjLoad = "animobjload"_hash,
		IdleChairSittingNoPerspectiveSwitch = "idlechairsittingnoperspectiveswitch"_hash,
		IdleStop = "idlestop"_hash,
		SoundPlay = "soundplay"_hash,
		InitiateStart = "initiatestart"_hash,
		ReevaluateGraphState = "reevaluategraphstate"_hash
	};

	enum class MenuName : std::size_t {

		LooksMenu = "looksmenu"_hash,
		TerminalMenu = "terminalmenu"_hash
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
		public PlayerInputHandler,
		public util::Singleton<Dispatcher> { 
		friend class util::Singleton<Dispatcher>; 

	public:

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

		void EnableAllInputs(bool value) noexcept { inputEnabled = value; }

		[[nodiscard]] bool GetInputEnabled() const noexcept { return inputEnabled; }

	private:

		Dispatcher() noexcept = default;
		~Dispatcher() noexcept = default;

		void RegisterInput() noexcept;
		void RegisterAnimationGraphEvent() noexcept;

		template<typename Tevent>
		void AddEvent() noexcept { GetEventDispatcher<Tevent>()->AddEventSink(this); }

		static void AnimObjFirstPerson(TESObjectREFR* refr, bool stop) noexcept;
		static void SwimEvent(TESObjectREFR* refr, bool soundPlay) noexcept;

		static void* ProcessEvent(void*, BSAnimationGraphEvent*, void*);

		template<typename T>
		void LoadEvent(T t) noexcept
		{
			hhs::Map::GetSingleton().visit(hhs::VisitFlags::None, t, [&](hhs::System& sys) {

				auto actor = sys.GetActorPtr();

				sys.DisableFix();

				auto ret = sys.SetHeight();

				if (actor->IsDead()) {
					return sys.ResetHeight();
				}

				auto furniture = sys.GetActorUtil().GetFurnitureReference();

				if (Settings::Ini::GetSingleton().CheckFurnitureBehavior(furniture)) {
					return hhs::Error::Success;
				}

				if (actor->IsSitting()) {

					sys.EnableFix(furniture);

					return sys.Stop();
				}

				if (actor->IsSwimming()) {
					return sys.Swim(true);
				}

				return ret;
			});
		}

		bool inputEnabled{ true };
	};
}
