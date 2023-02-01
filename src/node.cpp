
#include "node.h"

namespace Node {

	void Transform::ResetTransform(const std::string& sNode) noexcept
	{
		for (std::uint32_t idx{}; idx < static_cast<std::uint32_t>(Flags::Count); idx++)
			ResetTransform(sNode, static_cast<Flags>(idx));
	}

	std::int32_t Transform::ResetTransform(const std::string& node, const Flags& flags) noexcept
	{
		auto& m = map[node];

		if (!m.second.second[flags])
			return -1;

		return Visit(node, true, [&](NiAVObject* obj) {

			m.second.second[flags] = false;

			bool res{};

			for (auto& v : m.second.second)
				res |= v;

			if (!res)
				m.first = false;

			switch (flags) {

			case Flags::PosX:

				obj->m_localTransform.pos.x = m.second.first.pos.x;

				break;

			case Flags::PosY:

				obj->m_localTransform.pos.y = m.second.first.pos.y;

				break;

			case Flags::PosZ:

				obj->m_localTransform.pos.z = m.second.first.pos.z;

				break;

			case Flags::Scale:

				obj->m_localTransform.scale = m.second.first.scale;

				break;

			case Flags::RotX:

				SetEulerAngle(m.second.first, obj->m_localTransform, Angle::Heading);

				break;

			case Flags::RotY:

				SetEulerAngle(m.second.first, obj->m_localTransform, Angle::Attitude);

				break;

			case Flags::RotZ:

				SetEulerAngle(m.second.first, obj->m_localTransform, Angle::Bank);

				break;
			}
		});
	}

	std::int32_t Transform::SetTransform(const std::string& node, const Flags& flags, const float& value) noexcept
	{
		auto& m = map[node];

		return Visit(node, true, [&](NiAVObject* obj) {

			if (!m.first) {

				m.first = true;
				m.second.first = obj->m_localTransform;
			}

			m.second.second[flags] = true;

			switch (flags) {

			case Flags::PosX:

				obj->m_localTransform.pos.x = m.second.first.pos.x;
				obj->m_localTransform.pos.x += value;

				break;

			case Flags::PosY:

				obj->m_localTransform.pos.y = m.second.first.pos.y;
				obj->m_localTransform.pos.y += value;

				break;

			case Flags::PosZ:

				obj->m_localTransform.pos.z = m.second.first.pos.z;
				obj->m_localTransform.pos.z += value;

				break;

			case Flags::Scale:

				obj->m_localTransform.scale = value;

				break;

			case Flags::RotX:

				SetEulerAngle(m.second.first, obj->m_localTransform, Angle::Heading, value);

				break;

			case Flags::RotY:

				SetEulerAngle(m.second.first, obj->m_localTransform, Angle::Attitude, value);

				break;

			case Flags::RotZ:

				SetEulerAngle(m.second.first, obj->m_localTransform, Angle::Bank, value);

				break;
			}
		});
	}

	float Transform::GetTransform(const std::string& node, const Flags& flags) noexcept
	{
		float value{};

		bool ret = Visit(node, false, [&](NiAVObject* obj) {

			switch (flags) {

			case Flags::PosX:

				value = obj->m_localTransform.pos.x;

				break;

			case Flags::PosY:

				value = obj->m_localTransform.pos.y;

				break;

			case Flags::PosZ:

				value = obj->m_localTransform.pos.z;

				break;

			case Flags::Scale:

				value = obj->m_localTransform.scale;

				break;

			case Flags::RotX:

				value = GetEulerAngle(obj->m_localTransform, Angle::Heading);

				break;

			case Flags::RotY:

				value = GetEulerAngle(obj->m_localTransform, Angle::Attitude);

				break;

			case Flags::RotZ:

				value = GetEulerAngle(obj->m_localTransform, Angle::Bank);

				break;
			}
		});

		return value;
	}

	float Transform::GetEulerAngle(NiTransform& niTransform, const Angle& angle) noexcept
	{
		float h, a, b;

		niTransform.rot.GetEulerAngles(&h, &a, &b);

		switch (angle) {

		case Angle::Heading: return h;

		case Angle::Attitude: return a;

		case Angle::Bank: return b;

		}

		return 0.0f;
	}

	void Transform::SetEulerAngle(NiTransform& src, NiTransform& dst, const Angle& angle, const float& value) noexcept
	{
		float h0, a0, b0;

		dst.rot.GetEulerAngles(&h0, &a0, &b0);

		float h1, a1, b1;

		src.rot.GetEulerAngles(&h1, &a1, &b1);

		switch (angle) {

		case Angle::Heading:

			h1 += value;
			a1 = a0;
			b1 = b0;

			break;

		case Angle::Attitude:

			h1 = h0;
			a1 += value;
			b1 = b0;

			break;

		case Angle::Bank:

			h1 = h0;
			a1 = a0;
			b1 += value;

			break;
		}

		dst.rot.SetEulerAngles(h1, a1, b1);
	}

	template<typename Func>
	std::int32_t Transform::Visit(const std::string& sNode, const bool update, Func fn) noexcept
	{
		NiNode* root{ nullptr };

		if (!act || !(root = act->GetActorRootNode(firstPerson)))
			return -1;

		BSFixedString node_str{ sNode.c_str() };

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		NiAVObject* object = root->GetObjectByName(&node_str);
#else
		NiAVObject* object = CALL_MEMBER_FN(root, GetAVObjectByName)(&node_str, 0, 0);
#endif

		if (!object)
			return -2;

		fn(object);

		if (update) {

			NiAVObject::NiUpdateData ctx;
			ctx.flags = 0;
			object->UpdateWorldData(&ctx);
		}

		return 0;
	}
}
