
#include "events.h"

namespace Events {

	void Dispatcher::OnButtonEvent(ButtonEvent* inputEvent)
	{
		if (!inputEvent || !inputEnabled)
			return;

		std::uint32_t dev = inputEvent->deviceType;
		bool isDown = inputEvent->isDown == 1.0f && inputEvent->timer == 0.0f;
		std::uint32_t keycode = inputEvent->keyMask;

		if (!isDown || dev != InputEvent::kDeviceType_Keyboard || keycode == 0)
			return;

		auto& settings = Settings::Ini::GetInstance();

		if (keycode == settings.Get_iKeyStartStopPlayer()) {

			hhs::Map::GetInstance().visit(true, PlayerID, [&](hhs::System& sys) {

				if (sys.IsStop()) {

					return sys.Start();
				}

				return sys.Stop(true);
			});
		}

		if (keycode == settings.Get_iKeyStartStopAll()) {

			VisitCell([&](TESObjectREFR* refr) {

				hhs::Map::GetInstance().visit(true, refr, [&](hhs::System& sys) {

					if (sys.IsStop()) {

						return sys.Start();
					}

					return sys.Stop(true);
				});
			});
		}

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME

		auto& ingame = InGame::HeightEdit::GetInstance();

		if (keycode == settings.Get_iKeyActivateEdit()) 
			ingame.OnKeyPress(InGame::Key::Activate);

		if (keycode == settings.Get_iKeyCreateHeight()) 
			ingame.OnKeyPress(InGame::Key::Create);

		if (keycode == settings.Get_iKeyDeleteHeight()) 
			ingame.OnKeyPress(InGame::Key::Delete);

		if (keycode == settings.Get_iKeyIncrementHeight()) 
			ingame.OnKeyPress(InGame::Key::Increment);

		if (keycode == settings.Get_iKeyDecrementHeight()) 
			ingame.OnKeyPress(InGame::Key::Decrement);

		if (keycode == settings.Get_iKeyChangeRefr())		
			ingame.OnKeyPress(InGame::Key::ChangeReference);

#endif
	}

