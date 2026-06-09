module;

#include <dxe.h>
#include <DxLib.h>
#include <memory>
#include <vector>
#include <unordered_map>

module InputService;

import ServiceLocator;

class InputService : public IInputService
{
    Shared<dxe::Input> input;
    std::vector<InputContext> activeInputModes;
    std::unordered_map<InputContext, std::unordered_map<InputAction, std::vector<dxe::Input::eButton>>> actionMappings;

    const std::vector<dxe::Input::eButton>* CheckInput(InputAction action) const
    {
        if (activeInputModes.empty()) return nullptr;

        // 現在のコンテキストを表示する
        InputContext currentContext = activeInputModes.back();

        // 現在のコンテキストのマッピングテーブル内で、このアクションが設定されているか確認する
        auto contextIt = actionMappings.find(currentContext);
        if (contextIt == actionMappings.end()) return nullptr;

        // 現在のコンテキストがこのアクションを認識しているかを確認する
        auto actionIt = contextIt->second.find(action);
        if (actionIt == contextIt->second.end()) return nullptr;

        return &actionIt->second;
    }

public:
    InputService()
    {
        input = dxe::Input::Create(0, dxe::Input::eJoypad::PAD1);

        activeInputModes.push_back(InputContext::InGame);

        // Gameplay マッピングを設定する
        actionMappings[InputContext::InGame][InputAction::IgInteract] = {
            dxe::Input::eButton::KB_E,
            dxe::Input::eButton::PAD_X
        };
        actionMappings[InputContext::InGame][InputAction::IgPlatCard] = {
            dxe::Input::eButton::KB_SPACE,
            dxe::Input::eButton::PAD_A
        };
        // _actionMappings[InputContext::InGame][InputAction::IgMouseClick] = {
        //     dxe::Input::eButton::MOUSE_LEFT
        // };
        actionMappings[InputContext::InGame][InputAction::IgUp] = {
            dxe::Input::eButton::KB_UP,
            dxe::Input::eButton::PAD_UP
        };
        actionMappings[InputContext::InGame][InputAction::IgDown] = {
            dxe::Input::eButton::KB_DOWN,
            dxe::Input::eButton::PAD_DOWN
        };
        actionMappings[InputContext::InGame][InputAction::IgDrawCard] = {
            dxe::Input::eButton::KB_Q,
            dxe::Input::eButton::PAD_X
        };
        actionMappings[InputContext::InGame][InputAction::IgCombat] = {
            dxe::Input::eButton::KB_W,
            dxe::Input::eButton::PAD_Y
        };
        actionMappings[InputContext::InGame][InputAction::IgCancel] = {
            dxe::Input::eButton::KB_R,
            dxe::Input::eButton::PAD_B
        };

        // Menu マッピングを設定する
        actionMappings[InputContext::Menu][InputAction::MenuConfirm] = {
            dxe::Input::eButton::KB_SPACE,
            dxe::Input::eButton::PAD_A
        };
        actionMappings[InputContext::Menu][InputAction::MenuCancel] = {
            dxe::Input::eButton::KB_ESCAPE,
            dxe::Input::eButton::PAD_B
        };
        actionMappings[InputContext::Menu][InputAction::MenuMouseClick] = {
            dxe::Input::eButton::MOUSE_LEFT
        };
    }


    bool IsPressed(InputAction action) const override
    {
        auto keys = CheckInput(action);
        if (keys != nullptr)
        {
            for (auto key : *keys)
            {
                if (input->pressed(key))
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
                if (input->keep(key))
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
                if (input->released(key))
                    return true;
            }
        }
        return false;
    }

    void PushContext(InputContext context) override
    {
        // 同じContextが連続してスタックにプッシュされるのを防ぐ
        if (activeInputModes.empty() || activeInputModes.back() != context)
        {
            activeInputModes.push_back(context);
        }
    }

    void PopContext() override
    {
        // スタックが空になるのを防ぐ
        if (activeInputModes.size() > 1)
        {
            activeInputModes.pop_back();
        }
    }

    void ClearAndSetContext(InputContext context) override
    {
        activeInputModes.clear();
        activeInputModes.push_back(context);
    }

    tnl::Vector2i OnMouseClick(InputAction action) const override
    {
        auto key = CheckInput(action)->at(0);
        if (input->pressed(key))
        {
            int x = 0, y = 0;
            GetMousePoint(&x, &y);
            return tnl::Vector2i(x, y);
        }
        return tnl::Vector2i(-1, -1);
    }
};

static struct RegisterInputService
{
    RegisterInputService()
    {
        ServiceLocator::RegisterSingleton<IInputService, InputService>(std::make_shared<InputService>());
    }
} autoRegister_InputService;
