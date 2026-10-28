#pragma once

#include <glm/glm.hpp>

namespace Hazel {

	class Light
	{
	public:
		enum class LightType
		{
			Point = 0,
			Directional = 1,
		};
	public:
		Light() = default;

		LightType GetLightType() const { return lightType; }
		void SetLightType(LightType type) { lightType = type; }
		float GetIntentisy() const { return lightIntensity; }
		void SetIntensity(float intensity) { this->lightIntensity = intensity; }
		glm::vec3 GetLightColor() const { return lightColor; }
		void SetLightColor(glm::vec3 color) { lightColor = color; }
		float GetLightRange() const { return lightRange; }
		void SetLightRange(float range) { lightRange = range; }
	private:
		float lightIntensity = 1.0f;
		float lightRange = 25.0f;
		glm::vec3 lightColor = glm::vec3(1.0f);
		LightType lightType = LightType::Point;
	};
}
