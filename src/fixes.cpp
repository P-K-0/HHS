
#include "fixes.h"

#include "file.h"

namespace Fixes {

	void Preset::EnumFiles() noexcept
	{
		map.clear();

		Load(FilePreset.data());

		try {

			std_lib::set files;

			std::filesystem::path path{ DirData };
			path /= DirFurniture;

			if (!std::filesystem::exists(path)) {
				return;
			}

			for (auto& it : std::filesystem::directory_iterator{ path }) {

				if (std::filesystem::is_regular_file(it) &&
					_strcmpi(it.path().extension().string().c_str(), ".json") == 0) {
					files.insert(DirFurniture.data() + it.path().filename().string());
				}
			}

			if (files.empty()) {
				return;
			}

			for (const auto& file : files) {
				Load(file.c_str());
			}
		}
		catch (std::filesystem::filesystem_error& err) {

			_DMESSAGE("Filesystem error while enumerating presets: %s", err.what());
		}
		catch (...) {

			_DMESSAGE("Unknown error occurred in EnumFiles()");
		}
	}

	bool Preset::Load(const char* lpFilename) noexcept
	{
		_DMESSAGE("Loading preset file: %s", lpFilename);

		File::Reader reader{ lpFilename };
		std::string str;

		if (!reader.Read(str)) {
			return false;
		}

		Json::Reader jReader;
		Json::Value root;

		if (!jReader.parse(str, root) && root.empty()) {
			return false;
		}

		for (const auto& sKeyword : root.getMemberNames()) {

			const auto& keyword = root[sKeyword];

			if (keyword.type() == Json::ValueType::objectValue) {

				_DMESSAGE("Found keyword: %s", sKeyword.c_str());

				auto key = hash(sKeyword.c_str());

				auto it = map.find(key);

				if (it != map.end()) {

					_DMESSAGE("Warning: Duplicate keyword already defined in %s", it->second.filename.c_str());

					continue;
				}

				auto& m = map[key];

				m.filename = lpFilename;

				for (const auto& sNode : keyword.getMemberNames()) {

					const auto& node = keyword[sNode];

					if (node.type() == Json::ValueType::objectValue) {

						_DMESSAGE("Found node: %s", sNode.c_str());

						m.pairNodes.second.push_back(Node::NodeValues{ sNode.c_str() });

						auto& nodeValues = m.pairNodes.second.back();

						nodeValues.scale.value = -1.0f;

						for (const auto& sFlag : node.getMemberNames()) {

							_DMESSAGE("Processing flag: %s", sFlag.c_str());

							const auto& flag = node[sFlag];

							switch (GetFlags(sFlag.c_str())) {

							case Node::Flags::PosX:

								SetPairValue(nodeValues.x, flag);
								break;

							case Node::Flags::PosY:

								SetPairValue(nodeValues.y, flag);
								break;

							case Node::Flags::PosZ:

								SetPairValue(nodeValues.z, flag);
								break;

							case Node::Flags::RotX:

								SetPairValue(nodeValues.heading, flag);
								break;

							case Node::Flags::RotY:

								SetPairValue(nodeValues.attitude, flag);
								break;

							case Node::Flags::RotZ:

								SetPairValue(nodeValues.bank, flag);
								break;

							case Node::Flags::Scale:

								SetPairValue(nodeValues.scale, flag);
								break;

							default:

								_DMESSAGE("Warning: Unrecognized flag encountered!");
								break;
							}
						}
					}
					else if (node.type() == Json::ValueType::booleanValue) {

						if (sNode == "nostop") {

							m.pairNodes.first = node.asBool();

							_DMESSAGE("NoStop flag set to: %i", m.pairNodes.first);
						}
					}
				}
			}
		}

		_DMESSAGE("Finished loading preset file.");

		return true;
	}

	void Preset::SetPairValue(Node::PairValue& pairValue, const Json::Value& jValue) noexcept
	{
		const auto& value = jValue["value"];
		const auto& mul = jValue["mulheight"];

		if (value.type() == Json::ValueType::realValue) {
			pairValue.value = value.asFloat();
		}

		if (mul.type() == Json::ValueType::booleanValue) {
			pairValue.mul = mul.asBool();
		}

		_DMESSAGE("Set value: %.2f | MulHeight: %s", pairValue.value, pairValue.mul ? "true" : "false");
	}

	Preset::PairNodes& Preset::FindKeyword(const char* sKeyword) noexcept
	{
		auto key = hash(sKeyword);

		auto it = map.find(key);

		if (it != map.end()) {
			return it->second.pairNodes;
		}

		return fallBack;
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
