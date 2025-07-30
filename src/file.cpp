
#include "file.h"

#include "settings.h"

namespace File {

	bool Reader::Read(std::string& dst) noexcept
	{
		if (filename.empty()) {
			return false;
		}

		auto& settings = Settings::Ini::GetSingleton();

		len = settings.GetReadBufferLen();

		if (settings.GetAltRead()) {
			return InputStreamRead(dst, len) > 0;
		}

		return BSResourceNiBinaryStreamRead(dst, len) > 0;
	}

	std::uint32_t Reader::InputStreamRead(std::string& dst, std::uint32_t len) noexcept
	{
		std::ifstream ifs{ DirData.data() + filename, std::ios_base::binary };

		if (!ifs) {
			return 0;
		}

		dst.clear();

		char c{};
		std::uint32_t count{};

		for (count = 0; count < len && !ifs.eof(); ++count) {
			ifs.read(&c, sizeof c);
			dst += c;
		}

		return count;
	}

	std::uint32_t Reader::BSResourceNiBinaryStreamRead(std::string& dst, std::uint32_t len) noexcept
	{
		BSResourceNiBinaryStream bsRead(filename.c_str());

		if (!bsRead.IsValid()) {
			return 0;
		}

		dst.clear();

		char c{};
		std::uint32_t count{};

		for (count = 0; count < len && bsRead.Read(&c, sizeof c) != 0; ++count) {
			dst += c;
		}

		return dst.size();
	}

	std::string GetRelativeDir(const std::string& str) noexcept
	{
		std::string& path = std::filesystem::path(str).lexically_normal().string();

		if (_strcmpi(path.substr(0, DirMeshes.length()).c_str(), DirMeshes.data()) == 0) {
			return path;
		}

		if (_strcmpi(path.substr(0, DirData.length()).c_str(), DirData.data()) == 0) {
			return path.substr(DirData.length());
		}

		return DirMeshes.data() + path;
	}
}
