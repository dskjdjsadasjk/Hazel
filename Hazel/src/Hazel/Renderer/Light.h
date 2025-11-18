#pragma once

#include <glm/glm.hpp>

namespace Hazel {

	class Light
	{
	public:
		Light() = default;
		Light(glm::vec3 position);

		void SetLightPosition(glm::vec3 position) { m_Position = position; }
		glm::vec3 GetLightPosition() { return m_Position; }
	private:
		glm::vec3 m_Position;
	};
}
