
#include "camera.h"
#include "hhs.h"

namespace Camera {

	void Player::SetIniFloat(VirtualMachine* vm, std::uint64_t unk0, void* unk1, BSFixedString* name, float value)
	{
		auto val{ value };

		if (Settings::Ini::GetSingleton().GetEnableCustomCameraPatch() && name) {

			for (auto& camSetting : cam3rdSettings) {

				if (BSCompi(*name, camSetting.name.data())) {

					hhs::Map::GetSingleton().visit(hhs::VisitFlags::Override, PlayerID, [&](hhs::System& sys) {

						camSetting.value = val;

						val += sys.GetHeight();

						return hhs::Error::Success;
					});

					break;
				}
			}
		}

		o_SetINIFloat(vm, unk0, unk1, name, val);
	}

	void Player::Hook() noexcept
	{
		static bool hooked{};

		if (hooked) {
			return;
		}

		struct SetINIFloat_Code : Xbyak::CodeGenerator {

			SetINIFloat_Code(void* buf)
				: Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(ptr[rsp + 0x08], rbx);

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(SetINIFloat_Internal.GetUIntPtr() + 0x05);
			}
		};

		Trampoline::GetSingleton().Alloc<SetINIFloat_Code>(o_SetINIFloat);

		if (g_branchTrampoline.Write5Branch(SetINIFloat_Internal.GetUIntPtr(), (std::uintptr_t)SetIniFloat)) {

			hooked = true;
		}
	}

	void Player::Init() noexcept
	{
		Camera1st.SetActor(*g_player);
		Camera1st.SetFirstPerson(true);

		Get3rdCameraHeight();

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		Hook();
#endif
	}
	
	void Player::ResetCameraSettings() noexcept
	{
		for (auto& camSetting : cam3rdSettings) {
			camSetting.value = ZeroValue;
		}
	}

	bool Player::IsCameraNodeAnimations() noexcept
	{
		auto stg = GetINISetting(ApplyCameraNodeAnimations);

		double isApply{};

		if (stg && stg->GetDouble(&isApply) && isApply) {
			return false;
		}

		return true;
	}

	void Player::SetApplyCameraNodeAnimations(bool value) noexcept
	{
		auto stg = GetINISetting(ApplyCameraNodeAnimations);

		if (stg) {
			stg->SetDouble(value ? 1.0f : 0.0f);
		}
	}

	bool Player::Get3rdCameraHeight() noexcept
	{
		if (IsCameraNodeAnimations() && !isCameraNodeAnimations) {

			double val{};

			for (auto& camSetting : cam3rdSettings) {

				auto setting = GetINISetting(camSetting.name.data());

				if (setting && setting->GetDouble(&val)) {
					camSetting.value = val;
				}
			}

			isCameraNodeAnimations = true;

			return true;
		}

		return isCameraNodeAnimations;
	}

	void Player::SetCameraHeight(Actor* actor, float height) noexcept
	{
		if (!actor || actor != (*g_player) || !isCameraNodeAnimations) {
			return;
		}

		auto& settings = Settings::Ini::GetSingleton();

		float h{ settings.GetEnableDynamicCamera() ? height : ZeroValue };
		float h1st{ settings.GetEnable1stCamera() ? h : ZeroValue };
		float h3rd{ settings.GetEnable3rdCamera() ? h : ZeroValue };

		if (height1st == h1st && height3rd == h3rd) {
			return;
		}

		height1st = h1st;
		height3rd = h3rd;

		if (h1st != ZeroValue) {

			Camera1st.SetTransform(ComOverride, Node::Flags::PosZ, h1st);
		}
		else {

			Camera1st.ResetTransform(ComOverride, Node::Flags::PosZ);
		}

		for (const auto& camSetting : cam3rdSettings) {

			auto setting = GetINISetting(camSetting.name.data());

			if (setting) {
				setting->SetDouble(h3rd != ZeroValue ? camSetting.value + h3rd : camSetting.value);
			}
		}

		auto player = (*g_player);
		auto playerCamera = (*g_playerCamera);

		if (playerCamera && player) {

			auto thirdPersonCamera = DYNAMIC_CAST(playerCamera->cameraStates[PlayerCamera::kCameraState_ThirdPerson2], TESCameraState, ThirdPersonState);

			if (thirdPersonCamera) {
				thirdPersonCamera->UpdateMode(player->actorState.IsWeaponDrawn());
			}
		}

		return;
	}

	std::int32_t Player::GetCameraState() noexcept
	{
		auto playerCamera = (*g_playerCamera);

		if (playerCamera) {

			for (int i = 0; i < PlayerCamera::kNumCameraStates; i++) {

				if (playerCamera->cameraState == playerCamera->cameraStates[i]) {
					return i;
				}
			}
		}

		return -1;
	}
}
