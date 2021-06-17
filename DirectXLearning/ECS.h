#pragma once

#include <cstdint>
#include <bitset>
#include <queue>
#include <cassert>
#include <array>
#include <unordered_map>
#include <memory>
#include <set>


namespace Ecs {
	// Entity
	using Entity = std::uint32_t;
	const Entity MAX_ENTITIES = 5000;


	// Component
	using ComponentType = std::uint8_t;
	const ComponentType MAX_COMPONENTS = 32;

	// Signature
	using Signature = std::bitset<MAX_COMPONENTS>;


	//Entity Manager
	class EntityManager {
	public:
		EntityManager() {
			for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
				availableEntities.push(entity);
			}
		}

		Entity CreateEntity() {
			assert(livingEntityCount < MAX_ENTITIES && "Too many entities already");

			Entity id = availableEntities.front();
			availableEntities.pop();
			++livingEntityCount;

			return id;
		}

		void DestroyEntity(Entity entity) {
			assert(entity < MAX_ENTITIES && "Entity out of range");

			signatures[entity].reset();

			availableEntities.push(entity);
			--livingEntityCount;
		}

		void SetSignature(Entity entity, Signature signature) {
			assert(entity < MAX_ENTITIES && "Entity out of range");

			signatures[entity] = signature;
		}

		Signature GetSignature(Entity entity) {
			assert(entity < MAX_ENTITIES && "Entity out of range");

			return signatures[entity];
		}

