
#include "ingame.h"

#include "externals.h"
#include "translations.h"
#include "cache.h"
#include "actors.h"
#include "text.h"
#include "papyrus.h"

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME

namespace InGame {

	[[nodiscard]] std::uint32_t GetFormIDByHandle(UInt32 handle)
	{
		TESObjectREFR* refr{ nullptr };

		if (!LookupREFRByHandle(&handle, &refr) || !refr)
			return 0;

		return refr->formID;
	}

	EventResult	HeightEdit::ReceiveEvent(ViewCasterUpdateEvent* evn, void* dispatcher)
	{
		handle = 0;

		if (!evn || evn->handle == 0 || evn->handle == (*g_invalidRefHandle))
			return kEvent_Continue;

		handle = evn->handle;

		if (!enabled)
			return kEvent_Continue;

		auto id = GetFormIDByHandle(handle);

		if (id != 0)
			Papyrus::Notification("ID:", id);

		return kEvent_Continue;
	};

	void HeightEdit::Register() noexcept
	{
		static bool registered{};

		if (registered)
			return;

		(*g_viewCasterUpdateDispatcher)->eventDispatcher.AddEventSink(&instance);

		registered = true;
	}

	bool HeightEdit::Process(const Key& key) noexcept
	{
		auto& cache = Cache::Map::GetInstance();

		auto& settings = Settings::Ini::GetInstance();

		std::string filename;
		std::uint32_t id{};

		bool ret{ true };

		std::uint32_t refID{ PlayerID };

		if (handle != 0 && settings.Get_iReference() == Settings::Reference::CrossHair) {

			refID = GetFormIDByHandle(handle);
		}

		hhs::Error err = hhs::Map::GetInstance().visit(false, refID, [&](hhs::System& sys) {

			if (!sys.GetActorUtil().GetEquipData(settings.Get_iSlot(), id, filename))
				return hhs::Error::Unknown;

			if (!cache.IsText(filename))
				return hhs::Error::HeightTxt;

			hhs::Error e{ hhs::Error::Success };

			if (key == Key::Delete) {
					
				if ((e = sys.ResetHeight()) == hhs::Error::Success) {

					cache.Erase(filename);

					ret = Text::RemoveHeightFile(filename);
				}

				return e;
			}

			auto height = settings.Get_fHeight();

			if ((e = sys.SetHeight(height)) == hhs::Error::Success) {

				cache.Insert(filename, height, true);

				if (key == Key::Create) {

					ret = Text::CreateHeightFile(filename, height);
				}
			}

			return e;
		});

		ShowError(err);

		return err == hhs::Error::Success && ret;
	}

	bool HeightEdit::Test() noexcept
	{
		auto& lang = Translations::Lang::GetInstance();

		if (!Process(Key::Test)) {

			Papyrus::Message_Box(lang[Translations::LangID::ErrorTest]);

			return false;
		}

		Papyrus::Message_Box(lang[Translations::LangID::TestHeight]);

		return true;
	}

	bool HeightEdit::Create() noexcept
	{
		auto& lang = Translations::Lang::GetInstance();

		if (!Process(Key::Create)) {

			Papyrus::Message_Box(lang[Translations::LangID::ErrorCreated]);

			return false;
		}

		Papyrus::Message_Box(lang[Translations::LangID::FileCreated]);

		return true;
	}

	bool HeightEdit::Delete() noexcept
	{
		auto& lang = Translations::Lang::GetInstance();

		if (!Process(Key::Delete)) {

			Papyrus::Message_Box(lang[Translations::LangID::ErrorDeleted]);

			return false;
		}

		Papyrus::Message_Box(lang[Translations::LangID::FileDeleted]);

		return true;
	}

	void HeightEdit::OnKeyPress(const Key& key) noexcept
	{
		auto& lang = Translations::Lang::GetInstance();

		if (key == Key::Activate) {

			enabled = !enabled;
		
			Papyrus::Notification(lang[enabled ? Translations::LangID::Activated : Translations::LangID::Deactivated]);
		}

		if (!enabled)
			return;

		switch (key) {

		case Key::Create: Create(); break;

		case Key::Delete: Delete(); break;

		case Key::Increment: IncHeight(); break;

		case Key::Decrement: DecHeight(); break;

		case Key::ChangeReference: 

			{
				auto& settings = Settings::Ini::GetInstance();

				if (settings.Get_iReference() == Settings::Reference::Player) {

					settings.Set_iReference(Settings::Reference::CrossHair);
				}
				else {

					settings.Set_iReference(Settings::Reference::Player);
				}

				Papyrus::Notification(lang[settings.Get_iReference() == Settings::Reference::Player ? Translations::LangID::PlayerRefr : Translations::LangID::CrossHairRefr]);
			}

			break;
		}
	}

	void HeightEdit::IncHeight() noexcept
	{
		auto& settings = Settings::Ini::GetInstance();

		settings.Set_fHeight(settings.Get_fHeight() + settings.Get_fStep());

		Process(Key::Test);
	}

	void HeightEdit::DecHeight() noexcept
	{
		auto& settings = Settings::Ini::GetInstance();

		settings.Set_fHeight(settings.Get_fHeight() - settings.Get_fStep());

		Process(Key::Test);
	}

	void HeightEdit::ShowError(const hhs::Error& error) noexcept
	{
		if (error == hhs::Error::Success)
			return;

		auto& lang = Translations::Lang::GetInstance();
		
		switch (error) {

		case hhs::Error::Unknown: Papyrus::Message_Box(lang[Translations::LangID::ErrorUnknown]); break;

		case hhs::Error::ComOverride: Papyrus::Message_Box(lang[Translations::LangID::ErrorComOverride]); break;

		case hhs::Error::Runtime: Papyrus::Message_Box(lang[Translations::LangID::ErrorRuntime]); break;

		case hhs::Error::ActorDisabled: Papyrus::Message_Box(lang[Translations::LangID::ErrorActorDisabled]); break;

		case hhs::Error::Override: Papyrus::Message_Box(lang[Translations::LangID::ErrorOverride]); break;

		case hhs::Error::SetHeight: Papyrus::Message_Box(lang[Translations::LangID::ErrorSetHeight]); break;

		case hhs::Error::Race: Papyrus::Message_Box(lang[Translations::LangID::ErrorRace]); break;

		case hhs::Error::HeightTxt: Papyrus::Message_Box(lang[Translations::LangID::ErrorHeightTxt]); break;

		case hhs::Error::Arguments: Papyrus::Message_Box(lang[Translations::LangID::ErrorArguments]); break;

		}
	}

	HeightEdit HeightEdit::instance;
}

#endif
