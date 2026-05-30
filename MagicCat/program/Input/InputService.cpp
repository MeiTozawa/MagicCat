module;
#include <dxe.h>
#include <memory>

module InputService;

class InputService : public IInputService
{
private:
    Shared<dxe::Input> input;
    std::vector<InputContext> contextStack;
    std::unordered_map<InputContext, std::unordered_map<InputAction, std::vector<dxe::Input::eButton>>> actionMappings;

    const std::vector<dxe::Input::eButton>* checkInput(InputAction action) const
    {
        if (contextStack.empty()) return nullptr;

        // 現在のコンテキストを表示する
        InputContext currentContext = contextStack.back();

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
        
        contextStack.push_back(InputContext::Gameplay);

        // Gameplay マッピングを設定する
        actionMappings[InputContext::Gameplay][InputAction::Interact] = {
            dxe::Input::eButton::KB_SPACE,
            dxe::Input::eButton::PAD_A
        };

        // Menu マッピングを設定する
        actionMappings[InputContext::Menu][InputAction::UI_Confirm] = {
            dxe::Input::eButton::KB_SPACE,
            dxe::Input::eButton::PAD_A
        };
        actionMappings[InputContext::Menu][InputAction::UI_Cancel] = {
            dxe::Input::eButton::KB_ESCAPE, 
            dxe::Input::eButton::PAD_B
        };
    }


    bool IsPressed(InputAction action) const override
    {
        auto keys = checkInput(action);
        if (keys != nullptr)
            return input->pressed(keys);
        return false;
    }

    bool IsHolding(InputAction action) const override
    {
        auto keys = checkInput(action);
        if (keys != nullptr)
            return input->keep(keys);
        return false;
    }

    bool IsReleased(InputAction action) const override
    {
        auto keys = checkInput(action);
        if (keys != nullptr)
            return input->released(keys);
        return false;
    }

    void PushContext(InputContext context) override
    {
        // 同じContextが連続してスタックにプッシュされるのを防ぐ
        if (contextStack.empty() || contextStack.back() != context)
        {
            contextStack.push_back(context);
        }
    }

    void PopContext() override
    {
        // スタックが空になるのを防ぐ
        if (contextStack.size() > 1)
        {
            contextStack.pop_back();
        }
    }

    void ClearAndSetContext(InputContext context) override
    {
        contextStack.clear();
        contextStack.push_back(context);
    }
};

Shared<IInputService> CreateInputService()
{
    return std::make_shared<InputService>();
}
