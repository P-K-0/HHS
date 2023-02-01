
#include "file.h"

#include "settings.h"

namespace File {

	bool Reader::Read(std::string& dst) noexcept
	{
		if (filename.empty())
			return 0;

		auto& settings = Settings::Ini::GetInstance();

		len = settings.Get_iReadBufferLen();

		if (settings.Get_bAltRead()) {

			return InputStreamRead(dst, len) > 0;
		}

		return BSResourceNiBinaryStreamRead(dst, len) > 0;
	}

	std::uint32_t Reader::InputStreamRead(std::string& dst, const std::uint32_t len) noexcept
	{
		std::ifstream ifs{ filename, std::ios_base::binary };

		if (!ifs)
			return 0;

		dst.clear();

		char c{};
		std::uint32_t count{};

		for (count = 0; count < len; count++) {

			if (ifs.eof())
				break;

			ifs.read(&c, sizeof c);
			dst += c;
		}

		return count;
	}

	std::uint32_t Reader::BSResourceNiBinaryStreamRead(std::string& dst, const std::uint32_t len) noexcept
	{
		BSResourceNiBinaryStream bsRead(filename.c_str());

		if (!bsRead.IsValid())
			return 0;

		dst.clear();

		char c{};
		std::uint32_t count{};

		for (count = 0; count < len; count++) {

			if (bsRead.Read(&c, sizeof c) == 0)
				break;

			dst += c;
		}

		return dst.size();
	}
}
