module;

#include <unordered_map>
#include <typeindex>
#include <functional>
#include <vector>
#include <algorithm>
#include <concepts>
#include <ranges>
#include <shared_mutex>

export module EventBus;

namespace mc {

export struct IEvent
{
    virtual ~IEvent() = default;
};

export using EventHandle = uint64_t;

export class EventBus
{
private:
    struct CallbackWrapper
    {
        EventHandle Handle;
        std::function<void(const IEvent&)> Callback;
    };

    static std::unordered_map<std::type_index, std::vector<CallbackWrapper>>& GetListeners()
    {
        static std::unordered_map<std::type_index, std::vector<CallbackWrapper>> listeners;
        return listeners;
    }

    static std::unordered_map<EventHandle, std::type_index>& GetHandleToTypeMap()
    {
        static std::unordered_map<EventHandle, std::type_index> map;
        return map;
    }

    static std::shared_mutex& GetMutex()
    {
        static std::shared_mutex mtx;
        return mtx;
    }

    static EventHandle& GetNextHandle()
    {
        static EventHandle nextHandle = 0;
        return nextHandle;
    }

public:
    template <std::derived_from<IEvent> TEvent>
    static EventHandle Subscribe(std::function<void(const TEvent&)> callback)
    {
        std::unique_lock lock(GetMutex());
        EventHandle handle = GetNextHandle()++;
        auto& listeners = GetListeners()[std::type_index(typeid(TEvent))];

        listeners.push_back({
            handle,
            [callback](const IEvent& e)
            {
                callback(static_cast<const TEvent&>(e));
            }
        });

        std::type_index typeId = std::type_index(typeid(TEvent));
        GetHandleToTypeMap().insert_or_assign(handle, typeId);

        return handle;
    }

    static void Unsubscribe(EventHandle handle)
    {
        auto& map = GetHandleToTypeMap();
        auto itMap = map.find(handle);
        if (itMap == map.end()) return;

        std::type_index typeId = itMap->second;
        auto& listeners = GetListeners()[typeId];

        auto it = std::ranges::remove_if(listeners,
                                         [handle](const CallbackWrapper& w) { return w.Handle == handle; }).begin();

        if (it != listeners.end())
        {
            listeners.erase(it, listeners.end());
        }

        map.erase(itMap);
    }


    template <std::derived_from<IEvent> TEvent>
    static void Publish(const TEvent& event)
    {
        std::vector<CallbackWrapper> callbacks;
        {
            std::shared_lock lock(GetMutex());
            auto& listeners = GetListeners();
            auto it = listeners.find(std::type_index(typeid(TEvent)));
            if (it != listeners.end())
            {
                callbacks = it->second;
            }
        }
        for (const auto& wrapper : callbacks)
        {
            wrapper.Callback(event);
        }
    }
};

} // namespace mc

