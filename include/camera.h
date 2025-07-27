#pragma once

#include "version.h"
#include "externals.h"
#include "settings.h"
#include "node.h"
#include "util.h"

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

	class Player : 
		public util::Singleton<Player> {
		friend class util::Singleton<Player>;

	public:

		void SetApplyCameraNodeAnimations(bool value) noexcept;

		void Init() noexcept;
		void SetCameraHeight(Actor* akActor, float height = ZeroValue) noexcept; 
		void ResetCameraSettings() noexcept;

		[[nodiscard]] bool IsCameraNodeAnimations() noexcept;
		[[nodiscard]] std::int32_t GetCameraState() const noexcept;

	private:

		Player() noexcept = default;
		~Player() noexcept = default;

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
