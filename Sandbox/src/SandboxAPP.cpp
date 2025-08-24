#include<Hazel.h>
#include<Hazel/Core/EntryPoint.h>

#include "Platform/OpenGL/OpenGLShader.h"
#include"imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Sandbox2D.h"
class ExampleLayer :public Hazel::Layer
{
public:
	ExampleLayer()
		:Layer("Example"),m_CameraController(1280.f/720.f,true)
	{
		m_VertexArray=Hazel::VertexArray::Create();

		float vertices[3 * 7] = {
			-0.5f,-0.5f,0.0f,0.8f,0.2f,0.8f,1.0f,
			0.5f,-0.5f,0.0f,0.2f,0.3f,0.8f,1.0f,
			0.0f,0.5f,0.0f,0.8f,0.8f,0.2f,1.0f,
		};
		Hazel::Ref<Hazel::VertexBuffer> m_VertexBuffer;
		m_VertexBuffer=Hazel::VertexBuffer::Create(vertices, sizeof(vertices));

		Hazel::BufferLayout layout = {
			{Hazel::ShaderDataType::Float3,"a_Position"},
			{ Hazel::ShaderDataType::Float4,"a_Color" }
		};

		m_VertexBuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		unsigned int  indices[3] = { 0,1,2 };

		Hazel::Ref<Hazel::IndexBuffer> m_IndexBuffer;
		m_IndexBuffer=Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_SquareVA=Hazel::VertexArray::Create();
		float squarevertices[5 * 4] = {
			-0.5f, -0.5f,  0.0f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.0f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.0f,  0.0f, 1.0f
		};
		Hazel::Ref<Hazel::VertexBuffer> suqareVB;
		suqareVB=Hazel::VertexBuffer::Create(squarevertices, sizeof(squarevertices));
		suqareVB->SetLayout({
			{Hazel::ShaderDataType::Float3,"a_Position"},
			{Hazel::ShaderDataType ::Float2,"a_TexCoord"}
			});
		m_SquareVA->AddVertexBuffer(suqareVB);
		unsigned int  squareindices[6] = { 0,1,2,2,3,0 };

		Hazel::Ref<Hazel::IndexBuffer> squareIB;
		squareIB=Hazel::IndexBuffer::Create(squareindices, sizeof(squareindices) / sizeof(unsigned int));
		m_SquareVA->SetIndexBuffer(squareIB);
		std::string vertextSrc = R"(
			#version 330 core
			
			layout(location=0) in vec3 a_Position;
			layout(location=1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position=a_Position;
				v_Color=a_Color;
				gl_Position=u_ViewProjection*u_Transform*vec4(a_Position,1.0);
			}
		)";
		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location=0) out vec4 color;
			
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color=v_Color;
			}
		)";
		m_Shader=Hazel::Shader::Create("VertexColorTriangle",vertextSrc, fragmentSrc);
		std::string FlatColorvertextSrc = R"(
			#version 330 core
			
			layout(location=0) in vec3 a_Position;
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;
			
			void main()
			{
				gl_Position=u_ViewProjection*u_Transform*vec4(a_Position,1.0);
			}
		)";
		std::string FlatColorfragmentSrc = R"(
			#version 330 core
			
			layout(location=0) out vec4 color;
			uniform vec3 u_Color;
			void main()
			{
				color=vec4(u_Color,1.0f);
			}
		)";

		m_FlatColorShader=Hazel::Shader::Create("FlatColor",FlatColorvertextSrc, FlatColorfragmentSrc);
		
		Hazel::Ref<Hazel::Shader> m_TextureShader=m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);

		m_Texture=Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
		m_ChernoLogoTexture = Hazel::Texture2D::Create("assets/textures/Chernologo.png");
	}

	void OnUpdate(Hazel::Timestep ts) override
	{
		m_CameraController.OnUpdate(ts);

		Hazel::RenderCommand::SetClearColor({ 0.1f,0.1f,0.1f,1 });
		Hazel::RenderCommand::Clear();

		Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);
		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)*scale;
				Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		m_Texture->Bind();
		auto m_TextureShader = m_ShaderLibrary.Get("Texture");
		Hazel::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f),glm::vec3(1.5f)));
		m_ChernoLogoTexture->Bind();
		Hazel::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		//Hazel::Renderer::Submit(m_Shader, m_VertexArray);

		Hazel::Renderer::EndScene();
	}
	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Setting");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}
	void OnEvent(Hazel::Event& event) override
	{

		if (event.GetEventType() == Hazel::EventType::WindowResize)
		{
			auto re = (Hazel::WindowResizeEvent&)event;

			//float zoom = (float)re.GetHeight() / 720.f;
			//m_CameraController.SetZoomLevel(zoom);
		}
		m_CameraController.OnEvnet(event);
	}

	bool OnKeyPressedEvent(Hazel::KeyPressedEvent& event)
	{

		return false;
	}
private:
	Hazel::ShaderLibrary m_ShaderLibrary;
	Hazel::Ref<Hazel::VertexArray> m_VertexArray;
	Hazel::Ref<Hazel::Shader> m_Shader;

	Hazel::Ref<Hazel::VertexArray> m_SquareVA;
	Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	Hazel::Ref<Hazel::Texture2D> m_Texture;
	Hazel::Ref<Hazel::Texture2D> m_ChernoLogoTexture;

	Hazel::OrthographicCameraController m_CameraController;

	glm::vec3 m_SquareColor = { 0.2f,0.3f,0.8f };
};
class Sandbox: public Hazel::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
		//PushOverlay	(new Hazel::ImGuiLayer());
	}
	~Sandbox()
	{

	}
};
Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox();
}
