#pragma once

#include "version.h"
#include "externals.h"
#include "settings.h"
#include "node.h"

namespace Camera {

	using SetINIFloat = void(*)(VirtualMachine*, std::uint64_t, void*, BSFixedString*, float);

	static RelocAddr<SetINIFloat> SetINIFloat_Internal(0x014520A0);

	static SetINIFloat o_SetINIFloat;

	constexpr auto ApplyCameraNodeAnimations = "bApplyCameraNodeAnimations:Camera";

	class Player {

	public:

		[[nodiscard]] static Player& GetInstance() noexcept { return instance; }

		[[nodiscard]] bool IsCameraNodeAnimations() noexcept;
		void SetApplyCameraNodeAnimations(bool value) noexcept;

		void Init() noexcept;
		void SetCameraHeight(Actor* akActor, const float height = MinValue) noexcept; 
		void ResetCameraSettings() noexcept;

		[[nodiscard]] const std::int32_t GetCameraState() const noexcept;

	private:

		Player(const Player&) = delete;
		Player(Player&&) = delete;

		Player& operator=(const Player&) = delete;
		Player& operator=(Player&&) = delete;

		Player() noexcept {}
		~Player() noexcept {}

		Node::Transform Camera1st;

		bool isCameraNodeAnimations{};

		float height1st{ MinValue };
		float height3rd{ MinValue };

		void Hook() noexcept;

		static std::vector<std::pair<const std::string, double>> Camera3rdSettings;

		static void SetIniFloat(VirtualMachine*, std::uint64_t, void*, BSFixedString*, float);

		[[nodiscard]] bool Get3rdCameraHeight() noexcept;

		static Player instance;

		void* g_moduleHandle{ nullptr };
	};
}
