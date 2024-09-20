
#include "hhs.h"

namespace hhs {

	void System::SetActorUtil(Actors::Utility& utility) noexcept
	{
		util = utility;

		SetActor(util.GetActorPtr());
	}

	Error System::SetHeight(const float& Height) noexcept
	{
		auto h{ Height };

		auto ret = SetTransform(ComOverride, Node::Flags::PosZ, h);

		Camera::Player::GetInstance().SetCameraHeight(util.GetActorPtr(), h);

		if (ret == -1) {
			return Error::ComOverride;
		}

		if (ret == -2) {
			return Error::SetHeight;
		}

		height = h;
		skip = isStop = isAAF = isSwimming = false;

		return Error::Success;
	}

	Error System::SetHeight(const std::uint32_t& slot, const std::uint32_t& id, const bool equipped) noexcept
	{
		auto h = util.GetHeightFromWornItem(slot, id, equipped);

		if (h <= MinValue) {
			return ResetHeight();
		}
	
		auto ret = SetTransform(ComOverride, Node::Flags::PosZ, h);

		Camera::Player::GetInstance().SetCameraHeight(util.GetActorPtr(), h);

		if (ret == -1) {
			return Error::ComOverride;
		}

		if (ret == -2) {
			return Error::SetHeight;
		}

		height = h;
		skip = isStop = isAAF = isSwimming = false;

		return Error::Success;
	}

	Error System::ResetHeight() noexcept
	{
		auto ret = ResetTransform(ComOverride, Node::Flags::PosZ);

		Camera::Player::GetInstance().SetCameraHeight(util.GetActorPtr());

		if (ret == -1) {
			return Error::ComOverride;
		}

		if (ret == -2) {
			return Error::SetHeight;
		}

		height = MinValue;
		skip = hasOverride = isStop = isAAF = isSwimming = false;

		return Error::Success;
	}

	Error System::Start() noexcept
	{
		return SetHeight(height);
	}

	Error System::Stop(const bool& stopAAF) noexcept
	{
		auto ret = ResetTransform(ComOverride, Node::Flags::PosZ);

		Camera::Player::GetInstance().SetCameraHeight(util.GetActorPtr());

		if (ret == -1) {
			return Error::ComOverride;
		}

		if (ret == -2) {
			return Error::SetHeight;
		}

		isStop = true;
		isAAF = stopAAF;

		return Error::Success;
	}

	Error System::Swim(const bool& swim) noexcept 
	{
		isSwimming = swim;
		
		if (swim) {
			return Stop();
		}

		return Start();
	}

	void System::DisableFix() noexcept
	{
		for (auto& m : map) {
			ResetTransform(m.first);
		}
	}

	void System::EnableFix(TESObjectREFR* furniture) noexcept
	{
		auto& settings = Settings::Ini::GetInstance();

		if (!settings.Get_bEnableFixes()) {
			return;
		}

		TESForm* form{ nullptr };

		if (!furniture || !(form = furniture->baseForm)) {
			return;
		}

		TESFurniture* furn{ nullptr };

		if (!(furn = DYNAMIC_CAST(form, TESForm, TESFurniture))) {
			return;
		}

		Skeleton::Reader skeleton{ util.GetActorPtr(), util.IsFemale() };

		std::uint32_t version{};

		if (!skeleton || !skeleton.GetExtraData(ExtraDataSAF, version)) {
			return;
		}

		auto& fixes = Fixes::Preset::GetInstance();

		if (settings.Get_bEnableReloadFixes()) {
			fixes.Load();
		}

		if (fixes.GetSAFVersion() < version) {
			return;
		}

		bool ret{};

		for (std::uint32_t idx{}; idx < furn->keywordForm.numKeywords && !ret; idx++) {

			auto keywrd = furn->keywordForm.keywords[idx];

			if (keywrd) {

				bool noStop{};

				ret = fixes.GetPresetValues(keywrd->keyword.c_str(), noStop, [&](const Fixes::Values& values) {
					SetTransform(values.node, values.flags, values.value * (values.mulheight ? height : 1.0f));
				});

				if (noStop) {
					Start();
				}
			}
		}
	}

	Map Map::instance;
}
