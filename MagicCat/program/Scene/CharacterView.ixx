module;

export module CharacterView;

import AssetService;
import ServiceLocator;
import EventBus;
import CharacterService;
import HealthComponent;

constexpr int PLAYER_START_X = 800;
constexpr int PLAYER_START_Y = 450;
constexpr int ENEMY_START_X = 1000;
constexpr int ENEMY_START_Y = 450;
constexpr int SPRITE_SIZE = 256;
constexpr float EXTRA_RATE = 8.f;

constexpr int ANIMATION_SPEED = 10;
constexpr int FRAME_COUNT = 4;
constexpr int FRAME_SIZE = 32;

export struct Animation
{
    int handle;
    int x;
    int y;
    float extraRate = 1.f;
    bool isFlip = false;
};

export class CharacterView
{
    Shared<IAssetService> assetService;
    std::vector<Animation> animations = {};
    
    EventHandle healthUpdateHandle;
    int playerHealth = 0;
    int playerMaxHealth = 0;
    int enemyHealth = 0;
    int enemyMaxHealth = 0;

    float timer = 0;
    int frame_index = 0;
    int sprite_size = 0;
public:
    CharacterView()
    {
        assetService = ServiceLocator::Get<IAssetService>();
        
        int handle = assetService->GetSpriteHandle(ESprite::Bunny);
        animations.push_back({handle, PLAYER_START_X, PLAYER_START_Y, EXTRA_RATE, false});
        handle = assetService->GetSpriteHandle(ESprite::Wolf);
        animations.push_back({handle, ENEMY_START_X, ENEMY_START_Y, EXTRA_RATE, true});

        healthUpdateHandle = EventBus::Subscribe<HealthChangedEvent>([this](const HealthChangedEvent& e) {
            if (auto characterService = ServiceLocator::Get<ICharacterService>()) {
                if (e.Victim == &characterService->GetPlayer()) {
                    playerHealth = e.CurrentHealth;
                } else {
                    enemyHealth = e.CurrentHealth;
                }
            }
        });
    }

    ~CharacterView()
    {
        EventBus::Unsubscribe(healthUpdateHandle);
    }


    void PrintSprites(float delta_time) 
    {
        timer += delta_time;
        if (timer >= 1.f / ANIMATION_SPEED)
        {
            frame_index += 1;
            if (frame_index >= FRAME_COUNT)
                frame_index = 0;
            timer = 0;
        }
        for (auto animation : animations)
        {
            int frame = DerivationGraph(frame_index * FRAME_SIZE, 0, FRAME_SIZE, FRAME_SIZE, animation.handle);
            DrawRotaGraph(animation.x + FRAME_SIZE / 2, animation.y + FRAME_SIZE / 2, 
                animation.extraRate, 0., frame, TRUE, animation.isFlip);
        }
    }
};
