
#include "skeleton.h"

#include "file.h"

namespace Skeleton {

	inline bool operator==(const BSFixedString& bStr, const std::string& str) noexcept
	{
		auto cstr = bStr.c_str();
		return cstr && str == cstr;
	}

	inline bool operator==(const std::string& str, const BSFixedString& bStr) noexcept
	{
		return bStr == str;
	}

	inline bool operator!=(const std::string& str, const BSFixedString& bStr) noexcept
	{
		return !(bStr == str);
	}

	inline bool operator!=(const BSFixedString& bStr, const std::string& str) noexcept
	{
		return str != bStr;
	}

	NiStreamPtr::NiStreamPtr() noexcept
	{
		mem = std::make_unique<std::uint8_t[]>(sizeof(NiStream));

		niStream = reinterpret_cast<NiStream*>(mem.get());

		if (niStream) {
			CALL_MEMBER_FN(niStream, ctor)();
		}
	}

	NiStreamPtr::~NiStreamPtr() noexcept
	{
		if (niStream) {
			CALL_MEMBER_FN(niStream, dtor)();
		}
	}

	bool Reader::Init(const std::string& Filename) noexcept
	{
		if (!niStream) {
			return false;
		}

		auto file{ File::GetRelativeDir(Filename) };

		BSResourceNiBinaryStream binaryStream(file.c_str());

		if (!binaryStream.IsValid()) {
			return false;
		}

		if (!niStream->LoadStream(&binaryStream) || !niStream->root_objects.m_data) {
			return false;
		}

		return true;
	}

	Reader::Reader(const std::string& Filename) noexcept
	{
		isValid = Init(Filename);
	}

	Reader::Reader(Actor* actor, bool isFemale) noexcept
	{
		if (!actor) {
			return;
		}

		std::uint32_t female = isFemale ? 1 : 0;

		auto race{ actor->GetActorRace() };

		if (!race) {
			return;
		}

		isValid = Init(race->models[female].GetModelName());
	}

	Reader::Reader(TESObjectREFR* refr, bool isFemale) noexcept
	{
		Actor* actor{ nullptr };

		if (!refr || !(actor = DYNAMIC_CAST(refr, TESObjectREFR, Actor))) {
			return;
		}

		std::uint32_t female = isFemale ? 1 : 0;

		auto race{ actor->GetActorRace() };

		if (!race) {
			return;
		}

		isValid = Init(race->models[female].GetModelName());
	}

	template<typename Func>
	bool Reader::Visit(NiAVObject* obj, Func fn) noexcept
	{
		if (!obj) {
			return false;
		}

		auto node = obj->GetAsNiNode();

		if (!node) {
			return false;
		}

		for (std::uint32_t idx{}; idx < node->m_children.m_emptyRunStart; idx++) {

			auto child = node->m_children.m_data[idx];

			if (child) {

				if (fn(child) || Visit(child, fn)) {
					return true;
				}
			}
		}

		return false;
	}

	template<typename Func>
	bool Reader::Visit(Func fn) noexcept
	{
		if (!isValid) {
			return false;
		}

		for (std::uint32_t idx{}; idx < niStream->root_objects.m_emptyRunStart; idx++) {

			auto obj = niStream->root_objects.m_data[idx];

			if (obj) {

				auto node = obj->GetAsNiNode();

				if (node) {

					return Visit(node, [&](NiAVObject* obj) {

						if (fn(obj)) {
							return true;
						}

						return false;
					});
				}
			}
		}

		return false;
	}

	bool Reader::HasNode(const std::string& name) noexcept
	{
		return Visit([&](NiAVObject* obj) {

			if (name == obj->m_name) {
				return true;
			}

			return false;
		});
	}

	bool Reader::GetExtraData(const std::string& name, float& value) noexcept
	{
		return Visit([&](NiAVObject* obj) {

			if (!obj->m_extraData) {
				return false;
			}

			SimpleLocker lock(&obj->m_extraData->lock);

			for (std::uint32_t idx{}; idx < obj->m_extraData->count; idx++) {

				NiExtraData* extraData{ nullptr };

				obj->m_extraData->GetNthItem(idx, extraData);

				if (!extraData) {
					continue;
				}

				auto data = DYNAMIC_CAST(extraData, NiExtraData, NiFloatExtraData);

				if (!data || name != data->m_name) {
					continue;
				}

				value = data->value;

				return true;
			}

			return false;
		});
	}

	bool Reader::GetExtraData(const std::string& name, std::uint32_t& value) noexcept
	{
		return Visit([&](NiAVObject* obj) {

			if (!obj->m_extraData) {
				return false;
			}

			SimpleLocker lock(&obj->m_extraData->lock);

			for (std::uint32_t idx{}; idx < obj->m_extraData->count; idx++) {

				NiExtraData* extraData{ nullptr };

				obj->m_extraData->GetNthItem(idx, extraData);

				if (!extraData) {
					continue;
				}

				auto data = DYNAMIC_CAST(extraData, NiExtraData, NiIntegerExtraData);

				if (!data || name != data->m_name) {
					continue;
				}

				value = data->value;

				return true;
			}

			return false;
		});
	}

	float GetHeightFromSkeleton(const std::string& filename) noexcept
	{
		if (!Settings::Ini::GetSingleton().Get_bEnableExtraData()) {
			return ZeroValue;
		}

		auto height{ ZeroValue };

		Skeleton::Reader reader{ filename };

		reader.GetExtraData(ExtraDataHHS, height);

		return height;
	}
}
