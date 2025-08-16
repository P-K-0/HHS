#pragma once

#include "std_libraries.h"
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

	struct Camera3rdSettings {

		std::string_view name;
		double value{};
	};

	class Player : 
		public util::Singleton<Player> {
		friend class util::Singleton<Player>;

	public:

		void Init() noexcept;
		void SetCameraHeight(Actor* akActor, float height = ZeroValue) noexcept; 
		void ResetCameraSettings() noexcept;

		[[nodiscard]] bool IsCameraNodeAnimations() noexcept;
		void SetApplyCameraNodeAnimations(bool value) noexcept;
		[[nodiscard]] bool IsInFirstCamera() noexcept;

	private:

		Player() noexcept = default;
		~Player() noexcept = default;

		Node::Transform Camera1st;

		bool isCameraNodeAnimations{};

		float height1st{ ZeroValue };
		float height3rd{ ZeroValue };

		static inline Camera3rdSettings cam3rdSettings[] = {
			{ "fOverShoulderPosZ:Camera", ZeroValue },
			{ "fOverShoulderMeleeCombatPosZ:Camera", ZeroValue },
			{ "fOverShoulderCombatPosZ:Camera", ZeroValue }
		};

		void Hook() noexcept;

		static void SetIniFloat(VirtualMachine*, std::uint64_t, void*, BSFixedString*, float);

		[[nodiscard]] bool Get3rdCameraHeight() noexcept;
	};
}
