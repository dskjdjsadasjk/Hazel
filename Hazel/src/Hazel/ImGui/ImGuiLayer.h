#pragma once
#include "Hazel/Core/Layer.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/ApplicationEvent.h"
namespace Hazel {

	class HAZEL_API ImGuiLayer :public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach();
		virtual void OnDetach();
		//virtual void OnImGuiRender();
		void Begin();
		void End();
		virtual void OnEvent(Event& e);

		void BlockEvents(bool block) { m_BlockEvents = block; }
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}

