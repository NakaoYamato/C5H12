#pragma once

#include <unordered_map>
#include <vector>
#include <typeindex>
#include <functional>
#include <memory>

class EventBus
{
public:
    EventBus() = default;
    ~EventBus() = default;

    /// <summary>
    /// イベントの追加
    /// </summary>
    /// <typeparam name="EventT"></typeparam>
    /// <param name="handler"></param>
    template<typename EventT>
    void Subscribe(std::function<void(const EventT&)> handler)
    {
        auto wrapper = [handler](const void* e) 
            {
            handler(*static_cast<const EventT*>(e));
            };
        _listeners[typeid(EventT)].push_back(wrapper);
    }

    /// <summary>
    /// イベント実行
    /// </summary>
    /// <typeparam name="EventT"></typeparam>
    /// <param name="event"></param>
    template<typename EventT>
    void Publish(const EventT& event) const
    {
        auto it = _listeners.find(typeid(EventT));
        if (it != _listeners.end()) 
        {
            for (const auto& func : it->second)
            {
                func(&event);
            }
        }
    }
private:
    std::unordered_map<std::type_index,
        std::vector<std::function<void(const void*)>>> _listeners;
};