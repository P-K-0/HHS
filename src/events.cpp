
#include "events.h"

namespace Events {

	void Dispatcher::OnButtonEvent(ButtonEvent* inputEvent)
	{
		if (!inputEvent || !inputEnabled) {
			return;
		}

		std::uint32_t dev = inputEvent->deviceType;
		bool isDown = inputEvent->isDown == 1.0f && inputEvent->timer == 0.0f;
		std::uint32_t keycode = inputEvent->keyMask;

		if (!isDown || dev != InputEvent::kDeviceType_Keyboard || keycode == 0) {
			return;
		}

		auto& settings = Settings::Ini::GetSingleton();

		if (keycode == settings.Get_iKeyStartStopPlayer()) {

			hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, PlayerID, [&](hhs::System& sys) {

				if (sys.IsStop()) {
					return sys.Start();
				}

				return sys.Stop(true);
			});
		}

		if (keycode == settings.Get_iKeyStartStopAll()) {

			VisitCell([&](TESObjectREFR* refr) {

				hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, refr, [&](hhs::System& sys) {

					if (sys.IsStop()) {
						return sys.Start();
					}

					return sys.Stop(true);
				});
			});
		}

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME

		auto& ingame = InGame::HeightEdit::GetSingleton();

		if (keycode == settings.Get_iKeyActivateEdit()) {
			ingame.OnKeyPress(InGame::Key::Activate);
		}

		if (keycode == settings.Get_iKeyCreateHeight()) {
			ingame.OnKeyPress(InGame::Key::Create);
		}

		if (keycode == settings.Get_iKeyDeleteHeight()) {
			ingame.OnKeyPress(InGame::Key::Delete);
		}

		if (keycode == settings.Get_iKeyIncrementHeight()) {
			ingame.OnKeyPress(InGame::Key::Increment);
		}

		if (keycode == settings.Get_iKeyDecrementHeight()) {
			ingame.OnKeyPress(InGame::Key::Decrement);
		}

		if (keycode == settings.Get_iKeyChangeRefr()) {
			ingame.OnKeyPress(InGame::Key::ChangeReference);
		}

