#pragma once


#include "entt.hpp"

#include "Hazel/Core/Timestep.h"
#include "Hazel/Core/UUID.h"
#include "Hazel/Renderer/EditorCamera.h"
#include "Hazel/Renderer/Framebuffer.h"
#include "SkyBox.h"

class b2World;

namespace Hazel {
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		void DuplicateEntity(Entity entity);

		Entity GetPrimaryCameraEntity();

		void SetSkyBox(Ref<SkyBox> sky);
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;

		Ref<SkyBox> skyBox;

		friend class SceneSerializer;
		friend class Entity;
		friend class SceneHierarchyPanel;

//TODO::以后优化改进场景的渲染
	public:
		Ref<Framebuffer> m_Framebuffer;
		Ref<Framebuffer> m_ShadowFramebuffer;
	};
}
