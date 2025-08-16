#pragma once

#include "version.h"
#include "externals.h"
#include "node.h"
#include "util.h"

#include "json/json.h"

namespace Fixes {

	constexpr auto FilePreset = "f4se\\plugins\\hhs_fixes.json"_sv;
	constexpr auto DirFurniture = "f4se\\plugins\\hhs\\furniture\\"_sv;

	class Preset :
		public util::Singleton<Preset> {
		friend class util::Singleton<Preset>;

	public:

		using Keyword = std::size_t;
		using NoStop = bool;
		using VectorNodeValues = std::vector<Node::NodeValues>;
		using PairNodes = std::pair<NoStop, VectorNodeValues>;

		struct Furniture {

			std::string filename;
			PairNodes pairNodes;
		};

		void EnumFiles() noexcept;

		template<typename Fn> // Fn = void(NodeValues&)
		[[nodiscard]] bool GetPresetValues(const char* sKeyword, bool& noStop, Fn fn) noexcept
		{
			static_assert(std::is_invocable_r_v<void, Fn, Node::NodeValues&>, "Error: GetPresetValues expects a callable like void(NodeValues&)");

			auto& pairValues = FindKeyword(sKeyword);

			noStop = pairValues.first;

			for (auto& value : pairValues.second) {
				fn(value);
			}

			return true;
		}

	private:

		Preset() noexcept = default;
		~Preset() noexcept = default;

		void SetPairValue(Node::PairValue& pairValue, const Json::Value& jValue) noexcept;
		[[nodiscard]] PairNodes& FindKeyword(const char* sKeyword) noexcept;
		[[nodiscard]] Node::Flags GetFlags(const char* str) noexcept;
		[[nodiscard]] bool Load(const char* lpFilename) noexcept;

		std::unordered_map<Keyword, Furniture> map;
		PairNodes fallBack;
	};
}
