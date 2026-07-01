module;

#include <dxe.h>
#include <memory>
#include <vector>
#include <unordered_map>

module InputService;

namespace mc {
    class InputService : public IInputService
    {
        Shared<dxe::Input> input;
        std::vector<InputContext> activeInputModes;
        std::unordered_map<InputContext, std::unordered_map<InputAction, std::vector<dxe::Input::eButton>>>
        actionMappings;

        // 最後に入力を検出したデバイス種別
        mutable InputDevice activeDevice = InputDevice::Keyboard;

        const std::vector<dxe::Input::eButton>* CheckInput(InputAction action) const
        {
            if (activeInputModes.empty()) return nullptr;

            InputContext currentContext = activeInputModes.back();

            auto contextIt = actionMappings.find(currentContext);
            if (contextIt == actionMappings.end()) return nullptr;

            auto actionIt = contextIt->second.find(action);
            if (actionIt == contextIt->second.end()) return nullptr;

            return &actionIt->second;
        }

    public:
        InputService()
        {
            input = dxe::Input::Create(0, dxe::Input::eJoypad::PAD1);

            activeInputModes.push_back(InputContext::InGame);

            actionMappings[InputContext::InGame][InputAction::IgConfirm] = {
                dxe::Input::eButton::KB_SPACE,
                dxe::Input::eButton::KB_RETURN,
                dxe::Input::eButton::PAD_A
            };
            actionMappings[InputContext::InGame][InputAction::IgUp] = {
                dxe::Input::eButton::KB_UP,
                dxe::Input::eButton::PAD_UP
            };
            actionMappings[InputContext::InGame][InputAction::IgDown] = {
                dxe::Input::eButton::KB_DOWN,
                dxe::Input::eButton::PAD_DOWN
            };
            actionMappings[InputContext::InGame][InputAction::IgLeft] = {
                dxe::Input::eButton::KB_LEFT,
                dxe::Input::eButton::PAD_LEFT
            };
            actionMappings[InputContext::InGame][InputAction::IgRight] = {
                dxe::Input::eButton::KB_RIGHT,
                dxe::Input::eButton::PAD_RIGHT
            };
            actionMappings[InputContext::InGame][InputAction::IgDrawCard] = {
                dxe::Input::eButton::KB_Q,
                dxe::Input::eButton::PAD_X
            };
            actionMappings[InputContext::InGame][InputAction::IgShowRules] = {
                dxe::Input::eButton::KB_ESCAPE,
                dxe::Input::eButton::PAD_START
            };
            actionMappings[InputContext::InGame][InputAction::IgMouseClick] = { dxe::Input::eButton::MOUSE_LEFT };

            actionMappings[InputContext::Menu][InputAction::MenuConfirm] = {
                dxe::Input::eButton::KB_SPACE,
                dxe::Input::eButton::KB_RETURN,
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
                    {
                        if (static_cast<int>(key) >= static_cast<int>(dxe::Input::eButton::PAD_MIN))
                            activeDevice = InputDevice::Gamepad;
                        else
                            activeDevice = InputDevice::Keyboard;
                        return true;
                    }
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
            if (activeInputModes.empty() || activeInputModes.back() != context)
            {
                activeInputModes.push_back(context);
            }
        }

        void PopContext() override
        {
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

        InputDevice GetActiveDevice() const override
        {
            return activeDevice;
        }

        Point<int> OnMouseClick(InputAction action) const override
        {
            auto keys = CheckInput(action);
            if (!keys || keys->empty()) return Point<int>{-1, -1};
            auto key = keys->at(0);
            if (input->pressed(key))
            {
                activeDevice = InputDevice::Keyboard;
                int x = 0, y = 0;
                GetMousePoint(&x, &y);
                return Point<int>{x, y};
            }
            return Point<int>{-1, -1};
        }
    };

    std::unique_ptr<IInputService> CreateInputService()
    {
        return std::make_unique<InputService>();
    }
} // namespace mc
