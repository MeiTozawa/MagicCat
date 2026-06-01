module;
#include <dxe.h>
#include <memory>

module InputService;

class InputService : public IInputService
{
private:
    Shared<dxe::Input> _input;
    std::vector<InputContext> _activeInputModes;
    std::unordered_map<InputContext, std::unordered_map<InputAction, std::vector<dxe::Input::eButton>>> _actionMappings;

    const std::vector<dxe::Input::eButton>* CheckInput(InputAction action) const
    {
        if (_activeInputModes.empty()) return nullptr;

        // 現在のコンテキストを表示する
        InputContext currentContext = _activeInputModes.back();

        // 現在のコンテキストのマッピングテーブル内で、このアクションが設定されているか確認する
        auto contextIt = _actionMappings.find(currentContext);
        if (contextIt == _actionMappings.end()) return nullptr;

        // 現在のコンテキストがこのアクションを認識しているかを確認する
        auto actionIt = contextIt->second.find(action);
        if (actionIt == contextIt->second.end()) return nullptr;
        
        return &actionIt->second;
    }

public:
    InputService()
    {
        _input = dxe::Input::Create(0, dxe::Input::eJoypad::PAD1);
        
        _activeInputModes.push_back(InputContext::Gameplay);

        // Gameplay マッピングを設定する
        _actionMappings[InputContext::Gameplay][InputAction::GpInteract] = {
            dxe::Input::eButton::KB_SPACE,
            dxe::Input::eButton::PAD_A
        };

        // Menu マッピングを設定する
        _actionMappings[InputContext::Menu][InputAction::UiConfirm] = {
            dxe::Input::eButton::KB_SPACE,
            dxe::Input::eButton::PAD_A
        };
        _actionMappings[InputContext::Menu][InputAction::UiCancel] = {
            dxe::Input::eButton::KB_ESCAPE, 
            dxe::Input::eButton::PAD_B
        };
    }


    bool IsPressed(InputAction action) const override
    {
        auto keys = CheckInput(action);
        if (keys != nullptr)
            return _input->pressed(keys);
        return false;
    }

    bool IsHolding(InputAction action) const override
    {
        auto keys = CheckInput(action);
        if (keys != nullptr)
            return _input->keep(keys);
        return false;
    }

    bool IsReleased(InputAction action) const override
    {
        auto keys = CheckInput(action);
        if (keys != nullptr)
            return _input->released(keys);
        return false;
    }

    void PushContext(InputContext context) override
    {
        // 同じContextが連続してスタックにプッシュされるのを防ぐ
        if (_activeInputModes.empty() || _activeInputModes.back() != context)
        {
            _activeInputModes.push_back(context);
        }
    }

    void PopContext() override
    {
        // スタックが空になるのを防ぐ
        if (_activeInputModes.size() > 1)
        {
            _activeInputModes.pop_back();
        }
    }

    void ClearAndSetContext(InputContext context) override
    {
        _activeInputModes.clear();
        _activeInputModes.push_back(context);
    }
};

Shared<IInputService> CreateInputService()
{
    return std::make_shared<InputService>();
}
