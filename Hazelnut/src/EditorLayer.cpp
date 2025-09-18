#include "EditorLayer.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>


namespace Hazel
{
	EditorLayer::EditorLayer()
		:Layer("EditorLayer"), m_CameraController(1280.f / 720.f, true)
	{

	}

	void EditorLayer::OnAttach()
	{
		HZ_PROFILE_FUNCTION();

		m_CheckboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
		// Init here
		
		Hazel::FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Hazel::Framebuffer::Create(fbSpec);
	}

	void EditorLayer::OnDetach()
	{
		HZ_PROFILE_FUNCTION();

	}

	void EditorLayer::OnUpdate(Hazel::Timestep ts)
	{
		HZ_PROFILE_FUNCTION();

		if (m_ViewportFocused)
		{
			m_CameraController.OnUpdate(ts);
		}

		Hazel::Renderer2D::ResetStats();
		{
			HZ_PROFILE_SCOPE("Renderer Prep");
			m_Framebuffer->Bind();
			Hazel::RenderCommand::SetClearColor({ 0.1f,0.1f,0.1f,1 });
			Hazel::RenderCommand::Clear();
		}
		{
			static float rotation = 0.0f;
			rotation += ts * 50.0f;

			HZ_PROFILE_SCOPE("Renderer Draw");

			Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
			Hazel::Renderer2D::DrawRotateQuad({ 1.0f,0.0f }, { 0.8f,0.8f }, glm::radians(-45.f), { 0.8f,0.2f,0.3f,1.0f });
			Hazel::Renderer2D::DrawQuad({ -1.0f,0.0f }, { 0.8f,0.8f }, { 0.8f,0.2f,0.3f,1.0f });
			Hazel::Renderer2D::DrawQuad({ 0.5f,-0.5f }, { 0.5f,0.75f }, { 0.2f,0.3f,0.8f,1.0f });
			Hazel::Renderer2D::DrawQuad({ 0.0f,0.0f,-0.1f }, { 20.0f,20.0f }, m_CheckboardTexture, 10.f);
			Hazel::Renderer2D::DrawRotateQuad({ -2.0f,0.0f,0.0f }, { 1.0f,1.0f }, glm::radians(rotation), m_CheckboardTexture, 20.f);
			Hazel::Renderer2D::EndScene();

			Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
			for (float y = -5.0f; y < 5.f; y += 0.5f)
			{
				for (float x = -5.0f; x < 5.f; x += 0.5f)
				{
					glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
					Hazel::Renderer2D::DrawQuad({ x,y }, { 0.45f,0.45f }, color);
				}
			}
			Hazel::Renderer2D::EndScene();
		}


		if (Hazel::Input::MouseButtonPressed(HZ_MOUSE_BUTTON_LEFT))
		{
			auto [x, y] = Hazel::Input::GetMousePosition();
			auto width = Hazel::Application::Get().GetWindow().GetWidth();
			auto height = Hazel::Application::Get().GetWindow().GetHeight();

			auto bounds = m_CameraController.GetBounds();
			auto pos = m_CameraController.GetCamera().GetPosition();
			x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
			y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		}

		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Hazel::Renderer2D::EndScene();
		m_Framebuffer->UnBind();


	}

	void EditorLayer::OnImGuiRender()
	{
		HZ_PROFILE_FUNCTION();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
		bool opt_fullscreen = opt_fullscreen_persistant;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Exit")) Hazel::Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::Begin("Setting");

		auto stats = Hazel::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("DrawCalls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin("Viewport");

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
		{
			m_ViewportSize = { viewportPanelSize.x,viewportPanelSize.y };
			m_Framebuffer->Resize((uint32_t) m_ViewportSize.x,(uint32_t)m_ViewportSize.y);

			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
		}
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x,m_ViewportSize.y },ImVec2{0,1},ImVec2{1,0});
		ImGui::End();
		ImGui::PopStyleVar();

		/*
		for (auto& result : m_ProfileResults)
		{
			char label[50];
			strcpy(label, "%.3fms ");
			strcat(label, result.Name);
			ImGui::Text(label, result.Time);
		}
		m_ProfileResults.clear();*/

		ImGui::End();
	}

	void EditorLayer::OnEvent(Hazel::Event& event)
	{
		m_CameraController.OnEvnet(event);
	}
}

