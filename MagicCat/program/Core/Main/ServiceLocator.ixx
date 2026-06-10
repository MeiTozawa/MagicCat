module;
#include <unordered_map>
#include <typeindex>
#include <memory>

export module ServiceLocator;

namespace mc {

export class ServiceLocator {
private:
    static std::unordered_map<std::type_index, std::shared_ptr<void>>& GetRegistry() {
        static std::unordered_map<std::type_index, std::shared_ptr<void>> registry;
        return registry;
    }

public:
    template<typename TInterface, typename TImpl>
    static void RegisterSingleton(std::shared_ptr<TImpl> instance) {
        static_assert(std::is_base_of<TInterface, TImpl>::value, "TImpl must derive from TInterface");
        std::shared_ptr<TInterface> interfacePtr = instance;
        GetRegistry()[std::type_index(typeid(TInterface))] = std::shared_ptr<void>(interfacePtr);
    }

    template<typename TInterface>
    static TInterface* Get() {
        auto& registry = GetRegistry();
        auto it = registry.find(std::type_index(typeid(TInterface)));
        if (it != registry.end()) {
            return static_cast<TInterface*>(it->second.get());
        }
        return nullptr;
    }
};
} // namespace mc

