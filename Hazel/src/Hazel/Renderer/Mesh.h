#pragma once
#include <glm/glm.hpp>
#include "Hazel/Core/Core.h"
#include "Texture.h"

#include "VertexArray.h"
namespace Hazel {
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    class Mesh
    {
    public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        std::vector<Ref<Texture2D>> textures;

        Ref<VertexArray> TriangleVertexArray;
        Ref<VertexBuffer> TriangleVertexBuffer;
        Ref<IndexBuffer> TriangleIB;

        Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Ref<Texture2D>>);
    };
}

