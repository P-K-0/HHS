#pragma once

#include "version.h"

#include "node.h"
#include "util.h"

#include "json/json.h"

namespace Fixes {

	struct Values {

		std::string node;
		Node::Flags flags{};
		float value{};
		bool mulheight{};
	};

	constexpr char* FilePreset = "F4SE\\Plugins\\hhs_fixes.json";

	class Preset :
		public util::Singleton<Preset> {
		friend class util::Singleton<Preset>;

	public:

		[[nodiscard]] bool Load() noexcept;

		[[nodiscard]] std::int32_t GetSAFVersion() const noexcept { return saf_version; }

		[[nodiscard]] bool FindKeyword(const std::string& sKeyword, Json::Value& value) noexcept;

		template<typename Fn = std::function<void(const Values&)>>
		[[nodiscard]] bool GetPresetValues(const std::string& sKeyword, bool& noStop, Fn fn) noexcept
		{
			Json::Value objs;

			if (!FindKeyword(sKeyword, objs)) {
				return false;
			}

			for (auto& obj : objs.getMemberNames()) {

				auto& ref_obj = objs[obj];

				if (obj == "nostop" && ref_obj.isBool()) {
					noStop = ref_obj.asBool();
				}
				else {

					if (ref_obj.isObject()) {

						for (auto& flag : ref_obj.getMemberNames()) {

							auto& value = ref_obj[flag];

							if (value.isObject()) {

								Values values{ obj.c_str(), GetFlags(flag), value["value"].asFloat(), value["mulheight"].asBool() };

								fn(values);
							}
						}
					}
				}
			}

			return true;
		}

	private:

		Preset() noexcept = default;
		~Preset() noexcept = default;

		[[nodiscard]] Node::Flags GetFlags(const std::string& str) noexcept;

		Json::Value root;

		std::int32_t saf_version{};
	};
}
