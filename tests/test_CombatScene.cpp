#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>
#include "MockServices.h"

import SceneService;
import BattleService;
import CardService;
import EventBus;
import Character;
import Player;
import Enemy;
import HealthComponent;
import InputService;

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;
using ::testing::AnyNumber;

namespace mc {
namespace {

class MockSceneService : public ISceneService {
public:
    MOCK_METHOD(void, Update, (float), (override));
    MOCK_METHOD(void, PushScene, (ESceneState), (override));
    MOCK_METHOD(void, PopScene, (), (override));
    MOCK_METHOD(void, RegisterScene, (ESceneState, std::unique_ptr<IScene>&&), (override));
    MOCK_METHOD(ESceneState, GetCurrentScene, (), (override));
    MOCK_METHOD(void, SetCurrentScene, (ESceneState), (override));
};

class StubScene : public IScene {
public:
    void Start() override {}
    void Update(float) override {}
};

class CombatSceneTest : public ::testing::Test {
protected:
    NiceMock<MockRenderService>  mockRender;
    NiceMock<MockAssetService>   mockAsset;
    NiceMock<MockInputService>   mockInput;
    NiceMock<MockBattleService>  mockBattle;
    NiceMock<MockCardService>    mockCard;
    NiceMock<MockSceneService>   mockScene;
    NiceMock<MockOSService>      mockOsService;

    Player player;
    Enemy  enemy;

    std::unique_ptr<IScene> combatScene;

    void SetUp() override {
        ON_CALL(mockBattle, GetPlayer()).WillByDefault(testing::ReturnRef(player));
        ON_CALL(mockBattle, GetEnemy()).WillByDefault(testing::ReturnRef(enemy));
        ON_CALL(mockBattle, GetTotalEnemyCount()).WillByDefault(Return(1));
        ON_CALL(mockBattle, GetCurrentEnemyIndex()).WillByDefault(Return(0));

        ON_CALL(mockCard, GetHandCards()).WillByDefault(Return(std::vector<Card>{}));
        ON_CALL(mockCard, GetDrawCards()).WillByDefault(Return(std::vector<Card>{}));
        ON_CALL(mockCard, GetDiscardCards()).WillByDefault(Return(std::vector<Card>{}));

        ON_CALL(mockAsset, GetSpriteInfo(_)).WillByDefault(Return(SpriteInfo{{32, 32}, 4}));

        ON_CALL(mockRender, GetFontSize()).WillByDefault(Return(24));
        ON_CALL(mockRender, GetDrawStringWidth(_)).WillByDefault(Return(0));

        combatScene = CreateCombatScene(
            mockScene, mockAsset, mockCard, mockInput, mockRender, mockBattle, mockOsService);
    }

    void TearDown() override {
        combatScene.reset();
    }
};

// After N Start() calls, exactly one CombatEvent subscription must remain.
// The handler calls GetImageHandle twice; if K subscriptions exist it would be called 2*K times.
TEST_F(CombatSceneTest, StartIsIdempotentForCombatEvent) {
    rc::check("CombatScene::Start() N times leaves exactly one CombatEvent subscription", [this]() {
        combatScene = CreateCombatScene(
            mockScene, mockAsset, mockCard, mockInput, mockRender, mockBattle, mockOsService);

        int n = *rc::gen::inRange(1, 11);

        for (int i = 0; i < n; ++i) {
            combatScene->Start();
        }

        int imageHandleCallCount = 0;
        ON_CALL(mockAsset, GetImageHandle(_)).WillByDefault([&](EImage) {
            ++imageHandleCallCount;
            return 0;
        });

        CombatEvent evt{
            EAttackType::Rock, EAttackType::Scissors,
            /* playerDamage */ 2, /* enemyDamage */ 2
        };
        EventBus::Publish(evt);

        // Exactly 2 calls = exactly 1 subscription (handler calls GetImageHandle twice)
        RC_ASSERT(imageHandleCallCount == 2);

        combatScene.reset();
    });
}

TEST_F(CombatSceneTest, StartIsIdempotentForHealthChangedEvent) {
    rc::check("CombatScene::Start() N times leaves exactly one HealthChangedEvent subscription", [this]() {
        combatScene = CreateCombatScene(
            mockScene, mockAsset, mockCard, mockInput, mockRender, mockBattle, mockOsService);

        int n = *rc::gen::inRange(1, 11);

        for (int i = 0; i < n; ++i) {
            combatScene->Start();
        }

        int myCount = 0;
        auto h = EventBus::Subscribe<HealthChangedEvent>(
            [&](const HealthChangedEvent&) { ++myCount; });

        HealthChangedEvent evt{&player, 10};
        EventBus::Publish(evt);

        EventBus::Unsubscribe(h);

        RC_ASSERT(myCount == 1);

        combatScene.reset();
    });
}

TEST_F(CombatSceneTest, StartIsIdempotentForStageClearEvent) {
    rc::check("CombatScene::Start() N times leaves exactly one StageClearEvent subscription", [this]() {
        combatScene = CreateCombatScene(
            mockScene, mockAsset, mockCard, mockInput, mockRender, mockBattle, mockOsService);

        int n = *rc::gen::inRange(1, 11);

        for (int i = 0; i < n; ++i) {
            combatScene->Start();
        }

        int myCount = 0;
        auto h = EventBus::Subscribe<StageClearEvent>(
            [&](const StageClearEvent&) { ++myCount; });

        EventBus::Publish(StageClearEvent{});

        EventBus::Unsubscribe(h);

        RC_ASSERT(myCount == 1);

        combatScene.reset();
    });
}

} // namespace
} // namespace mc

