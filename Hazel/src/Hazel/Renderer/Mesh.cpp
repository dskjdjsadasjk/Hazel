#include "hzpch.h"

#include "Mesh.h"

Hazel::Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Ref<Texture2D>> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	TriangleVertexArray = VertexArray::Create();
	TriangleVertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(Vertex));
	TriangleVertexBuffer->SetLayout({
		{ShaderDataType::Float3,"a_Position"},
		{ShaderDataType::Float3,"a_Normal"},
		{ShaderDataType::Float2,"a_TexCoord"},
		});
	TriangleVertexArray->AddVertexBuffer(TriangleVertexBuffer);
	TriangleIB = IndexBuffer::Create(indices.data(), indices.size());
	TriangleVertexArray->SetIndexBuffer(TriangleIB);




	uint32_t dataSize = vertices.size() * sizeof(Vertex) / (sizeof(uint8_t));
	//uint32_t dataSize = (uint32_t)((uint8_t*)(src.vertices.data()+src.vertices.size())- (uint8_t*)src.vertices.data());
	TriangleVertexBuffer->SetData(vertices.data(), dataSize);
}
