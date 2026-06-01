module;


export module InputService; 

export enum class InputAction {
    GpMove, GpInteract, GpConfirm, GpToggleMenu,
    
    UiConfirm = 100,
    UiCancel,
};

export enum class InputContext {
    Gameplay, Menu, Cutscene
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
