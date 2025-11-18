#pragma once

#include "Hazel/Core/Core.h"
#include "Mesh.h"
#include "Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Hazel {

	class Model
	{
	public:
		Model() = default;
		Model(const char* path)
		{
			loadModel(path);
		}

		void loadModel(const std::string& path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	public:
		std::vector<Mesh> meshes;

	private:
		Ref<Texture2D> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);
	private:
		std::vector<Ref<Texture2D>> textures_loaded;
		std::string directory;
	};
}
