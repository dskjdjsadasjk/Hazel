#pragma once

#include "Hazel/Renderer/Texture.h"

namespace Hazel {

	class SkyBox
	{
	public:
		SkyBox() = default;
		SkyBox(std::vector<std::string>& paths);

		
	public:
		Ref<TextureCube> skyTextureCube;
	};
}
