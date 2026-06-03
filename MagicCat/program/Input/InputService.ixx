module;


export module InputService; 

export enum class InputAction {
    IgMove, IgInteract, IgConfirm, IgToggleMenu,
    
    MenuConfirm = 100,
    MenuCancel,
};

export enum class InputContext {
    InGame, MENU
};

export class IInputService {
public:
    virtual ~IInputService() = default;
    
    virtual bool IsPressed(InputAction action) const = 0;
    virtual bool IsHolding(InputAction action) const = 0;
    virtual bool IsReleased(InputAction action) const = 0;

    virtual void PushContext(InputContext context) = 0;
    virtual void PopContext() = 0;
    virtual void ClearAndSetContext(InputContext context) = 0;
};
