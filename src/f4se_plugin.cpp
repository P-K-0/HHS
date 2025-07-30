
#include "f4se_plugin.h"
#include "externals.h"
#include "papyrus.h"
#include "scaleform.h"
#include "events.h"
#include "settings.h"
#include "translations.h"
#include "cache.h"
#include "camera.h"
#include "aaf.h"
#include "ingame.h"
#include "jsonparser.h"
#include "fixes.h"

namespace f4se {

	bool Plugin::LoadInterfaces(const F4SEInterface* a_f4se) noexcept
	{
		iLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Fallout4\\F4SE\\HHS.log");

		pluginHandle = a_f4se->GetPluginHandle();

#if CURRENT_RELEASE_RUNTIME != RUNTIME_VR_VERSION_1_2_72
		if (!(isValid = CheckPluginVersion(a_f4se))) {
			return false;
		}
#else
		isValid = true;
#endif

		if (a_f4se->isEditor) {

			_ERROR("Plugin is not compatible with editor!");

			return false;
		}

		if (!(task = static_cast<F4SETaskInterface*>(a_f4se->QueryInterface(kInterface_Task)))) {

			_ERROR("Task Interface error!");

			return false;
		}

		if (!(papyrus = static_cast<F4SEPapyrusInterface*>(a_f4se->QueryInterface(kInterface_Papyrus)))) {

			_ERROR("Papyrus Interface error!");

			return false;
		}

		if (!(messaging = static_cast<F4SEMessagingInterface*>(a_f4se->QueryInterface(kInterface_Messaging)))) {

			_ERROR("Messaging Interface error!");

			return false;
		}

		if (!(scaleform = static_cast<F4SEScaleformInterface*>(a_f4se->QueryInterface(kInterface_Scaleform)))) {

			_ERROR("Scaleform Interface error!");

			return false;
		}

		return true;
	}

	bool Plugin::Register() noexcept
	{
		if (papyrus && messaging && scaleform) {

			papyrus->Register(Papyrus::Register);

			messaging->RegisterListener(pluginHandle, "F4SE", MsgCallback);

			scaleform->Register(Version::ShortName.data(), Scaleform::Register);

			return true;
		}

		return false;
	}

#if CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163
	bool Plugin::Query(const F4SEInterface* a_f4se, PluginInfo* info) noexcept
	{
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = Version::ShortName.data();
		info->version = 1;

		return LoadInterfaces(a_f4se);
	}
#endif

	bool Plugin::Load(const F4SEInterface* a_f4se) noexcept
	{

#if CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163

		return Register();
#else

		if (LoadInterfaces(a_f4se)) {

			return Register();
		}
#endif

		return false;
	}

	std::string Plugin::GetVersionString(std::uint32_t version) noexcept
	{
		std::ostringstream oss; 

		oss << GET_EXE_VERSION_MAJOR(version) << "."
			<< GET_EXE_VERSION_MINOR(version) << "."
			<< GET_EXE_VERSION_BUILD(version) << "."
			<< GET_EXE_VERSION_SUB(version);

		return oss.str();
	}

	void Plugin::PrintInfoPlugin(const std::string& version) noexcept
	{
		_DMESSAGE("=================================================================================");
		_DMESSAGE("%s Version (%i.%i.%i) by PK0", Version::Name.data(), Version::Major, Version::Minor, Version::Build);
		_DMESSAGE("Game Version %s", version.c_str());
		_DMESSAGE("=================================================================================");
	}

	void Plugin::ErrMessageBox(const F4SEInterface* f4se, const std::string& version) noexcept
	{
		auto Msg = "Game version error (" + GetVersionString(f4se->runtimeVersion) + "), compatible version " + version;

		_ERROR(Msg.c_str());

		MessageBox(GetActiveWindow(), Msg.c_str(), (std::string(Version::Name) + " Warning!"_sv.data()).c_str(), MB_ICONWARNING);
	}

	bool Plugin::CheckPluginVersion(const F4SEInterface* f4se) noexcept
	{
#if	CURRENT_RELEASE_RUNTIME >= RUNTIME_VERSION_1_10_162 && CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163

		auto version = "(" + GetVersionString(RUNTIME_VERSION_1_10_162) + "/" + GetVersionString(RUNTIME_VERSION_1_10_163) + ")";

		PrintInfoPlugin(version);

		if (f4se->runtimeVersion < RUNTIME_VERSION_1_10_162 || f4se->runtimeVersion > RUNTIME_VERSION_1_10_163) {

			ErrMessageBox(f4se, version);

			return false;
		}
#else
		auto version = "(" + GetVersionString(CURRENT_RELEASE_RUNTIME) + ")";

		PrintInfoPlugin(version);

		if (f4se->runtimeVersion != CURRENT_RELEASE_RUNTIME) {

			ErrMessageBox(f4se, version);

			return false;
		}
#endif

		return true;
	}

	void Plugin::MsgCallback(F4SEMessagingInterface::Message* msg) noexcept
	{
		switch (msg->type) {

		case F4SEMessagingInterface::kMessage_GameLoaded:

			Trampoline::GetSingleton().Create();

			Settings::Ini::GetSingleton().ReadAllSettings();

			Translations::Lang::GetSingleton().Load();

			Fixes::Preset::GetSingleton().Load();

			JsonParser::EnumFiles();

			Cache::Map::GetSingleton().Load();

			Camera::Player::GetSingleton().Init();

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
			Aaf::Event::GetSingleton().Hook();

			InGame::HeightEdit::GetSingleton().Register();
#endif

			Events::Dispatcher::GetSingleton().Register();

			break;

		case F4SEMessagingInterface::kMessage_PostSaveGame:

			Cache::Map::GetSingleton().Save();

			break;
		}
	}

	void Plugin::AddTask(ITaskDelegate* tsk) noexcept
	{
		if (task) {
			task->AddTask(tsk);
		}
	}
}
