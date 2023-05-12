#pragma once

#include "version.h"

#include "node.h"

#include "json/json.h"

namespace Fixes {

	struct Values {

		std::string node;
		Node::Flags flags{};
		float value{};
		bool mulheight{};
	};

	constexpr char* FilePreset = "F4SE\\Plugins\\hhs_fixes.json";

	class Preset {

	public:

		static [[nodiscard]] Preset& GetInstance() noexcept { return instance; }

		[[nodiscard]] bool Load() noexcept;

		[[nodiscard]] const std::int32_t& GetSAFVersion() const noexcept { return saf_version; }

		[[nodiscard]] bool FindKeyword(const std::string& sKeyword, Json::Value& value) noexcept;

		template<typename Fn = std::function<void(const Values&)>>
		[[nodiscard]] bool GetPresetValues(const std::string& sKeyword, bool& noStop, Fn fn) noexcept
		{
			Json::Value objs;

			if (!FindKeyword(sKeyword, objs))
				return false;

			for (auto& obj : objs.getMemberNames()) {

				auto& ref_obj = objs[obj];

				if (obj == "nostop" && ref_obj.isBool()) {

					noStop = ref_obj.asBool();
				
					continue;
				} 

				if (!ref_obj.isObject())
					continue;

				for (auto& flag : ref_obj.getMemberNames()) {

					auto& value = ref_obj[flag];

					if (!value.isObject())
						continue;

					Values values{ obj.c_str(), GetFlags(flag), value["value"].asFloat(), value["mulheight"].asBool() };

					fn(values);
				}
			}

			return true;
		}

	private:

		Preset() noexcept {}
		~Preset() noexcept {}

		Preset(const Preset&) = delete;
		Preset(Preset&&) = delete;

		Preset& operator=(const Preset&) = delete;
		Preset& operator=(Preset&&) = delete;

		[[nodiscard]] Node::Flags GetFlags(const std::string& str) noexcept;

		Json::Value root;

		std::int32_t saf_version{};

		static Preset instance;
	};
}
