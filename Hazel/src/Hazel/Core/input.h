#pragma once

#include "Hazel/Core/Core.h"
#include "Hazel/Core/KeyCode.h"

namespace Hazel {
	class HAZEL_API Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool MouseButtonPressed(int button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
