#pragma once

#include "version.h"
#include "externals.h"
#include "settings.h"
#include "node.h"

namespace Camera {

	using SetINIFloat = void(*)(VirtualMachine*, std::uint64_t, void*, BSFixedString*, float);

#if CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163
	static RelocAddr<SetINIFloat> SetINIFloat_Internal(0x014520A0);
#elif CURRENT_RELEASE_RUNTIME == RUNTIME_VERSION_1_10_980
	static RelocAddr<SetINIFloat> SetINIFloat_Internal(0x01105FF0);
#elif CURRENT_RELEASE_RUNTIME == RUNTIME_VERSION_1_10_984
	static RelocAddr<SetINIFloat> SetINIFloat_Internal(0x011063C0);
#endif

	static SetINIFloat o_SetINIFloat;

	constexpr auto ApplyCameraNodeAnimations = "bApplyCameraNodeAnimations:Camera";

	class Player {

	public:

		[[nodiscard]] static Player& GetSingleton() noexcept {
			static Player instance;
			return instance;
		}

		[[nodiscard]] bool IsCameraNodeAnimations() noexcept;
		void SetApplyCameraNodeAnimations(bool value) noexcept;

		void Init() noexcept;
		void SetCameraHeight(Actor* akActor, float height = ZeroValue) noexcept; 
		void ResetCameraSettings() noexcept;

		[[nodiscard]] std::int32_t GetCameraState() const noexcept;

	private:

		Player(const Player&) = delete;
		Player(Player&&) = delete;

		Player& operator=(const Player&) = delete;
		Player& operator=(Player&&) = delete;

		Player() noexcept {}
		~Player() noexcept {}

		Node::Transform Camera1st;

		bool isCameraNodeAnimations{};

		float height1st{ ZeroValue };
		float height3rd{ ZeroValue };

		void Hook() noexcept;

		static std::vector<std::pair<const std::string, double>> Camera3rdSettings;

		static void SetIniFloat(VirtualMachine*, std::uint64_t, void*, BSFixedString*, float);

		[[nodiscard]] bool Get3rdCameraHeight() noexcept;
	};
}
