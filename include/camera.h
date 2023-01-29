#pragma once

#include "version.h"
#include "externals.h"
#include "settings.h"
#include "node.h"

namespace Camera {

	class Player {

	public:

		[[nodiscard]] static Player& GetInstance() noexcept { return instance; }

		[[nodiscard]] bool IsCameraNodeAnimations() noexcept;

		void Init() noexcept;
		void SetCameraHeight(Actor* akActor, const float height = MinValue) noexcept; 

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

		std::vector<std::pair<const std::string, double>> Camera3rdSettings = {
			{ "fOverShoulderPosZ:Camera", 0.0f },
			{ "fOverShoulderMeleeCombatPosZ:Camera", 0.0f },
			{ "fOverShoulderCombatPosZ:Camera", 0.0f }
		};

		[[nodiscard]] bool Get3rdCameraHeight() noexcept;

		static Player instance;
	};
}
