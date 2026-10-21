#ifndef EVENT_DISPATCHER_HPP
#define EVENT_DISPATCHER_HPP

#include <iostream>
#include <functional>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <any>
#include <algorithm>

// stores events and listensers (functions)
class Dispatcher
{
private:
    std::unordered_map<std::type_index, std::any> events;

    template<typename T>
    auto findEvent() const
    {
        return std::type_index(typeid(T));
    }

    template<typename T>
    auto getEventIterator()
    {
        return events.find(findEvent<T>());
    }

    template<typename T>
    auto* getEventArray()
    {
        if (getEventIterator<T>() == events.end())
        {
            throw std::runtime_error("Event not registered.");
        }

        return std::any_cast<std::vector<std::function<void(T)>>>(&(getEventIterator<T>()->second));
    }

    Dispatcher() {}

    Dispatcher(const Dispatcher&) = delete;
    Dispatcher& operator=(const Dispatcher&) = delete;

public:
    static auto& getInstance()
    {
        static Dispatcher instance;
        return instance;
    }

    template<typename T>
    void registerEvent()
    {
        if (getEventIterator<T>() != events.end())
        {
            throw std::runtime_error("Event already registered.");
        }

        events.insert({ findEvent<T>(), std::vector<std::function<void(T)>>{} });
    }

    template<typename T>
    void listen(std::function<void(T)> event)
    {
        if (getEventIterator<T>() == events.end())
        {
            throw std::runtime_error("Event not registered.");
        }

        getEventArray<T>()->push_back(event);
    }

    template<typename T>
    void call(T event)
    {
        if (getEventIterator<T>() == events.end())
        {
            throw std::runtime_error("Event not registered.");
        }

        for (std::function<void(T)> func : *getEventArray<T>())
        {
            func(event);
        }
    }
};

#endif