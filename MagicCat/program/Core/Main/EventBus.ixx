module;

#include <unordered_map>
#include <typeindex>
#include <functional>
#include <vector>
#include <algorithm>
#include <concepts>
#include <ranges>

export module EventBus;

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

    static EventHandle& GetNextHandle()
    {
        static EventHandle nextHandle = 0;
        return nextHandle;
    }

public:
    // ----------------------------------------------------------
    // Subscribe: 使用 concept 直接在模板声明处约束 TEvent 必须派生自 IEvent
    // ----------------------------------------------------------
    template <std::derived_from<IEvent> TEvent>
    static EventHandle Subscribe(std::function<void(const TEvent&)> callback)
    {
        EventHandle handle = GetNextHandle()++;
        auto& listeners = GetListeners()[std::type_index(typeid(TEvent))];

        listeners.push_back({
            handle,
            [callback](const IEvent& e)
            {
                callback(static_cast<const TEvent&>(e));
            }
        });

        return handle;
    }

    static void Unsubscribe(EventHandle handle)
    {
        for (auto& listeners : GetListeners() | std::views::values)
        {
            auto it = std::ranges::remove_if(
                listeners,
                [handle](const CallbackWrapper& w)
                {
                    return w.Handle == handle;
                }).begin();
            listeners.erase(it, listeners.end());
        }
    }


    template <std::derived_from<IEvent> TEvent>
    static void Publish(const TEvent& event)
    {
        auto& listeners = GetListeners();
        auto it = listeners.find(std::type_index(typeid(TEvent)));
        if (it != listeners.end())
        {
            auto callbacks = it->second;
            for (const auto& wrapper : callbacks)
            {
                wrapper.Callback(event);
            }
        }
    }
};
