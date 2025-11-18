#include "hzpch.h"

#include "SkyBox.h"

namespace Hazel {
	SkyBox::SkyBox(std::vector<std::string>& paths)
	{
		skyTextureCube = TextureCube::Create(paths);
	}
}