	EventResult Dispatcher::ReceiveEvent(TESFurnitureEvent* evn, void* dispatcher)
	{
		if (!evn || !evn->actor)
			return kEvent_Continue;

		hhs::Map::GetInstance().visit(false, evn->actor, [&](hhs::System& sys) {

			if (!sys.HasHeight())
				return hhs::Error::Success;

			if (evn->isGettingUp) {

				sys.DisableFix();

				return sys.SetHeight();
			}
			
			if (Settings::Ini::GetInstance().CheckFurnitureBehavior(evn->furniture))
				return hhs::Error::Success;

			auto ret = sys.Stop();

			sys.EnableFix(evn->furniture);

			return ret;
		});

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(TESDeathEvent * evn, void * dispatcher)
	{
		if (!evn || !evn->source)
			return kEvent_Continue;

		hhs::Map::GetInstance().visit(false, evn->source, [&](hhs::System& sys) {

			return sys.ResetHeight();
		});

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(TESEquipEvent * evn, void * dispatcher)
	{
		if (!evn || !evn->source || evn->ObjectID == 0)
			return kEvent_Continue;

		std::uint32_t slot{ Actors::GetSlotMaskByID(evn->ObjectID) };

		if ((slot & Settings::Ini::GetInstance().Get_uSlotFlags()) == InvalidSlot)
			return kEvent_Continue;

		bool isEquipped{ static_cast<bool>(evn->equipped) };

		hhs::Map::GetInstance().visit(true, evn->source, [&](hhs::System& sys) {

			auto actor = sys.GetActorPtr();

			if (actor->IsDead() || actor->IsSitting() || actor->IsSwimming() || sys.IsAAF())
				return hhs::Error::Success;

			if (!sys.HasOverride())
				return sys.SetHeight(slot, evn->ObjectID, isEquipped);

			if (sys.HasOverride() && !isEquipped && evn->ObjectID == sys.GetActorUtil().GetIDFromScriptKwrd())
				return sys.SetHeight(slot, evn->ObjectID, isEquipped);
				
			return hhs::Error::Success;
		});

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(TESRaceSwitchEvent * evn, void * dispatcher)
	{
		if (!evn || !evn->source)
			return kEvent_Continue;

		hhs::Map::GetInstance().visit(false, evn->source, [&](hhs::System& sys) {

			auto actor = sys.GetActorPtr();

			if (!actor->IsDead() && !actor->IsSitting() && !actor->IsSwimming())
				return sys.SetHeight();

			return hhs::Error::Success;
		});

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(TESObjectLoadedEvent * evn, void * dispatcher)
	{
		if (!evn || !evn->loaded)
			return kEvent_Continue;

		LoadEvent(static_cast<std::uint32_t>(evn->formId));

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(TESLoadGameEvent * evn, void * dispatcher)
	{
		if (!evn)
			return kEvent_Continue;

		VisitCell([&](TESObjectREFR* refr) {

			LoadEvent(refr);
		});

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(TESCellAttachEvent* evn, void * dispatcher)
	{
		if (!evn || !evn->source || !evn->attached)
			return kEvent_Continue;

		hhs::Map::GetInstance().visit(false, evn->source, [&](hhs::System& sys) {

			auto actor = sys.GetActorPtr();

			if (!actor->IsDead() && !actor->IsSitting() && !actor->IsSwimming() && !sys.IsStop() && !sys.IsAAF())
				return sys.SetHeight();

			return hhs::Error::Success;
		});

		return kEvent_Continue;
	}

	EventResult Dispatcher::ReceiveEvent(TESInitScriptEvent* evn, void* dispatcher)
	{
		if (!evn || !evn->reference)
			return kEvent_Continue;

		LoadEvent(evn->reference);

		return kEvent_Continue;
	}

	void Dispatcher::AnimObjFirstPerson(hhs::System& sys, const bool& stop) noexcept
	{
		if (!sys.GetActorUtil().IsPlayer())
			return;

		if (Camera::Player::GetInstance().GetCameraState() != PlayerCamera::kCameraState_FirstPerson)
			return;

		if (!Settings::Ini::GetInstance().Get_bEnableFirstPersonAnim())
			return;

		if (stop) {

			sys.Stop();
		
			return;
		}

		sys.Start();
	}

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
	void Dispatcher::unknownE21090(void* cls, BSAnimationGraphEvent* graph)
#else
	void Dispatcher::unknownE752E0(void* cls, BSAnimationGraphEvent* graph)
#endif
	{
		if (graph && graph->refr && graph->eventName) {

			const auto id = graph->refr->formID;

			hhs::Map::GetInstance().visit(true, id, [&](hhs::System& sys) {

				auto actor = sys.GetActorPtr();

				if (actor->IsDead() || actor->IsSitting() || !sys.HasHeight())
					return hhs::Error::Success;

				const std::string eventName = graph->eventName.c_str();

				std::size_t key = std::hash<std::string>{}(eventName);

				switch (static_cast<AnimEvents>(key)) {

				case AnimEvents::AnimObjLoad:

					AnimObjFirstPerson(sys, true);

					break;

				case AnimEvents::idleChairSittingNoPerspectiveSwitch:

					AnimObjFirstPerson(sys, false);

					break;

				case AnimEvents::SoundPlay:

					if (!Settings::Ini::GetInstance().Get_bEnableSwimming())
						return hhs::Error::Success;

					if (actor->IsSwimming() && !sys.IsSwimming())
						return sys.Swim(true);

					if (!actor->IsSwimming() && sys.IsSwimming())
						return sys.Swim(false);

					break;

				default:

					if (!actor->IsSwimming() && sys.IsSwimming())
						return sys.Swim(false);

					break;
				}

				return hhs::Error::Success;
			});
		}

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		o_unknownE21090(cls, graph);
#else
		o_unknownE752E0(cls, graph);
#endif
	}

	EventResult Dispatcher::ReceiveEvent(MenuOpenCloseEvent* evn, void* dispatcher)
	{
		if (!evn || !Settings::Ini::GetInstance().Get_bLooksmenu())
			return kEvent_Continue;

		if (std::string(evn->menuName.c_str()) != "LooksMenu")
			return kEvent_Continue;

		inputEnabled = !evn->isOpen;

		VisitCell([&](TESObjectREFR* refr) {

			hhs::Map::GetInstance().visit(true, refr, [&](hhs::System& sys) {

				auto actor = sys.GetActorPtr();

				if (!actor->IsDead() && !actor->IsSitting() && !actor->IsSwimming() && !sys.IsAAF()) {

					return evn->isOpen ? sys.Stop() : sys.Start();
				}

				return hhs::Error::Success;
			});
		});

		return kEvent_Continue;
	}

	void Dispatcher::Register() noexcept
	{
		static bool registered{ false };

		if (registered) 
			return;

		AddEvent<TESFurnitureEvent>();
		AddEvent<TESDeathEvent>();
		AddEvent<TESEquipEvent>();
		AddEvent<TESRaceSwitchEvent>();
		AddEvent<TESObjectLoadedEvent>();
		AddEvent<TESLoadGameEvent>();
		AddEvent<TESCellAttachEvent>();
		AddEvent<TESInitScriptEvent>();

		if ((*g_ui))
			(*g_ui)->menuOpenCloseEventSource.AddEventSink(&instance);

		_DMESSAGE("Events registered successfully!");

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		RegisterInput();
#endif

		RegisterAnimationGraphEvent();
	}

	void Dispatcher::RegisterInput() noexcept
	{
		static bool registered{ false };

		if (registered) 
			return;
		
		auto playerCtrl = (*g_playerControls);

		if (!playerCtrl)
			return;

		tArray<PlayerInputHandler*>* inputEvents = &(playerCtrl->inputEvents1);
		PlayerInputHandler* inputHandler = &instance;

		if (!inputEvents)
			return;

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

		if (registered)
			return;

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

		rcx = class
		rdx = struct BSAnimationGraphEvent

		*/
		
		std::size_t len{ 65536 };

		if (!g_branchTrampoline.Create(len)) {

			_ERROR("Branch Trampoline init error!");

			return;
		}

		if (!g_localTrampoline.Create(len, g_moduleHandle)) {

			_ERROR("Codegen buffer init error!");

			return;
		}

		struct AnimationGraphEvent_Code : Xbyak::CodeGenerator {

			AnimationGraphEvent_Code(void* buf)
				: Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(ptr[rsp + 0x08], rbx);
				mov(ptr[rsp + 0x10], rbp);
				mov(ptr[rsp + 0x18], rsi);
				push(rdi);

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
				dq(reloc_unknownE21090.GetUIntPtr() + 0x10);
#else
				dq(reloc_unknownE752E0.GetUIntPtr() + 0x10);
#endif
			}
		};

		void* codeBuf = g_localTrampoline.StartAlloc();

		AnimationGraphEvent_Code code(codeBuf);

		g_localTrampoline.EndAlloc(code.getCurr());

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		o_unknownE21090 = (_unknownE21090)codeBuf;

		if (g_branchTrampoline.Write5Branch(reloc_unknownE21090.GetUIntPtr(), (uintptr_t)unknownE21090)) {

			_DMESSAGE("Animation Graph Event registered successfully!");

			registered = true;
		}	
#else
		o_unknownE752E0 = (_unknownE752E0)codeBuf;

		if (g_branchTrampoline.Write5Branch(reloc_unknownE752E0.GetUIntPtr(), (uintptr_t)unknownE752E0)) {

			_DMESSAGE("Animation Graph Event registered successfully!");

			registered = true;
		}
#endif
	}

	Dispatcher Dispatcher::instance;
}
