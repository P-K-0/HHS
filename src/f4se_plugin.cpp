
#include "f4se_plugin.h"

#include "externals.h"
#include "events.h"
#include "papyrus.h"
#include "scaleform.h"
#include "settings.h"
#include "translations.h"
#include "cache.h"
#include "camera.h"
#include "aaf.h"
#include "ingame.h"
#include "jsonparser.h"
#include "fixes.h"

namespace f4se {

	bool Plugin::Query(const F4SEInterface* f4se, PluginInfo* info) noexcept
	{
		iLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Fallout4\\F4SE\\HHS.log");

		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = Version::ShortName;
		info->version = 1;

		hPlug = f4se->GetPluginHandle();

		if (!(isValid = CheckPluginVersion(f4se)))
			return false;

		if (f4se->isEditor) {

			_ERROR("Plugin is not compatible with editor!");

			return false;
		}

		if (!(f4se_papyrus_interface = static_cast<F4SEPapyrusInterface*>(f4se->QueryInterface(kInterface_Papyrus)))) {

			_ERROR("Papyrus Interface error!");

			return false;
		}

		if (!(f4se_task_interface = static_cast<F4SETaskInterface*>(f4se->QueryInterface(kInterface_Task)))) {

			_ERROR("Task Interface error!");

			return false;
		}

		if (!(f4se_msg_interface = static_cast<F4SEMessagingInterface*>(f4se->QueryInterface(kInterface_Messaging)))) {

			_ERROR("Messaging Interface error!");

			return false;
		}

		if (!(f4se_scaleform_interface = static_cast<F4SEScaleformInterface*>(f4se->QueryInterface(kInterface_Scaleform)))) {

			_ERROR("Scaleform Interface error!");

			return false;
		}

		return true;
	}

	bool Plugin::Load(const F4SEInterface* a_f4se) noexcept
	{
		if (f4se_papyrus_interface && f4se_msg_interface && f4se_scaleform_interface) {

			f4se_papyrus_interface->Register(Papyrus::Register);

			f4se_msg_interface->RegisterListener(hPlug, "F4SE", MsgCallback);

			f4se_scaleform_interface->Register(Version::ShortName, Scaleform::Register);

			return true;
		}

		return false;
	}

	bool Plugin::CheckPluginVersion(const F4SEInterface* f4se) noexcept
	{
#if	RUNTIME_VERSION_1_10_138 < CURRENT_RELEASE_RUNTIME
		std::string version = "(" + GETVERSIONSTRING(RUNTIME_VERSION_1_10_162) + "/" + GETVERSIONSTRING(RUNTIME_VERSION_1_10_163) + ")";
#else
		std::string version = "(" + GETVERSIONSTRING(CURRENT_RELEASE_RUNTIME) + ")";
#endif

		_DMESSAGE("=================================================================================");
		_DMESSAGE("%s Version (%i.%i) by PK0", Version::Name, Version::Major, Version::Minor);
		_DMESSAGE("Game Version %s", version.c_str());
		_DMESSAGE("=================================================================================");

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME 
#if	RUNTIME_VERSION_1_10_138 < CURRENT_RELEASE_RUNTIME
		if (f4se->runtimeVersion < RUNTIME_VERSION_1_10_162 || RUNTIME_VERSION_1_10_163 < f4se->runtimeVersion) {
#else
		if (f4se->runtimeVersion != CURRENT_RELEASE_RUNTIME) {
#endif

			std::string Msg = "Game version error (" + GETVERSIONSTRING(f4se->runtimeVersion) + "), compatible version " + version;

			_ERROR(Msg.c_str());

			MessageBox(GetActiveWindow(), Msg.c_str(), std::string(Version::Name + std::string(" Warning!")).c_str(), MB_ICONWARNING);

			return false;
		}
#endif

		return true;
	}

	void Plugin::MsgCallback(F4SEMessagingInterface::Message* msg) noexcept
	{
		switch (msg->type) {

		case F4SEMessagingInterface::kMessage_GameLoaded:

			Settings::Ini::GetInstance().ReadAllSettings();

			Translations::Lang::GetInstance().Load();

			Fixes::Preset::GetInstance().Load();

			JsonParser::EnumFiles();

			Cache::Map::GetInstance().Load();

			Camera::Player::GetInstance().Init();

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
			Aaf::Event::GetInstance().Hook();

			InGame::HeightEdit::GetInstance().Register();
#endif

			Events::Dispatcher::GetInstance().Register();

			break;

		case F4SEMessagingInterface::kMessage_PostSaveGame:

			Cache::Map::GetInstance().Save();

			break;
		}
	}

	void Plugin::AddTask(ITaskDelegate* task) noexcept
	{
		if (f4se_task_interface)
			f4se_task_interface->AddTask(task);
	}
}
