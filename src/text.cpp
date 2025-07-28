
#include "text.h"

#include "Settings.h"
#include "file.h"

namespace Text {

	float Parser(const std::string& str) noexcept
	{
		std::regex reg{ R"(height\s*=\s*(-?(?:\d*\.\d+|\d+)))", std::regex::icase };
		std::smatch matches;

		if (!std::regex_search(str, matches, reg)) {
			return ZeroValue;
		}

		try {

			return std::stof(matches[1].str());
		}
		catch (...) {}

		return ZeroValue;
	}

	std::string GetTextFile(const std::string& Filename, bool dirF4SE) noexcept
	{
		std::filesystem::path path{ Filename };

		path.replace_extension(".txt");

		if (dirF4SE) {
			return std::string{ DirF4SE.data() + path.filename().string()};
		}

		return File::GetRelativeDir(path.string());
	}

	float GetHeightFromText(const std::string& filename) noexcept
	{
		if (!Settings::Ini::GetSingleton().GetEnableTextFile()) {
			return ZeroValue;
		}

		std::vector<std::string> files{ GetTextFile(filename, false), GetTextFile(filename, true) };

		std::string str;

		float height{ ZeroValue };

		for (std::size_t i{}; i < files.size() && height == ZeroValue; ++i) {

			File::Reader reader{ files[i] };

			if (reader.Read(str)) {
				height = Parser(str);
			}
		}

		return height;
	}

	bool CreateHeightFile(const std::string& Filename, float value) noexcept
	{
		if (value == ZeroValue) {
			return false;
		}

		std::string file{ GetTextFile(Filename, Settings::Ini::GetSingleton().GetDirF4SE()) };

		std::filesystem::path path{ file };

		path.remove_filename();

		std::filesystem::create_directories(path);

		std::ofstream ofs{ DirData.data() + file };

		if (!ofs) {
			return false;
		}

		ofs << "Height = " << value;

		ofs.flush();

		return ofs.good();
	}

	bool RemoveHeightFile(const std::string& Filename) noexcept
	{
		bool ret{};

		std::vector<std::string> files{ GetTextFile(Filename, false), GetTextFile(Filename, true) };

		for (auto& file : files) {
			ret |= std::filesystem::remove(DirData.data() + file);
		}

		return ret;
	}
}
