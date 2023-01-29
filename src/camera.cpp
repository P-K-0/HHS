
#include "camera.h"

namespace Camera {

	void Player::Init() noexcept
	{
		Camera1st.SetActor(*g_player);
		Camera1st.SetFirstPerson(true);

		Get3rdCameraHeight();
	}

	bool Player::IsCameraNodeAnimations() noexcept
	{
		Setting* stg = GetINISetting("bApplyCameraNodeAnimations:Camera");
		double isApply{};

		if (stg && stg->GetDouble(&isApply) && isApply)
			return false;

		return true;
	}

	bool Player::Get3rdCameraHeight() noexcept
	{
		if (IsCameraNodeAnimations() && !isCameraNodeAnimations) {

			double val{};

			for (auto& stgCamera : Camera3rdSettings) {

				Setting* setting = GetINISetting(stgCamera.first.c_str());

				if (setting && setting->GetDouble(&val))
					stgCamera.second = val;					
			}

			isCameraNodeAnimations = true;

			return true;
		}

		return isCameraNodeAnimations;
	}

	void Player::SetCameraHeight(Actor* actor, const float height) noexcept
	{
		if (!actor || actor != *g_player || !isCameraNodeAnimations)
			return;

		auto& settings = Settings::Ini::GetInstance();

		float h{ settings.Get_bEnableDynamicCamera() ? height : MinValue };
		float h1st{ settings.Get_bEnable1stCamera() ? h : MinValue };
		float h3rd{ settings.Get_bEnable3rdCamera() ? h : MinValue };

		if (height1st == h1st && height3rd == h3rd)
			return;

		height1st = h1st;
		height3rd = h3rd;

		if (h1st > MinValue) {

			Camera1st.SetTransform(ComOverride, Node::Flags::PosZ, h1st);
		}
		else {

			Camera1st.ResetTransform(ComOverride, Node::Flags::PosZ);
		}

		for (const auto& stgCamera : Camera3rdSettings) {

			Setting* setting = GetINISetting(stgCamera.first.c_str());

			if (setting) {

				setting->SetDouble(h3rd > MinValue ? h3rd : stgCamera.second);
			}
		}

		PlayerCharacter* player = *g_player;
		PlayerCamera* playerCamera = *g_playerCamera;

		if (playerCamera && player) {

			ThirdPersonState* thirdPersonCamera = DYNAMIC_CAST(playerCamera->cameraStates[PlayerCamera::kCameraState_ThirdPerson2],
				TESCameraState, ThirdPersonState);

			if (thirdPersonCamera)
				thirdPersonCamera->UpdateMode(player->actorState.IsWeaponDrawn());
		}

		return;
	}

	const std::int32_t Player::GetCameraState() const noexcept
	{
		PlayerCamera* playerCamera = *g_playerCamera;

		if (playerCamera)
			for (int i = 0; i < PlayerCamera::kNumCameraStates; i++)
				if (playerCamera->cameraState == playerCamera->cameraStates[i])
					return i;

		return -1;
	}

	Player Player::instance;
}
