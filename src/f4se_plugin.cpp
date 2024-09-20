
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

#if CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163
	bool Plugin::Query(const F4SEInterface* f4se, PluginInfo* info) noexcept
	{
		iLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Fallout4\\F4SE\\HHS.log");

		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = Version::ShortName.data();
		info->version = 1;

		hPlug = f4se->GetPluginHandle();

		if (!(isValid = CheckPluginVersion(f4se))) {
			return false;
		}

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
#endif

	bool Plugin::Load(const F4SEInterface* a_f4se) noexcept
	{
#if CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163
		if (f4se_papyrus_interface && f4se_msg_interface && f4se_scaleform_interface) {

			f4se_papyrus_interface->Register(Papyrus::Register);

			f4se_msg_interface->RegisterListener(hPlug, "F4SE", MsgCallback);

			f4se_scaleform_interface->Register(Version::ShortName.data(), Scaleform::Register);

			return true;
		}
#else
		iLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Fallout4\\F4SE\\HHS.log");

		if (!(isValid = CheckPluginVersion(a_f4se))) {
			return false;
		}

		if (a_f4se->isEditor) {

			_ERROR("Plugin is not compatible with editor!");

			return false;
		}

		if (!(f4se_papyrus_interface = static_cast<F4SEPapyrusInterface*>(a_f4se->QueryInterface(kInterface_Papyrus)))) {

			_ERROR("Papyrus Interface error!");

			return false;
		}

		if (!(f4se_task_interface = static_cast<F4SETaskInterface*>(a_f4se->QueryInterface(kInterface_Task)))) {

			_ERROR("Task Interface error!");

			return false;
		}

		if (!(f4se_msg_interface = static_cast<F4SEMessagingInterface*>(a_f4se->QueryInterface(kInterface_Messaging)))) {

			_ERROR("Messaging Interface error!");

			return false;
		}

		if (!(f4se_scaleform_interface = static_cast<F4SEScaleformInterface*>(a_f4se->QueryInterface(kInterface_Scaleform)))) {

			_ERROR("Scaleform Interface error!");

			return false;
		}

		if (f4se_papyrus_interface && f4se_msg_interface && f4se_scaleform_interface) {

			f4se_papyrus_interface->Register(Papyrus::Register);

			f4se_msg_interface->RegisterListener(a_f4se->GetPluginHandle(), "F4SE", MsgCallback);

			f4se_scaleform_interface->Register(Version::ShortName.data(), Scaleform::Register);

			return true;
		}
#endif

		return false;
	}

	std::string Plugin::GetVersionString(std::uint32_t version) noexcept
	{
		return std::to_string(GET_EXE_VERSION_MAJOR(version)) + "." +
			std::to_string(GET_EXE_VERSION_MINOR(version)) + "." +
			std::to_string(GET_EXE_VERSION_BUILD(version)) + "." +
			std::to_string(GET_EXE_VERSION_SUB(version));
	}

	bool Plugin::CheckPluginVersion(const F4SEInterface* f4se) noexcept
	{
#if	CURRENT_RELEASE_RUNTIME >= RUNTIME_VERSION_1_10_162 && CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163
		auto version = "(" + GetVersionString(RUNTIME_VERSION_1_10_162) + "/" + GetVersionString(RUNTIME_VERSION_1_10_163) + ")";
#else
		auto version = "(" + GetVersionString(CURRENT_RELEASE_RUNTIME) + ")";
#endif

		_DMESSAGE("=================================================================================");
		_DMESSAGE("%s Version (%i.%i.%i) by PK0", Version::Name.data(), Version::Major, Version::Minor, Version::Build);
		_DMESSAGE("Game Version %s", version.c_str());
		_DMESSAGE("=================================================================================");

#if	CURRENT_RELEASE_RUNTIME < RUNTIME_VERSION_1_10_162 && CURRENT_RELEASE_RUNTIME > RUNTIME_VERSION_1_10_163
		if (f4se->runtimeVersion != CURRENT_RELEASE_RUNTIME) {
#else
		if (f4se->runtimeVersion < RUNTIME_VERSION_1_10_162 || f4se->runtimeVersion > RUNTIME_VERSION_1_10_163) {
#endif
			auto Msg = "Game version error (" + GetVersionString(f4se->runtimeVersion) + "), compatible version " + version;

			_ERROR(Msg.c_str());

			MessageBox(GetActiveWindow(), Msg.c_str(), (std::string(Version::Name) + " Warning!"_sv.data()).c_str(), MB_ICONWARNING);

			return false;
		}

		return true;
	}

	void Plugin::MsgCallback(F4SEMessagingInterface::Message* msg) noexcept
	{
		switch (msg->type) {

		case F4SEMessagingInterface::kMessage_GameLoaded:

			Trampoline::GetSingleton().Create();

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
		if (f4se_task_interface) {
			f4se_task_interface->AddTask(task);
		}
	}
}
