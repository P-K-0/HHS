
#include "file.h"

#include "settings.h"

namespace File {

	std::uint32_t Reader::Read(std::string& dst, const std::uint32_t len) noexcept
	{
		if (filename.empty())
			return 0;

		if (Settings::Ini::GetInstance().Get_bAltRead()) {

			return InputStreamRead(dst, len);
		}

		return BSResourceNiBinaryStreamRead(dst, len);
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
