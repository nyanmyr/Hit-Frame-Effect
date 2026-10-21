#ifndef COMPONENT_ARRAY_HPP
#define COMPONENT_ARRAY_HPP

#include <unordered_map>

#include "Core.hpp"

struct IComponentArray
{
	virtual ~IComponentArray() = default;
	virtual void destroyComponents(Entity entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray
{
private:
	std::unordered_map<Entity, T> components;

public:

	void insertData(Entity entity, T component)
	{
		components[entity] = component;
	}

	void removeData(Entity entity)
	{
		components.erase(entity);
	}

	T& getData(Entity entity)
	{
		return components.at(entity);
	}

	bool hasData(Entity entity) const
	{
		return components.find(entity) != components.end();
	}

	std::unordered_map<Entity, T>& getAll()
	{
		return components;
	}

	void destroyComponents(Entity entity) override
	{
		if (components.find(entity) != components.end())
		{
			components.erase(entity);
		}
	}
};

#endif