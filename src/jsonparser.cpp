
#include "jsonparser.h"

#include "settings.h"
#include "ba2.h"
#include "file.h"
#include "cache.h"

#include "json/json.h"

namespace JsonParser {

	void EnumFiles(const std::string& directory, std_boost::set& files) noexcept
	{
		boost::filesystem::path path{ DirData.data() + directory };

		if (!boost::filesystem::exists(path))
			return;

		try {

			for (auto& it : boost::filesystem::directory_iterator{ path }) {

				if (!boost::filesystem::is_regular_file(it))
					continue;

				if (_strcmpi(it.path().extension().string().c_str(), ".json") != 0)
					continue;

				files.insert(directory + it.path().filename().string());
			}
		}
		catch (boost::filesystem::filesystem_error& err) {

			_DMESSAGE("%s", err.what());
		}
		catch (...) {

			_DMESSAGE("%s unknown error!", __FUNCTION__);
		}
	}

	void EnumFilesBA2(const std::string& directory, std_boost::set& files) noexcept
	{
		if (Settings::Ini::GetInstance().Get_bAltRead())
			return;

		boost::filesystem::path path{ DirData.data() };

		if (!boost::filesystem::exists(path))
			return;

		for (auto& dir : boost::filesystem::directory_iterator{ path }) {

			if (!boost::filesystem::is_regular_file(dir))
				continue;

			if (_strcmpi(dir.path().extension().string().c_str(), ".ba2") != 0)
				continue;

			BA2::Reader reader{ dir.path().string() };

			_DMESSAGE("Opening file : %s error code %i", dir.path().filename().string().c_str(), reader.GetError());

			if (!reader || reader.GetHeader().GetType() != BA2::Type::GNRL)
				continue;

			for (auto& tbl : reader.GetStringsTable()) {

				boost::filesystem::path file{ tbl.GetFilename() };

				std::string parent_path = file.branch_path().lexically_normal().string() + "\\";

				if (_strcmpi(directory.c_str(), parent_path.c_str()) == 0 && _strcmpi(file.extension().string().c_str(), ".json") == 0) {

					files.insert(file.string());
				}
			}
		}
	}

	[[nodiscard]] std::uint32_t GetFormIDByModName(const std::string& name, const std::string& id) noexcept
	{
		if (name.empty() || id.empty())
			return 0;

		std::uint32_t formID{};

		std::stringstream sstr;

		sstr << std::hex << id;
		sstr >> formID;

		VisitMods([&](ModInfo* mod) {
	
			if (_strcmpi(name.c_str(), mod->name) == 0 && mod->modIndex != 0xff) {

				formID |= mod->modIndex == 0xfe ? (static_cast<std::uint32_t>(mod->lightIndex) << 12) : (static_cast<std::uint32_t>(mod->modIndex) << 24);

				return true;
			}

			return false;
		});

		return formID;
	}

	class BGSMod__Attachment__Mod;

	template<typename Func = std::function<void(const std::string&)>>
	void GetPathFromID(const std::uint32_t id, const std::uint32_t gender, Func func) noexcept
	{
		TESForm* form{ nullptr };

		if (id == 0 || !(form = LookupFormByID(id)))
			return;

		if (gender == 3) {

			BGSMod::Attachment::Mod* objectmod = (BGSMod::Attachment::Mod*)DYNAMIC_CAST(form, TESForm, BGSMod__Attachment__Mod);

			if (objectmod) {

				func(objectmod->materialSwap.GetModelName());
			}
			
			return;
		}
		
		TESObjectARMA* arma = DYNAMIC_CAST(form, TESForm, TESObjectARMA);

		if (!arma)
			return;

		switch (gender) {

		case 0: 
		case 1:

			func(arma->swap50[gender].GetModelName());

			break;

		case 2:

			for (int gndr = 0; gndr < 2; gndr++)
				func(arma->swap50[gndr].GetModelName());

			break;

		default:

			_DMESSAGE("Invalid gender : %i", gender);
		}
	}

	[[nodiscard]] bool HeightFile(const std::string& Filename) noexcept
	{
		File::Reader reader{ Filename };

		std::string buffer;

		if (!reader.Read(buffer))
			return false;

		Json::Value root;
		Json::Reader jReader;

		if (!jReader.parse(buffer, root))
			return false;

		auto& cache = Cache::Map::GetInstance();

		for (auto& member : root.getMemberNames()) {

			auto& _root = root[member];

			if (!_root.isArray())
				continue;

			for (auto& v : _root) {

				auto& key = v["key"];
				auto& value = v["value"];

				if (!key.isString() || key.asString().empty()) {

					auto& formId = v["formid"];
					auto& gender = v["gender"];

					auto id = GetFormIDByModName(member.c_str(), formId.asString());

					GetPathFromID(id, gender.asInt(), [&](const std::string& filepath) {

						cache.Insert(filepath, value.asFloat());
					});					

					continue;
				}

				cache.Insert(key.asString(), value.asFloat());
			}
		}

		return true;
	}

	void EnumFiles() noexcept
	{
		if (!Settings::Ini::GetInstance().Get_bEnableJsonFile())
			return;

		std_boost::set files;

		EnumFiles(DirF4SE.data(), files);

		EnumFilesBA2(DirF4SE.data(), files);

		if (files.empty())
			return;

		for (auto& f : files) {

			_DMESSAGE("Loading Json : %s %s", f.c_str(), HeightFile(f) ? "parsed successfully." : "found errors!");
		}
	}
}
