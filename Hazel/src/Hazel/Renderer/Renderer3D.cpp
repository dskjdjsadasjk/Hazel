#include "hzpch.h"

#include "Renderer3D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Hazel/Renderer/UniformBuffer.h"
#include "RenderCommand.h"
#include "Framebuffer.h"

//É¾³ý
#include <glad/glad.h>
namespace Hazel {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		int EntityID;
	};

	struct TriangleVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;

		int EntityID;
	};

	struct Renderer3DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTexturesSlots = 32;

		static const uint32_t MaxTriangles = 20000;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;
		Ref<Texture2D> WhiteTexture;
		glm::vec4 QuadVertexPositions[4];
		std::array<Ref<Texture2D>, MaxTexturesSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		Ref<Shader> TriangleShader;
		Ref<VertexArray> TriangleVertexArray;
		Ref<VertexBuffer> TriangleVertexBuffer;
		Ref<IndexBuffer> TriangleIB;

		Ref<Shader> SkyBoxShader;
		Ref<VertexArray> SkyBoxVertexArray;
		Ref<VertexBuffer> SkyBoxBuffer;
		glm::mat4 SkyViewProjection;

		Ref<Shader> ShadowDepthShader;
		Ref<Framebuffer> ShadowFramebuffer;
		Ref<TextureCube> ShadowCubeTexture; 
		
		static const int ShadowWidth = 1024;
		static const int ShadowHeight = 1024;
		bool IsShadow = false;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

		struct LightData
		{
			glm::vec3 LightPosition;
		};
		LightData LightBuffer;
		Ref<UniformBuffer> LightUniformBuffer;

		glm::vec3 viewPosition;
		Ref<UniformBuffer> ViewPosUniformBuffer;

		Renderer3D::Statistics Stats;
	};

	static Renderer3DData s_Data;
	
	void Renderer3D::Init()
	{
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::CameraData), 0);
		s_Data.LightUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::LightData), 1);
		s_Data.ViewPosUniformBuffer = UniformBuffer::Create(sizeof(glm::vec3), 2);

		s_Data.QuadShader = Shader::Create("assets/shaders/Renderer3D_Quad.glsl");
		s_Data.QuadVertexArray = VertexArray::Create();
		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3,"a_Position"},
			{ ShaderDataType::Float4,"a_Color"},
			{ ShaderDataType::Float2,"a_TexCoord"},
			{ ShaderDataType::Float,"a_TexIndex"},
			{ ShaderDataType::Float,"a_TilingFactor"},
			{ ShaderDataType::Int,"a_EntityID" }
			});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> squareIB;
		squareIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(squareIB);
		delete[] quadIndices;

		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f,-0.5f,0.0f,1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f,-0.5f,0.0f,1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f, 0.5f,0.0f,1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f, 0.5f,0.0f,1.0f };

		s_Data.TriangleShader = Shader::Create("assets/shaders/Blinn-Phong.glsl");

		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
		s_Data.SkyBoxShader = Shader::Create("assets/shaders/Renderer3D_SkyBox.glsl");
		s_Data.SkyBoxVertexArray = VertexArray::Create();
		s_Data.SkyBoxBuffer = VertexBuffer::Create(sizeof(float) * 180);
		s_Data.SkyBoxBuffer->SetLayout({
			{ShaderDataType::Float3,"a_Position"},
			});
		s_Data.SkyBoxVertexArray->AddVertexBuffer(s_Data.SkyBoxBuffer);
		s_Data.SkyBoxBuffer->SetData(skyboxVertices, sizeof(float) * 108 / sizeof(uint8_t));

		s_Data.ShadowDepthShader = Shader::Create("assets/shaders/ShadowDepth.glsl");
		FramebufferSpecification fbShadowSpec;
		fbShadowSpec.Attachments = { FramebufferTextureFormat::Depth };
		fbShadowSpec.Width = 1024;
		fbShadowSpec.Height = 1024;
		fbShadowSpec.Target = FramebufferTextureTarget::TextureCube;
		s_Data.ShadowFramebuffer = Framebuffer::Create(fbShadowSpec);
		s_Data.ShadowCubeTexture = TextureCube::Create(1024, 1024, s_Data.ShadowFramebuffer->GetDepthAttachmentRendererID());
	}

	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.IsShadow = false;
		
		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DData::CameraData));
		s_Data.TriangleShader->Bind();
		s_Data.viewPosition = camera.GetPosition();
		s_Data.ViewPosUniformBuffer->SetData(&s_Data.viewPosition, sizeof(glm::vec3));
		s_Data.ShadowCubeTexture->Bind(2);

		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glm::mat4 projection = camera.GetProjection();
		s_Data.SkyViewProjection = projection * view;
		
		s_Data.IsShadow = false;

		StartBatch();
	}

	void Renderer3D::BeginSceneShadow(Light& light)
	{
		s_Data.IsShadow = true;
		s_Data.ShadowFramebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f,0.1f,0.1f,1 });
		RenderCommand::Clear();

		glm::vec3 lightPos = light.GetLightPosition();
		s_Data.ShadowDepthShader->Bind();
		float near_plane = 0.01f;
		float far_plane = 25.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)s_Data.ShadowWidth / (float)s_Data.ShadowHeight, near_plane, far_plane);
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		for (unsigned int i = 0; i < 6; ++i)
			s_Data.ShadowDepthShader->SetMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		s_Data.ShadowDepthShader->SetFloat("far_plane", far_plane);
		s_Data.ShadowDepthShader->SetFloat3("lightPos", lightPos);
	}

	void Renderer3D::StartBatch()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer3D::SetLight(const glm::vec3& position)
	{
		s_Data.LightBuffer.LightPosition = position;
		s_Data.LightUniformBuffer->SetData(&s_Data.LightBuffer, sizeof(Renderer3DData::LightData));
		float near_plane = 1.0f;
		float far_plane = 25.0f;
		s_Data.TriangleShader->SetFloat("far_plane", far_plane);
	}

	void Renderer3D::Flush()
	{
		if (s_Data.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

			// Bind textures
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);

			if (!s_Data.IsShadow)
			{
				s_Data.QuadShader->Bind();
				float far_plane = 25.0f;
				s_Data.QuadShader->SetFloat("far_plane", far_plane);
				s_Data.ShadowCubeTexture->Bind(31);
			}
			RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
			s_Data.Stats.DrawCalls++;
		}

	}

	void Renderer3D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer3D::DrawRotateQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& tintColor, float tilingFactor)
	{
		HZ_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= Renderer3DData::MaxIndices)
			NextBatch();

		glm::vec4 color = tintColor;
		float textureIndex = 0;

		if (texture != nullptr)
		{

			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i].get() == *texture.get())
				{
					textureIndex = (float)i;
					break;
				}
			}
			if (textureIndex == 0.0f)
			{
				textureIndex = s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
				s_Data.TextureSlotIndex++;
			}

		}

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[0];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f,0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[1];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f,0.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[2];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 1.0f,1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[3];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = { 0.0f,1.0f };
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		s_Data.QuadVertexBufferPtr++;

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer3D::EndScene()
	{
		Flush();
	}

	void Renderer3D::DrawModel(const glm::mat4& transform, Model& src, int entityID)
	{
		HZ_PROFILE_FUNCTION();
		s_Data.TriangleShader->SetMat4("model", transform);
		s_Data.ShadowDepthShader->SetMat4("model", transform);
		for (int i = 0; i < src.meshes.size(); i++)
		{
			DrawMesh(transform, src.meshes[i], entityID);
		}
	}

	void Renderer3D::DrawMesh(const glm::mat4& transform, Mesh& src, int entityID)
	{
		/*
		HZ_PROFILE_FUNCTION();
		s_Data.TriangleVertexArray = VertexArray::Create();
		s_Data.TriangleVertexBuffer = VertexBuffer::Create(src.vertices.size() * sizeof(Vertex));
		s_Data.TriangleVertexBuffer->SetLayout({
			{ShaderDataType::Float3,"a_Position"},
			{ShaderDataType::Float3,"a_Normal"},
			{ShaderDataType::Float2,"a_TexCoord"},
			});
		s_Data.TriangleVertexArray->AddVertexBuffer(s_Data.TriangleVertexBuffer);
		s_Data.TriangleIB = IndexBuffer::Create(src.indices.data(), src.indices.size());
		s_Data.TriangleVertexArray->SetIndexBuffer(s_Data.TriangleIB);


		for (int i = 0; i < src.textures.size(); i++)
		{
			src.textures[i]->Bind(i);
		}


		uint32_t dataSize = src.vertices.size() * sizeof(Vertex)/(sizeof(uint8_t));
		//uint32_t dataSize = (uint32_t)((uint8_t*)(src.vertices.data()+src.vertices.size())- (uint8_t*)src.vertices.data());
		s_Data.TriangleVertexBuffer->SetData(src.vertices.data(), dataSize);
		RenderCommand::DrawIndexed(s_Data.TriangleVertexArray, src.indices.size());

		s_Data.Stats.DrawCalls++;*/

		for (int i = 0; i < src.textures.size(); i++)
		{
			src.textures[i]->Bind(i);
		}

		RenderCommand::DrawIndexed(src.TriangleVertexArray, src.indices.size());

		s_Data.Stats.DrawCalls++;
	}

	void Renderer3D::DrawSkyBox(SkyBox& skyBox)
	{
		s_Data.SkyBoxShader->Bind();
		s_Data.SkyBoxVertexArray->Bind();
		s_Data.SkyBoxShader->SetMat4("u_ViewProjection", s_Data.SkyViewProjection);
		skyBox.skyTextureCube->Bind();
		
		//glBindTextureUnit(0, i);

		RenderCommand::SetDepth(DepthType::LEQUAL);
		RenderCommand::DrawArray(0, 36);
		RenderCommand::SetDepth(DepthType::LESS);
	}

	void Renderer3D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer3D::Statistics Renderer3D::GetStats()
	{
		return s_Data.Stats;
	}
}
