
#include "ingame.h"

#include "externals.h"
#include "translations.h"
#include "cache.h"
#include "actors.h"
#include "text.h"
#include "papyrus.h"

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME

namespace InGame {

	std::uint32_t HeightEdit::GetFormIDByHandle(UInt32 handle) noexcept
	{
#if CURRENT_RELEASE_RUNTIME <= RUNTIME_VERSION_1_10_163
		TESObjectREFR* refr{ nullptr };

		if (!LookupREFRByHandle(&handle, &refr) || !refr) {
			return 0;
		}
#else
		NiPointer<TESObjectREFR> refr;

		if (!LookupREFRByHandle(handle, refr) || !refr) {
			return 0;
		}
#endif

		return refr->formID;
	}

	EventResult	HeightEdit::ReceiveEvent(ViewCasterUpdateEvent* evn, void* dispatcher)
	{
		handle = 0;

		if (!evn || evn->handle == 0 || evn->handle == (*g_invalidRefHandle)) {
			return kEvent_Continue;
		}

		handle = evn->handle;

		if (!enabled) {
			return kEvent_Continue;
		}

		auto id = GetFormIDByHandle(handle);

		if (id != 0) {
			Papyrus::Notification("ID:", id);
		}

		return kEvent_Continue;
	};

	void HeightEdit::Register() noexcept
	{
		static bool registered{};

		if (registered || !g_viewCasterUpdateDispatcher || !(*g_viewCasterUpdateDispatcher)) {
			return;
		}

		_DMESSAGE("Initializing height adjustment interface...");

		if ((*g_viewCasterUpdateDispatcher)->eventDispatcher.AddEventSink(this)) {

			_DMESSAGE("Height adjustment interface initialized successfully.");
		}
		else {

			_DMESSAGE("Error initializing height adjustment interface!");
		}

		registered = true;
	}

	bool HeightEdit::Process(Key key) noexcept
	{
		auto& cache = Cache::Map::GetSingleton();

		auto& settings = Settings::Ini::GetSingleton();

		std::string filename;
		std::uint32_t id{};

		bool ret{ true };

		std::uint32_t refID{ PlayerID };

		if (handle != 0 && settings.GetReference() == Settings::Reference::CrossHair) {
			refID = GetFormIDByHandle(handle);
		}

		hhs::Error err = hhs::Map::GetSingleton().visit(hhs::VisitFlags::None, refID, [&](hhs::System& sys) {

			if (!sys.GetActorUtil().GetEquipData(settings.GetSlot(), id, filename)) {
				return hhs::Error::Unknown;
			}

			if (!cache.IsText(filename)) {
				return hhs::Error::HeightTxt;
			}

			hhs::Error err{ hhs::Error::Success };

			if (key == Key::Delete) {
					
				if ((err = sys.ResetHeight()) == hhs::Error::Success) {

					cache.Erase(filename);

					ret = Text::RemoveHeightFile(filename);
				}

				return err;
			}

			auto height = settings.GetHeight();

			if ((err = sys.SetHeight(height)) == hhs::Error::Success) {

				cache.Insert(filename, height, true, true);

				if (key == Key::Create) {

					ret = Text::CreateHeightFile(filename, height);
				}
			}

			return err;
		});

		ShowError(err);

		return err == hhs::Error::Success && ret;
	}

	bool HeightEdit::Test() noexcept
	{
		auto& lang = Translations::Lang::GetSingleton();

		if (!Process(Key::Test)) {

			Papyrus::Message_Box(lang[Translations::LangID::ErrorTest].data());

			return false;
		}

		Papyrus::Message_Box(lang[Translations::LangID::TestHeight].data());

		return true;
	}

	bool HeightEdit::Create() noexcept
	{
		auto& lang = Translations::Lang::GetSingleton();

		if (!Process(Key::Create)) {

			Papyrus::Message_Box(lang[Translations::LangID::ErrorCreated].data());

			return false;
		}

		Papyrus::Message_Box(lang[Translations::LangID::FileCreated].data());

		return true;
	}

	bool HeightEdit::Delete() noexcept
	{
		auto& lang = Translations::Lang::GetSingleton();

		if (!Process(Key::Delete)) {

			Papyrus::Message_Box(lang[Translations::LangID::ErrorDeleted].data());

			return false;
		}

		Papyrus::Message_Box(lang[Translations::LangID::FileDeleted].data());

		return true;
	}

	void HeightEdit::OnKeyPress(Key key) noexcept
	{
		auto& lang = Translations::Lang::GetSingleton();

		if (key == Key::Activate) {

			enabled = !enabled;
		
			Papyrus::Notification(lang[enabled ? Translations::LangID::Activated : Translations::LangID::Deactivated].data());
		}

		if (!enabled) {
			return;
		}

		switch (key) {

		case Key::Create: Create(); break;

		case Key::Delete: Delete(); break;

		case Key::Increment: IncHeight(); break;

		case Key::Decrement: DecHeight(); break;

		case Key::ChangeReference: 

			{
				auto& settings = Settings::Ini::GetSingleton();

				if (settings.GetReference() == Settings::Reference::Player) {

					settings.SetReference(Settings::Reference::CrossHair); 
				}
				else {

					settings.SetReference(Settings::Reference::Player);
				}

				Papyrus::Notification(lang[settings.GetReference() == Settings::Reference::Player ? Translations::LangID::PlayerRefr : Translations::LangID::CrossHairRefr].data());
			}

			break;
		}
	}

	void HeightEdit::IncHeight() noexcept
	{
		auto& settings = Settings::Ini::GetSingleton();

		settings.SetHeight(settings.GetHeight() + settings.GetStep());

		Process(Key::Test);
	}

	void HeightEdit::DecHeight() noexcept
	{
		auto& settings = Settings::Ini::GetSingleton();

		settings.SetHeight(settings.GetHeight() - settings.GetStep());

		Process(Key::Test);
	}

	void HeightEdit::ShowError(hhs::Error error) noexcept
	{
		if (error == hhs::Error::Success) {
			return;
		}

		auto& lang = Translations::Lang::GetSingleton();
		
		switch (error) {

		case hhs::Error::Unknown:
			Papyrus::Message_Box(lang[Translations::LangID::ErrorUnknown].data()); 
			break;

		case hhs::Error::ComOverride: 
			Papyrus::Message_Box(lang[Translations::LangID::ErrorComOverride].data());
			break;

		case hhs::Error::Runtime:
			Papyrus::Message_Box(lang[Translations::LangID::ErrorRuntime].data());
			break;

		case hhs::Error::ActorDisabled:
			Papyrus::Message_Box(lang[Translations::LangID::ErrorActorDisabled].data());
			break;

		case hhs::Error::Override:
			Papyrus::Message_Box(lang[Translations::LangID::ErrorOverride].data());
			break;

		case hhs::Error::SetHeight:
			Papyrus::Message_Box(lang[Translations::LangID::ErrorSetHeight].data());
			break;

		case hhs::Error::Race: 
			Papyrus::Message_Box(lang[Translations::LangID::ErrorRace].data());
			break;

		case hhs::Error::HeightTxt: 
			Papyrus::Message_Box(lang[Translations::LangID::ErrorHeightTxt].data());
			break;

		case hhs::Error::Arguments: 
			Papyrus::Message_Box(lang[Translations::LangID::ErrorArguments].data());
			break;

		}
	}
}

#endif
