
#include "node.h"

namespace Node {

	void Pool::TryAddingToPool(const char* node) noexcept
	{
		auto key = chash(node);

		TryAddingToPool(node, key);
	}

	void Pool::TryAddingToPool(const char* node, std::size_t key) noexcept
	{
		auto it = pool.find(key);

		if (it != pool.end()) {
			return;
		}

		pool[key] = node;
	}

	const std::string& Pool::GetFromPool(std::size_t key) noexcept
	{
		auto it = pool.find(key);

		if (it != pool.end()) {
			return it->second;
		}

		return strEmpty;
	}

	void Transform::ResetTransform(std::size_t key) noexcept
	{
		auto& node = Pool::GetSingleton().GetFromPool(key);

		if (node.empty()) {
			return;
		}

		auto& m = map[key];

		if (m.flags == 0) {
			return;
		}

		Visit(node.c_str(), key, true, [&](NiAVObject* obj) {

			obj->m_localTransform = m.transform;

			m.flags = 0;
			m.isSet = false;
		});
	}

	void Transform::ResetTransform(const char* node) noexcept 
	{
		auto key = chash(node);
		auto& m = map[key];

		if (m.flags == 0) {
			return;
		}

		Visit(node, key, true, [&](NiAVObject* obj) {

			obj->m_localTransform = m.transform;

			m.flags = 0;
			m.isSet = false;
		});
	}

	std::int32_t Transform::ResetTransform(const char* node, Flags flags) noexcept
	{
		auto key = chash(node);
		auto& m = map[key];
		auto index = static_cast<std::uint32_t>(flags);

		std::uint8_t bitmask = (1 << index);

		if ((m.flags & bitmask) == 0) {
			return -1;
		}

		return Visit(node, key, true, [&](NiAVObject* obj) {

			m.flags &= ~(bitmask);

			if (m.flags == 0) {
				m.isSet = false;
			}

			switch (flags) {

			case Flags::PosX:

				obj->m_localTransform.pos.x = m.transform.pos.x;

				break;

			case Flags::PosY:

				obj->m_localTransform.pos.y = m.transform.pos.y;

				break;

			case Flags::PosZ:

				obj->m_localTransform.pos.z = m.transform.pos.z;

				break;

			case Flags::Scale:

				obj->m_localTransform.scale = m.transform.scale;

				break;

			case Flags::RotX:

				SetEulerAngle(m.transform, obj->m_localTransform, Angle::Heading);

				break;

			case Flags::RotY:

				SetEulerAngle(m.transform, obj->m_localTransform, Angle::Attitude);

				break;

			case Flags::RotZ:

				SetEulerAngle(m.transform, obj->m_localTransform, Angle::Bank);

				break;
			}
		});
	}

	std::int32_t Transform::SetTransform(const char* node, NodeValues& values, float height) noexcept
	{
		auto key = chash(node);
		auto& m = map[key];
		
		return Visit(node, key, true, [&](NiAVObject* obj) {

			if (!m.isSet) {

				m.isSet = true;
				m.transform = obj->m_localTransform;
			}

			m.flags |= GetAllBitFlags();

			obj->m_localTransform = m.transform;

			obj->m_localTransform.pos.x += values.x.GetMulValue(height);
			obj->m_localTransform.pos.y += values.y.GetMulValue(height);
			obj->m_localTransform.pos.z += values.z.GetMulValue(height);

			if (values.scale.value >= 0.0f) {
				obj->m_localTransform.scale = values.scale.GetMulValue(height);
			}

			float h, a, b;

			obj->m_localTransform.rot.GetEulerAngles(&h, &a, &b);

			h += values.heading.GetMulValue(height);
			a += values.attitude.GetMulValue(height);
			b += values.bank.GetMulValue(height);

			obj->m_localTransform.rot.SetEulerAngles(h, a, b);
		});
	}

	std::int32_t Transform::SetTransform(const char* node, Flags flags, float value) noexcept
	{
		auto key = chash(node);
		auto& m = map[key];
		auto index = static_cast<std::uint32_t>(flags);

		return Visit(node, key, true, [&](NiAVObject* obj) {

			if (!m.isSet) {

				m.isSet = true;
				m.transform = obj->m_localTransform;
			}

			m.flags |= (1 << index);

			switch (flags) {

			case Flags::PosX:

				obj->m_localTransform.pos.x = m.transform.pos.x;
				obj->m_localTransform.pos.x += value;

				break;

			case Flags::PosY:

				obj->m_localTransform.pos.y = m.transform.pos.y;
				obj->m_localTransform.pos.y += value;

				break;

			case Flags::PosZ:

				obj->m_localTransform.pos.z = m.transform.pos.z;
				obj->m_localTransform.pos.z += value;

				break;

			case Flags::Scale:

				obj->m_localTransform.scale = value;

				break;

			case Flags::RotX:

				SetEulerAngle(m.transform, obj->m_localTransform, Angle::Heading, value);

				break;

			case Flags::RotY:

				SetEulerAngle(m.transform, obj->m_localTransform, Angle::Attitude, value);

				break;

			case Flags::RotZ:

				SetEulerAngle(m.transform, obj->m_localTransform, Angle::Bank, value);

				break;
			}
		});
	}

	float Transform::GetTransform(const char* node, Flags flags) noexcept
	{
		float value{};

		auto ret = Visit(node, chash(node), false, [&](NiAVObject* obj) {

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

		return ret == 0 ? value : 0.0f;
	}

	float Transform::GetEulerAngle(NiTransform& niTransform, Angle angle) noexcept
	{
		float h, a, b;

		niTransform.rot.GetEulerAngles(&h, &a, &b);

		switch (angle) {

		case Angle::Heading: 
			return h;

		case Angle::Attitude:
			return a;

		case Angle::Bank: 
			return b;

		}

		return 0.0f;
	}

	void Transform::SetEulerAngle(NiTransform& src, NiTransform& dst, Angle angle, float value) noexcept
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

	template<typename Fn>
	std::int32_t Transform::Visit(const char* node, std::size_t key, bool update, Fn fn) noexcept
	{
		static_assert(std::is_invocable_r_v<void, Fn, NiAVObject*>, "Error: Visit expects a callable like void(NiAVObject*)");

		Pool::GetSingleton().TryAddingToPool(node, key);

		NiNode* root{ nullptr };

		if (!act || !(root = act->GetActorRootNode(frstPerson))) {
			return -1;
		}

		BSFixedString bsNode{ node };

#if RUNTIME_VR_VERSION_1_2_72 != CURRENT_RELEASE_RUNTIME
		auto object = root->GetObjectByName(&bsNode);
#else
		auto object = CALL_MEMBER_FN(root, GetAVObjectByName)(&bsNode, 0, 0);
#endif

		if (!object) {
			return -2;
		}

		fn(object);

		if (update) {

			NiAVObject::NiUpdateData ctx{};

			object->UpdateWorldData(&ctx);
		}

		return 0;
	}
}
