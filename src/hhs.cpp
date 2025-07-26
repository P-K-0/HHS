
#include "hhs.h"

namespace hhs {

	Error System::SetHeight(float Height) noexcept
	{
		auto h{ Height };

		auto ret = SetTransform(ComOverride, Node::Flags::PosZ, h);

		Camera::Player::GetSingleton().SetCameraHeight(util.GetActorPtr(), h);

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

	Error System::SetHeight(std::uint32_t slot, std::uint32_t id, bool equipped) noexcept
	{
		auto h = util.GetHeightFromWornItem(slot, id, equipped);

		//_DMESSAGE("%s : id: %i equipped: %i h: %f", __FUNCTION__, id, equipped, h);

		if (h == ZeroValue) {
			return ResetHeight();
		}
	
		auto ret = SetTransform(ComOverride, Node::Flags::PosZ, h);

		//_DMESSAGE("%s : ret: %i", __FUNCTION__, ret);

		Camera::Player::GetSingleton().SetCameraHeight(util.GetActorPtr(), h);

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

		Camera::Player::GetSingleton().SetCameraHeight(util.GetActorPtr());

		if (ret == -1) {
			return Error::ComOverride;
		}

		if (ret == -2) {
			return Error::SetHeight;
		}

		height = ZeroValue;
		skip = hasOverride = isStop = isAAF = isSwimming = false;

		return Error::Success;
	}

	Error System::Start() noexcept
	{
		return SetHeight(height);
	}

	Error System::Stop(bool stopAAF) noexcept
	{
		auto ret = ResetTransform(ComOverride, Node::Flags::PosZ);

		Camera::Player::GetSingleton().SetCameraHeight(util.GetActorPtr());

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

	Error System::Swim(bool swim) noexcept 
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
		auto& settings = Settings::Ini::GetSingleton();

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

		auto& fixes = Fixes::Preset::GetSingleton();

		if (settings.Get_bEnableReloadFixes()) {
			fixes.Load();
		}

		if (fixes.GetSAFVersion() < version) {
			return;
		}

		bool ret{};

		for (std::uint32_t idx{}; idx < furn->keywordForm.numKeywords && !ret; idx++) {

			auto keywrd = furn->keywordForm.keywords[idx];

			if (keywrd &&
				keywrd->keyword.c_str()) {

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
}
