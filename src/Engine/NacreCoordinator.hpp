#ifndef NACRE_MANAGER_HPP
#define NACRE_MANAGER_HPP

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "EventDispatcher.hpp"

class NacreCoordinator
{
private:
	ComponentManager& cm = ComponentManager::getInstance();
	EntityManager& em = EntityManager::getInstance();
	Dispatcher& ds = Dispatcher::getInstance();

	NacreCoordinator() {}

	NacreCoordinator(const NacreCoordinator&) = delete;
	NacreCoordinator& operator=(const NacreCoordinator&) = delete;
public:
	static auto& getInstance()
	{
		static NacreCoordinator instance;
		return instance;
	}

	template<typename T>
	void registerComponent()
	{
		cm.registerComponent<T>();
	}

	template<typename T>
	void addComponent(Entity entity, T component)
	{
		cm.addComponent(entity, component);
	}

	template<typename T>
	void removeComponent(Entity entity)
	{
		cm.removeComponent<T>(entity);
	}

	template<typename T>
	T& getComponent(Entity entity) const
	{
		T& component = cm.getComponent<T>(entity);
		return component;
	}

	template<typename T>
	std::shared_ptr<ComponentArray<T>> getComponentArray()
	{
		std::shared_ptr<ComponentArray<T>> componentArray = cm.getComponentArray<T>();
		return componentArray;
	}

	Entity createEntity()
	{
		Entity entity = em.createEntity();
		return entity;
	}

	bool isAlive(Entity entity)
	{
		bool isAlive = em.isAlive(entity);
		return isAlive;
	}

	void deleteEntity(Entity entity)
	{
		cm.entityDestroyed(entity);
		em.destroyEntity(entity);
	}

	void destroyAll()
	{
		cm.allEntitiesDestroyed();
		em.destroyAllEntities();
	}

	template<typename T>
	void registerEvent()
	{
		ds.registerEvent<T>();
	}

	template<typename T>
	void listen(std::function<void(T)> event)
	{
		ds.listen<T>(event);
	}

	template<typename T>
	void call(T event)
	{
		ds.call<T>(event);
	}
};

inline auto& nc()
{
	return NacreCoordinator::getInstance();
}

#endif