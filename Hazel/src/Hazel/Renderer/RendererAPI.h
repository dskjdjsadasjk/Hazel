#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Hazel {

	//暂时不清楚放在这里是否不合适
	enum class DepthType
	{
		LESS, LEQUAL
	};

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};
	public:
		virtual void Init() = 0; 
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void SetDepth(DepthType depth) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)=0;
		virtual void DrawArrays(uint32_t arrayBegin, uint32_t arrayCount) = 0;

		inline static API GetAPI() { return s_API; }

		virtual ~RendererAPI() = default;
	private:
		static API s_API;
	};
}