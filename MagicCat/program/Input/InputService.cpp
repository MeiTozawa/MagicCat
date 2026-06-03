module;

#include <dxe.h>
#include <memory>
#include <vector>
#include <unordered_map>

module InputService;

import ServiceLocator;

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
        
        _activeInputModes.push_back(InputContext::InGame);

        // Gameplay マッピングを設定する
        _actionMappings[InputContext::InGame][InputAction::IgInteract] = {
            dxe::Input::eButton::KB_E,
            dxe::Input::eButton::PAD_X
        };
        _actionMappings[InputContext::InGame][InputAction::IgConfirm] = {
            dxe::Input::eButton::KB_SPACE,
            dxe::Input::eButton::PAD_A
        };

        // Menu マッピングを設定する
        _actionMappings[InputContext::MENU][InputAction::MenuConfirm] = {
            dxe::Input::eButton::KB_SPACE,
            dxe::Input::eButton::PAD_A
        };
        _actionMappings[InputContext::MENU][InputAction::MenuCancel] = {
            dxe::Input::eButton::KB_ESCAPE, 
            dxe::Input::eButton::PAD_B
        };
    }


    bool IsPressed(InputAction action) const override
    {
        auto keys = CheckInput(action);
        if (keys != nullptr)
        {
            for (auto key : *keys)
            {
                if (_input->pressed(key))
                    return true;
            }
        }
        return false;
    }

    bool IsHolding(InputAction action) const override
    {
        auto keys = CheckInput(action);
        if (keys != nullptr)
        {
            for (auto key : *keys)
            {
                if (_input->keep(key))
                    return true;
            }
        }
        return false;
    }

    bool IsReleased(InputAction action) const override
    {
        auto keys = CheckInput(action);
        if (keys != nullptr)
        {
            for (auto key : *keys)
            {
                if (_input->released(key))
                    return true;
            }
        }
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

static struct RegisterInputService {
    RegisterInputService() {
        ServiceLocator::RegisterSingleton<IInputService, InputService>(std::make_shared<InputService>());
    }
} autoRegister_InputService;
