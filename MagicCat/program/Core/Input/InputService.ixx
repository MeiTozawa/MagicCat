module;

#include <tnl_vector2i.h>

export module InputService; 

namespace mc {

export enum class InputAction {
    IgUp, IgDown, IgInteract, IgPlatCard, IgToggleMenu, // IgMouseClick,
    IgDrawCard, IgCombat, IgCancel,
    
    MenuConfirm = 100,
    MenuCancel,
    MenuMouseClick,
};

export enum class InputContext {
    InGame, Menu
};

export class IInputService {
public:
    virtual ~IInputService() = default;
    
    virtual bool IsPressed(InputAction action) const = 0;
    virtual bool IsHolding(InputAction action) const = 0;
    virtual bool IsReleased(InputAction action) const = 0;
    virtual tnl::Vector2i OnMouseClick(InputAction action) const = 0;

    virtual void PushContext(InputContext context) = 0;
    virtual void PopContext() = 0;
    virtual void ClearAndSetContext(InputContext context) = 0;
};

export Shared<IInputService> CreateInputService();

} // namespace mc