#endif
	}

	EventResult Dispatcher::ReceiveEvent(TESFurnitureEvent* evn, void* dispatcher)
	{
		if (!evn || !evn->actor) {
			return kEvent_Continue;
		}

		hhs::Map::GetSingleton().visit(hhs::VisitFlags::None, evn->actor, [&](hhs::System& sys) {

			if (!sys.HasHeight()) {
				return hhs::Error::Success;
			}

			if (evn->isGettingUp) {

				sys.DisableFix();

				return sys.SetHeight();
			}
			
			if (Settings::Ini::GetSingleton().CheckFurnitureBehavior(evn->furniture)) {
				return hhs::Error::Success;
			}

			auto ret = sys.Stop();

			sys.EnableFix(evn->furniture);

			return ret;
		});

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(TESDeathEvent * evn, void * dispatcher)
	{
		if (!evn || !evn->source) {
			return kEvent_Continue;
		}

		hhs::Map::GetSingleton().visit(hhs::VisitFlags::None, evn->source, [&](hhs::System& sys) {
			return sys.ResetHeight();
		});

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(TESEquipEvent * evn, void * dispatcher)
	{
		if (!evn || !evn->source || evn->ObjectID == 0) {
			return kEvent_Continue;
		}

		std::uint32_t slot{ Actors::GetSlotMaskByID(evn->ObjectID) };

		if ((slot & Settings::Ini::GetSingleton().Get_uSlotFlags()) == InvalidSlot) {
			return kEvent_Continue;
		}

		bool isEquipped{ static_cast<bool>(evn->equipped) };

		hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, evn->source, [&](hhs::System& sys) {

			auto actor = sys.GetActorPtr();

			if (actor->IsDead() || actor->IsSitting() || actor->IsSwimming() || sys.IsAAF()) {
				return hhs::Error::Success;
			}

			if (!sys.HasOverride()) {
				return sys.SetHeight(slot, evn->ObjectID, isEquipped);
			}

			if (sys.HasOverride() && !isEquipped && evn->ObjectID == sys.GetActorUtil().GetIDFromScriptKwrd()) {
				return sys.SetHeight(slot, evn->ObjectID, isEquipped);
			}

			return hhs::Error::Success;
		});

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(TESRaceSwitchEvent * evn, void * dispatcher)
	{
		if (!evn || !evn->source) {
			return kEvent_Continue;
		}

		hhs::Map::GetSingleton().visit(hhs::VisitFlags::None, evn->source, [&](hhs::System& sys) {

			auto actor = sys.GetActorPtr();

			if (!actor->IsDead() &&
				!actor->IsSitting() &&
				!actor->IsSwimming()) {

				return sys.SetHeight();
			}

			return hhs::Error::Success;
		});

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(TESObjectLoadedEvent * evn, void * dispatcher)
	{
		if (!evn || !evn->loaded) {
			return kEvent_Continue;
		}

		LoadEvent(static_cast<std::uint32_t>(evn->formId));

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(TESLoadGameEvent * evn, void * dispatcher)
	{
		if (!evn) {
			return kEvent_Continue;
		}

		VisitCell([&](TESObjectREFR* refr) {
			LoadEvent(refr);
		});

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(TESCellAttachEvent* evn, void * dispatcher)
	{
		if (!evn || !evn->source || !evn->attached) {
			return kEvent_Continue;
		}

		hhs::Map::GetSingleton().visit(hhs::VisitFlags::None, evn->source, [&](hhs::System& sys) {

			auto actor = sys.GetActorPtr();

			if (!actor->IsDead() && 
				!actor->IsSitting() &&
				!actor->IsSwimming() &&
				!sys.HasHeight() &&
				!sys.IsStop() &&
				!sys.IsAAF() &&
				!sys.IsSkip()) {

				auto ret = sys.SetHeight();

				sys.Skip(true);

				return ret;
			}

			return hhs::Error::Success;
		});

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(TESInitScriptEvent* evn, void* dispatcher)
	{
		if (!evn || !evn->reference) {
			return kEvent_Continue;
		}

		LoadEvent(evn->reference);

		return kEvent_Continue;
	}

	void Dispatcher::AnimObjFirstPerson(TESObjectREFR* refr, bool stop) noexcept
	{
		if (refr != *g_player) {
			return;
		}

		if (!Settings::Ini::GetSingleton().Get_bEnableFirstPersonAnim()) {
			return;
		}

		if (Camera::Player::GetSingleton().GetCameraState() != PlayerCamera::kCameraState_FirstPerson) {
			return;
		}

		hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, refr, [&](hhs::System& sys) {

			auto actor = sys.GetActorPtr();
			auto furniture = sys.GetActorUtil().GetFurnitureReference();

			if (actor->IsDead() || actor->IsSitting() || !sys.HasHeight()) {
				return hhs::Error::Success;
			}

			if (stop && furniture) {

				sys.Stop();
		
				return hhs::Error::Success;
			}

			sys.Start();

			return hhs::Error::Success;
		});
	}

	void Dispatcher::SwimEvent(TESObjectREFR* refr, bool soundPlay) noexcept
	{
		if (!Settings::Ini::GetSingleton().Get_bEnableSwimming()) {
			return;
		}

		hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, refr, [&](hhs::System& sys) {

			auto actor = sys.GetActorPtr();

			if (actor->IsDead() || actor->IsSitting() || !sys.HasHeight()) {
				return hhs::Error::Success;
			}

			if (actor->IsSwimming() && !sys.IsSwimming() && soundPlay) {
				return sys.Swim(true);
			}

			if (sys.IsSwimming() && !soundPlay) {
				return sys.Swim(false);
			}

			return hhs::Error::Success;
		});
	}

	void* Dispatcher::ProcessEvent(void* a_this, BSAnimationGraphEvent* graph, void* dispatcher)
	{
		if (graph && graph->refr && graph->eventName && graph->eventName.c_str()) {

			auto key = hash<AnimationGraphEvent>(graph->eventName.c_str());

			//if (graph->refr->formID == PlayerID) {
			//	_DMESSAGE("%.8X %s %I64X", graph->refr->formID, graph->eventName.c_str(), key);
			//}

			switch (key) {

			case AnimationGraphEvent::AnimObjLoad:

				AnimObjFirstPerson(graph->refr, true);
				break;

			case AnimationGraphEvent::IdleChairSittingNoPerspectiveSwitch:
			case AnimationGraphEvent::IdleStop:

				AnimObjFirstPerson(graph->refr, false);
				break;

			case AnimationGraphEvent::SoundPlay:

				SwimEvent(graph->refr, true);
				break;

			//case AnimationGraphEvent::ReevaluateGraphState:
			case AnimationGraphEvent::InitiateStart:

				SwimEvent(graph->refr, false);
				break;

			}
		}

		return o_ActorMediator_ProcessEvent(a_this, graph, dispatcher);
	}

	EventResult Dispatcher::ReceiveEvent(MenuOpenCloseEvent* evn, void* dispatcher)
	{
		if (!evn || !evn->menuName.c_str()) {
			return kEvent_Continue;
		}

		auto key = hash<MenuName>(evn->menuName.c_str());

		switch (key) {

		case MenuName::LooksMenu:

			if (!Settings::Ini::GetSingleton().Get_bLooksmenu()) {

				VisitCell([&](TESObjectREFR* refr) {

					hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, refr, [&](hhs::System& sys) {

						auto actor = sys.GetActorPtr();

						if (!actor->IsDead() && !actor->IsSitting() && !actor->IsSwimming() && !sys.IsAAF()) {

							return evn->isOpen ? sys.Stop() : sys.Start();
						}

						return hhs::Error::Success;
					});
				});
			}

			break;

		case MenuName::TerminalMenu:

			if (Camera::Player::GetSingleton().GetCameraState() == PlayerCamera::kCameraState_FirstPerson) {

				hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, PlayerID, [&](hhs::System& sys) {

					auto actor = sys.GetActorPtr();

					if (!actor->IsDead() && !sys.IsAAF()) {

						return evn->isOpen ? sys.Stop() : sys.Start();
					}

					return hhs::Error::Success;
				});
			}

			break;

		default:

			return kEvent_Continue;
		}

		inputEnabled = !evn->isOpen;

		return kEvent_Continue;
	}

	void Dispatcher::Register() noexcept
	{
		static bool registered{};

		if (registered) {
			return;
		}

		AddEvent<TESFurnitureEvent>();
		AddEvent<TESDeathEvent>();
		AddEvent<TESEquipEvent>();
		AddEvent<TESRaceSwitchEvent>();
		AddEvent<TESObjectLoadedEvent>();
		AddEvent<TESLoadGameEvent>();
		AddEvent<TESCellAttachEvent>();
		AddEvent<TESInitScriptEvent>();

		auto ui = *g_ui;

		if (ui) {
			ui->menuOpenCloseEventSource.AddEventSink(this);
		}

		_DMESSAGE("Events registered successfully!");

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		RegisterInput();
#endif

		RegisterAnimationGraphEvent();
	}

	void Dispatcher::RegisterInput() noexcept
	{
		static bool registered{};

		if (registered) {
			return;
		}
		
		auto playerCtrl = (*g_playerControls);

		if (!playerCtrl) {
			return;
		}

		tArray<PlayerInputHandler*>* inputEvents = std::addressof(playerCtrl->inputEvents1);
		PlayerInputHandler* inputHandler = this;

		if (!inputEvents) {
			return;
		}

		auto idx = inputEvents->GetItemIndex(inputHandler);

		if (idx == -1) {

			inputEvents->Push(inputHandler);

			registered = true;

			_DMESSAGE("Input events registered successfully!");
		}

		return;
	}

	void Dispatcher::RegisterAnimationGraphEvent() noexcept
	{
		static bool registered{};

		if (registered) {
			return;
		}

		/*

		function address 0xE21090

		arg_0 = qword ptr 8h
		arg_8 = qword ptr 10h
		arg_10 = qword ptr 18h

		E21090	mov		[rsp + arg_0], rbx
		E21095 	mov		[rsp + arg_8], rbp
		E2109A	mov		[rsp + arg_10], rsi
		E2109F	push	rdi
		E210A0	sub 	rsp, 20h
		E210A4	mov		rbp, rdx
		E210A7	mov		rdi, rcx

		rcx = class ActorMediator
		rdx = BSTEventSource<BSAnimationGraphEvent>
		r8 = BSAnimationGraphEvent

		*/

		struct AnimationGraphEvent_Code : Xbyak::CodeGenerator {

			AnimationGraphEvent_Code(void* buf)
				: Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(ptr[rsp + 0x08], rbx);

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);

				dq(reloc_ActorMediator_ProcessEvent.GetUIntPtr() + 0x05);
			}
		};

		Trampoline::GetSingleton().Alloc<AnimationGraphEvent_Code>(o_ActorMediator_ProcessEvent);

		if (g_branchTrampoline.Write5Branch(reloc_ActorMediator_ProcessEvent.GetUIntPtr(), (uintptr_t)ProcessEvent)) {

			_DMESSAGE("Animation Graph Event registered successfully!");

			registered = true;
		}	
	}
}
