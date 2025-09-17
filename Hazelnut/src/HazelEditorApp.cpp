#include<Hazel.h>
#include<Hazel/Core/EntryPoint.h>

#include "Platform/OpenGL/OpenGLShader.h"
#include"imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "EditorLayer.h"

namespace Hazel
{
	class Hazelnut : public Application
	{
	public:
		Hazelnut()
			:Application("Hazel Hazelnut")
		{
			//PushLayer(new ExampleLayer());
			PushLayer(new EditorLayer());
			//PushOverlay	(new Hazel::ImGuiLayer());
		}
		~Hazelnut()
		{

		}
	};
	Application* Hazel::CreateApplication()
	{
		return new Hazelnut();
	}
}

