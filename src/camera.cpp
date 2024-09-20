
#include "camera.h"
#include "hhs.h"

namespace Camera {

	std::vector<std::pair<const std::string, double>> Player::Camera3rdSettings = {
		{ "fOverShoulderPosZ:Camera", 0.0f },
		{ "fOverShoulderMeleeCombatPosZ:Camera", 0.0f },
		{ "fOverShoulderCombatPosZ:Camera", 0.0f }
	};

	void Player::SetIniFloat(VirtualMachine* vm, std::uint64_t unk0, void* unk1, BSFixedString* name, float value)
	{
		auto val{ value };

		if (Settings::Ini::GetInstance().Get_bEnableCustomCameraPatch() && name) {

			for (auto& stgCamera : Camera3rdSettings) {

				if (_strcmpi(name->c_str(), stgCamera.first.c_str()) == 0) {

					hhs::Map::GetInstance().visit(true, PlayerID, [&](hhs::System& sys) {

						stgCamera.second = val;

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
		for (auto& cam : Camera3rdSettings) {
			cam.second = 0.0f;
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

			for (auto& stgCamera : Camera3rdSettings) {

				auto setting = GetINISetting(stgCamera.first.c_str());

				if (setting && setting->GetDouble(&val)) {
					stgCamera.second = val;					
				}
			}

			isCameraNodeAnimations = true;

			return true;
		}

		return isCameraNodeAnimations;
	}

	void Player::SetCameraHeight(Actor* actor, const float height) noexcept
	{
		if (!actor || actor != (*g_player) || !isCameraNodeAnimations) {
			return;
		}

		auto& settings = Settings::Ini::GetInstance();

		float h{ settings.Get_bEnableDynamicCamera() ? height : MinValue };
		float h1st{ settings.Get_bEnable1stCamera() ? h : MinValue };
		float h3rd{ settings.Get_bEnable3rdCamera() ? h : MinValue };

		if (height1st == h1st && height3rd == h3rd) {
			return;
		}

		height1st = h1st;
		height3rd = h3rd;

		if (h1st > MinValue) {

			Camera1st.SetTransform(ComOverride, Node::Flags::PosZ, h1st);
		}
		else {

			Camera1st.ResetTransform(ComOverride, Node::Flags::PosZ);
		}

		for (const auto& cam : Camera3rdSettings) {

			auto setting = GetINISetting(cam.first.c_str());

			if (setting) {
				setting->SetDouble(h3rd > MinValue ? cam.second + h3rd : cam.second);
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

	const std::int32_t Player::GetCameraState() const noexcept
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

	Player Player::instance;
}
