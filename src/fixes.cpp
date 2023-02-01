
#include "fixes.h"

namespace Fixes {

	bool Preset::Load() noexcept
	{
		std::ifstream ifs{ FilePreset, std::ios_base::binary };

		if (!ifs)
			return false;

		_DMESSAGE("Reading preset file from %s...", FilePreset);

		std::string str{};

		for (char c{}; !ifs.eof(); ) {
			ifs.read(&c, sizeof c);
			str += c;
		}

		Json::Reader jReader;

		saf_version = 0;

		root.clear();

		if (!jReader.parse(str, root))
			return false;

		if (root.empty())
			return false;

		auto v = root["saf"];

		if (v.type() == Json::ValueType::intValue)
			saf_version = v.asInt();

		return true;
	}

	bool Preset::FindKeyword(const std::string& sKeyword, Json::Value& value) noexcept
	{
		if (root.empty())
			return false;

		auto members = root.getMemberNames();

		for (auto& m : members) {

			if (m == sKeyword) {

				value = root[sKeyword];

				return true;
			}
		}

		return false;
	}

	Node::Flags Preset::GetFlags(const std::string& str) noexcept
	{
		static std::vector<std::string> v_flags{ "posx", "posy", "posz", "scale", "rotx", "roty", "rotz" };

		for (std::uint32_t index{}; index < v_flags.size(); index++) {

			if (str == v_flags[index])
				return Node::flags_cast(index);
		}

		return Node::Flags::PosX;
	}

	Preset Preset::instance;
}
