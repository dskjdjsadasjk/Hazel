#pragma once

#include "Hazel.h"                                                                                                                                                                                                                                                                                                                            
#include "Panels/SceneHierarchyPanel.h"
#include "Hazel/Renderer/EditorCamera.h"

namespace Hazel
{
	class EditorLayer :public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& event) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
	private:
		OrthographicCameraController m_CameraController;

		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<Framebuffer> m_Framebuffer;

		Entity m_SquareEntity;
		Entity m_SecondCamera;
		Entity m_CameraEntity;
		Entity m_HoveredEntity;
		Ref<Scene> m_ActiveScene;

		bool m_PrimaryCamera = true;

		EditorCamera m_EditorCamera;

		Ref<Texture2D> m_CheckboardTexture;

		bool m_ViewportFocused = false, m_ViewportHovered = false;;
		glm::vec2 m_ViewportSize = { 0,0 };
		glm::vec2 m_ViewportBounds[2];

		glm::vec4 m_SquareColor = { 0.2f,0.3f,0.8f,1.0f };

		int m_GizmoType = -1;

		SceneHierarchyPanel m_SceneHierarchyPanel;
	};
}



