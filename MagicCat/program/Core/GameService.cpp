module;

#include <dxe.h>
#include <unordered_map>
#include <functional>
#include <memory>

module GameService;

import UiService;
import InputService;
import ServiceLocator;
import Player;
import HealthComponent;
import Enemy;
import CardService;
import AssetService;
import CharacterService;
import Character;

constexpr int CARD_MAX = 4;


class GameService : public IGameService
{
private:
    EGameState gameState = START;
    Shared<IUiService> uiService = nullptr;
    Shared<IInputService> inputService = nullptr;
    Shared<ICharacterService> characterService = nullptr;
    Shared<IAssetService> assetService;
    Shared<ICardService> cardService;

public:
    GameService()
    {
        // TODO
    }

    void Start() override
    {
        uiService = ServiceLocator::Get<IUiService>();
        inputService = ServiceLocator::Get<IInputService>();
        characterService = ServiceLocator::Get<ICharacterService>();
        cardService = ServiceLocator::Get<ICardService>();
        assetService = ServiceLocator::Get<IAssetService>();
        assetService->LoadAssets();
    }

    void End() override
    {
        // TODO
    }

    void Update(float deltaTime) override
    {
        switch (gameState)
        {
        case START:
            if (inputService->IsPressed(InputAction::IgConfirm))
                ChangeToCombat();
            break;
        case COMBAT:
            if (characterService->GetPlayer().IsDead())
                ChangeToStart();
            if (auto p = inputService->OnMouseClick(InputAction::IgMouseClick); p.x >= 0 && p.y >= 0)
            {
                for (const auto& rect : cardService->GetRectOfCards())
                {
                    // TODO: Handling mouse events
                }
            }
            if (inputService->IsPressed(InputAction::IgLeft))
            {
                cardService->MoveFocusToLeft();
            }
            else if (inputService->IsPressed(InputAction::IgRight))
            {
                cardService->MoveFocusToRight();
            }
            else if (inputService->IsPressed(InputAction::IgConfirm))
            {
                cardService->PlayCard(characterService->GetEnemy());
            }
            break;
        default: ;
        }

        uiService->Draw();
    }

private:
    void ChangeToStart()
    {
        gameState = START;
        uiService->ChangeSceneTo(START);
        characterService->Reset();
    }

    void ChangeToCombat()
    {
        gameState = COMBAT;
        characterService->Reset();
        cardService->DrawCards(CARD_MAX);
        uiService->ChangeSceneTo(COMBAT);
    }
};

static struct RegisterGameService
{
    RegisterGameService()
    {
        ServiceLocator::RegisterSingleton<IGameService, GameService>(std::make_shared<GameService>());
    }
} autoRegister_GameService;
