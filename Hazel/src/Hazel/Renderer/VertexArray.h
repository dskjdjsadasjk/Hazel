#pragma once

#include <memory>
#include "Hazel/Renderer/Buffer.h"

namespace Hazel {
	class VertexArray
	{
	public:
		virtual ~VertexArray(){}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexbuffer) = 0;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexbuffer) = 0;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffers() const = 0;

		static VertexArray* Create();
	};
}
