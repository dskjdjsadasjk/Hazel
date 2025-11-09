#pragma once

#include <xhash>

namespace Hazel {

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};
}

namespace std {

	//可用于优化，防止生成的UID相同
	template<>
	struct hash<Hazel::UUID>
	{
		std::size_t operator()(const Hazel::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}
