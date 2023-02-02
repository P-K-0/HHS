
#include "text.h"

#include "Settings.h"
#include "file.h"

namespace Text {

	float Parser(const std::string& str) noexcept
	{
		std::regex reg{ R"((\s*)(\w*)(\s*)=(\s*)(\d*.\d*))" };
		std::smatch matches;

		if (!std::regex_search(str, matches, reg))
			return InvalidValue;

		try {

			if (_strcmpi(matches[2].str().c_str(), "height") != 0)
				return InvalidValue;

			return std::stof(matches[5].str());
		}
		catch (...) {}

		return InvalidValue;
	}

	std::string GetTextFile(const std::string& Filename, const bool& dirF4SE) noexcept
	{
		boost::filesystem::path path{ Filename };

		path.replace_extension(".txt");

		if (dirF4SE) {

			return std::string{ DirF4SE + path.filename().string()};
		}

		return File::GetRelativeDir(path.string());
	}

	float GetHeightFromText(const std::string& filename) noexcept
	{
		if (!Settings::Ini::GetInstance().Get_bEnableTextFile())
			return InvalidValue;

		std::vector<std::string> files{ GetTextFile(filename, false), GetTextFile(filename, true) };

		std::string str;

		float height{ MinValue };

		for (auto& file : files) {

			File::Reader reader{ file }; 

			if (!reader.Read(str))
				continue;

			height = Parser(str);

			if (height > MinValue)
				break;
		}

		return height > MinValue ? height : InvalidValue;
	}

	bool CreateHeightFile(const std::string& Filename, const float value) noexcept
	{
		std::string file{ GetTextFile(Filename, Settings::Ini::GetInstance().Get_iDirF4SE()) };

		boost::filesystem::path path{ file };

		path.remove_filename();

		boost::filesystem::create_directories(path);

		std::ofstream ofs{ file };

		if (!ofs || value <= MinValue)
			return false;

		ofs << "Height = " << value;

		ofs.flush();

		return ofs.good();
	}

	bool RemoveHeightFile(const std::string& Filename) noexcept
	{
		bool ret{};

		std::vector<std::string> files{ GetTextFile(Filename, false), GetTextFile(Filename, true) };

		for (auto& file : files)
			ret |= boost::filesystem::remove(file);

		return ret;
	}
}
