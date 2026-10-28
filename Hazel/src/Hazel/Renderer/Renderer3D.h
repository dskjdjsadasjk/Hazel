#pragma once

#include "OrthographicCamera.h"

#include "Texture.h"
#include "Camera.h"
#include "Hazel/Renderer/EditorCamera.h"
#include "Hazel/Renderer/Light.h"
#include "Mesh.h"
#include "Model.h"
#include "Hazel/Scene/SkyBox.h"

#include "Hazel/Scene/Components.h"


namespace Hazel {

	class Renderer3D
	{
	public:
		static void Init();

		static void BeginScene(const EditorCamera& camera);
		static void BeginSceneShadow();
		static void EndScene();
		static void StartBatch();
		static void SetLight(const glm::vec3& position, Light& light);

		static void DrawRotateQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f), float tilingFactor=1.0f);

		static void DrawModel(const glm::mat4& transform, Model& src, int entityID);
		static void DrawMesh(const glm::mat4& transform, Mesh& src, int entityID);

		static void DrawSkyBox(SkyBox& skyBox);
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};
		static void ResetStats();
		static Statistics GetStats();

	private:
		static void Flush();
		static void NextBatch();
	};
}

