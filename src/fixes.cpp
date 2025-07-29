
#include "fixes.h"

#include "file.h"

namespace Fixes {

	bool Preset::Load() noexcept
	{
		std::string str;

		_DMESSAGE("Reading preset file from %s...", FilePreset);

		File::Reader reader{ FilePreset };

		if (!reader.Read(str)) {
			return false;
		}

		Json::Reader jReader;

		saf_version = 0;

		root.clear();

		if (!jReader.parse(str, root)) {
			return false;
		}

		if (root.empty()) {
			return false;
		}

		auto v = root["saf"];

		if (v.type() == Json::ValueType::intValue) {
			saf_version = v.asInt();
		}

		_DMESSAGE("Preset file Loaded!");

		return true;
	}

	bool Preset::FindKeyword(const char* sKeyword, Json::Value& value) noexcept
	{
		if (root.empty()) {
			return false;
		}

		auto members = root.getMemberNames();

		for (const auto& m : members) {

			if (m == sKeyword) {

				value = root[sKeyword];

				return true;
			}
		}

		return false;
	}

	Node::Flags Preset::GetFlags(const char* str) noexcept
	{
		switch (hash(str)) {

		case "posx"_hash:
			return Node::Flags::PosX;

		case "posy"_hash:
			return Node::Flags::PosY;

		case "posz"_hash:
			return Node::Flags::PosZ;

		case "scale"_hash:
			return Node::Flags::Scale;

		case "rotx"_hash:
			return Node::Flags::RotX;

		case "roty"_hash:
			return Node::Flags::RotY;

		case "rotz"_hash:
			return Node::Flags::RotZ;

		default:
			return Node::Flags::None;
			
		}
	}
}
