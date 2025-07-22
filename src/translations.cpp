
#include "translations.h"

namespace Translations {

	bool Lang::Load() noexcept
	{
		static bool loaded{};

		if (loaded || !g_scaleformManager || !(*g_scaleformManager)->stateBag) {
			return false;
		}

		auto translator = static_cast<BSScaleformTranslator*>((*g_scaleformManager)->stateBag->GetStateAddRef(GFxState::kInterface_Translator));

		if (!translator) {
			return false;
		}

		Translation::ParseTranslation(translator, std::string(Version::ShortName));

		loaded = true;

		return true;
	}
}