	private:
		std::queue<Entity> availableEntities{};
		std::array<Signature, MAX_ENTITIES> signatures{};
		uint32_t livingEntityCount{};
	};


	// Component
	class IComponentArray {
	public:
		virtual ~IComponentArray() = default;
		virtual void EntityDestroyed(Entity entity) = 0;
	};

	template<typename T>
	class ComponentArray : public IComponentArray {
	public:
		void InsertData(Entity entity, T component) {
			assert(entityToIndexMap.find(entity) == entityToIndexMap.end() && "Component already added!");

			size_t newIndex = size;
			entityToIndexMap[entity] = newIndex;
			indexToEntityMap[newIndex] = entity;
			componentArray[newIndex] = component;
			++size;
		}

		void RemoveData(Entity entity) {
			assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Removing non-existent component!");

			size_t indexOfRemovedEntity = entityToIndexMap[entity];
			size_t indexOfLastElement = size - 1;
			componentArray[indexOfRemovedEntity] = componentArray[indexOfLastElement];

			Entity entityOfLastElement = indexToEntityMap[indexOfLastElement];
			entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
			indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

			entityToIndexMap.erase(entity);
			indexToEntityMap.erase(indexOfLastElement);
			--size;
		}

		T& GetData(Entity entity) {
			assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Retrieving non-existent component!");

			return componentArray[entityToIndexMap[entity]];
		}

		void EntityDestroyed(Entity entity) override {
			if (entityToIndexMap.find(entity) != entityToIndexMap.end()) {
				RemoveData(entity);
			}
		}

	private:
		std::array<T, MAX_ENTITIES> componentArray;
		std::unordered_map<Entity, size_t> entityToIndexMap;
		std::unordered_map<size_t, Entity> indexToEntityMap;
		size_t size;
	};

	// ComponentManager
	class ComponentManager {
	public:
		template<typename T>
		void RegisterComponent() {
			const char* typeName = typeid(T).name();
			assert(componentTypes.find(typeName) == componentTypes.end() && "Component type already registered!");

			componentTypes.insert({ typeName, nextComponentType });
			componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

			++nextComponentType;
		}

		template<typename T>
		ComponentType GetComponentType() {
			const char* typeName = typeid(T).name();
			assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered before use!");
			return componentTypes[typeName];
		}

		template<typename T>
		void AddComponent(Entity entity, T component) {
			GetComponentArray<T>()->InsertData(entity, component);
		}

		template<typename T>
		ComponentType RemoveComponent(Entity entity, T component) {
			GetComponentArray<T>()->RemoveData(entity);
		}

		template<typename T>
		T& GetComponent(Entity entity) {
			return GetComponentArray<T>()->GetData(entity);
		}

		void EntityDestroyed(Entity entity) {
			for (auto const& pair : componentArrays) {
				auto const& component = pair.second;
				component->EntityDestroyed(entity);
			}
		}

	private:
		std::unordered_map<const char*, ComponentType> componentTypes{};
		std::unordered_map<const char*, std::shared_ptr<IComponentArray>> componentArrays{};

		ComponentType nextComponentType{};

		template<typename T>
		std::shared_ptr<ComponentArray<T>> GetComponentArray() {
			const char* typeName = typeid(T).name();
			assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered before use!");
			return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
		}
	};

	class System {
	public:
		std::set<Entity> entities;
	};

	class SystemManager {
	public:
		template<typename T>
		std::shared_ptr<T> RegisterSystem() {
			const char* typeName = typeid(T).name();

			assert(systems.find(typeName) == systems.end() && "Registering system more than once.");

			auto system = std::make_shared<T>();
			systems.insert({ typeName, system });
			return system;
		}

		template<typename T>
		void SetSignature(Signature signature) {
			const char* typeName = typeid(T).name();
			assert(systems.find(typeName) != systems.end() && "System used before registered.");

			signatures.insert({ typeName, signature });
		}

		void EntityDestroyed(Entity entity) {
			for (auto const& pair : systems) {
				auto const& system = pair.second;
				system->entities.erase(entity);
			}
		}

		void EntitySignatureChanged(Entity entity, Signature entitySignature) {
			for (auto const& pair : systems) {
				auto const& type = pair.first;
				auto const& system = pair.second;
				auto const& systemSignature = signatures[type];

				if ((entitySignature & systemSignature) == systemSignature) {
					system->entities.insert(entity);
				}
				else {
					system->entities.erase(entity);
				}
			}
		}

	private:
		std::unordered_map<const char*, Signature> signatures;
		std::unordered_map<const char*, std::shared_ptr<System>> systems{};
	};

	class Coordinator {
	public:
		Coordinator() {
			Init();
		}

		void Init() 	{
			componentManager = std::make_unique<ComponentManager>();
			entityManager = std::make_unique<EntityManager>();
			systemManager = std::make_unique<SystemManager>();
		}

		Entity CreateEntity() 	{
			return entityManager->CreateEntity();
		}

		void DestroyEntity(Entity entity) 	{
			entityManager->DestroyEntity(entity);
			componentManager->EntityDestroyed(entity);
			systemManager->EntityDestroyed(entity);
		}

		template<typename T>
		void RegisterComponent() 	{
			componentManager->RegisterComponent<T>();
		}

		template<typename T>
		void AddComponent(Entity entity, T component) 	{
			componentManager->AddComponent<T>(entity, component);

			auto signature = entityManager->GetSignature(entity);
			signature.set(componentManager->GetComponentType<T>(), true);
			entityManager->SetSignature(entity, signature);

			systemManager->EntitySignatureChanged(entity, signature);
		}

		template<typename T>
		void RemoveComponent(Entity entity) 	{
			componentManager->RemoveComponent<T>(entity);

			auto signature = entityManager->GetSignature(entity);
			signature.set(componentManager->GetComponentType<T>(), false);
			entityManager->SetSignature(entity, signature);

			systemManager->EntitySignatureChanged(entity, signature);
		}

		template<typename T>
		T& GetComponent(Entity entity) 	{
			return componentManager->GetComponent<T>(entity);
		}

		template<typename T>
		ComponentType GetComponentType() 	{
			return componentManager->GetComponentType<T>();
		}

		template<typename T>
		std::shared_ptr<T> RegisterSystem() 	{
			return systemManager->RegisterSystem<T>();
		}

		template<typename T>
		void SetSystemSignature(Signature signature) 	{
			systemManager->SetSignature<T>(signature);
		}

	private:
		std::unique_ptr<ComponentManager> componentManager;
		std::unique_ptr<EntityManager> entityManager;
		std::unique_ptr<SystemManager> systemManager;
	};
}
