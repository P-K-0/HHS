#pragma once

#include "hhs.h"
#include "util.h"

namespace Translations {

	enum class LangID : std::uint32_t {

		FileCreated,
		FileDeleted,
		TestHeight,
		ErrorCreated,
		ErrorDeleted,
		ErrorTest,
		ErrorUnknown,
		ErrorComOverride,
		ErrorRuntime,
		ErrorActorDisabled,
		ErrorOverride,
		ErrorSetHeight,
		ErrorRace,
		ErrorHeightTxt,
		ErrorArguments,
		Activated,
		Deactivated,
		PlayerRefr,
		CrossHairRefr
	};

	class Lang :
		public util::Singleton<Lang> {
		friend class util::Singleton<Lang>;

	public:

		[[nodiscard]] bool Load() noexcept;
		[[nodiscard]] const std::string_view& operator[](LangID id) const noexcept { return ids[static_cast<std::uint32_t>(id)]; }

	private:

		Lang() noexcept = default;
		~Lang() noexcept = default;

		std::array<std::string_view, 19> ids = {
			"$HHS_MSG_FILECREATED" , 
			"$HHS_MSG_FILEDELETED",
			"$HHS_MSG_TESTHEIGHT",
			"$HHS_MSG_ERRORCREATED", 
			"$HHS_MSG_ERRORDELETED", 
			"$HHS_MSG_ERRORTEST",
			"$HHS_MSG_ERRORUNKNOWN", 
			"$HHS_MSG_ERRORCOMOVERRIDE", 
			"$HHS_MSG_ERRORRUNTIME",
			"$HHS_MSG_ERRORACTORDISABLED", 
			"$HHS_MSG_ERROROVERRIDE", 
			"$HHS_MSG_ERRORSETHEIGHT", 
			"$HHS_MSG_ERRORRACE",
			"$HHS_MSG_ERRORTXT", 
			"$HHS_MSG_ERRORARGUMENTS",
			"$HHS_MSG_ACTIVATED",
			"$HHS_MSG_DEACTIVATED",
			"$HHS_MSG_PLAYER_REFR",
			"$HHS_MSG_CROSSHAIR_REFR"
		};	
	};
}
