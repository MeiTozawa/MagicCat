module;

#include <dxe.h>
#include <memory>

module UiService;

import GameService;
import ServiceLocator;
class StartUi : public IUi {
public:
    void Draw() override {
        DrawString(300, 300, L"Press SPACE to Start", 0xFFFFFF, TRUE);
    }
};

static struct RegisterStartUi {
    RegisterStartUi() {
        UiRegistry::GetRegistrations().push_back([]() {
            if(auto uiService = ServiceLocator::Get<IUiService>()) {
                uiService->RegisterScene(START, std::make_shared<StartUi>());
            }
        });
    }
} autoRegister_StartUi;