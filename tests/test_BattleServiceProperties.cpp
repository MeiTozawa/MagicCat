// Property-based tests for IBattleService using RapidCheck.
// Verifies invariants that must hold for any arbitrary input.

// rpcndr.h (pulled in via DxLib) defines `small` as `char`, which conflicts
// with headers in some dependency chains. Undefine before gtest includes.
#ifdef small
#undef small
#endif

#include <gtest/gtest.h>
#include <rapidcheck.h>
#include <rapidcheck/gtest.h>

import BattleService;
import CardService;
import ConfigService;
import EventBus;
import Character;
import Enemy;
import Player;
import HealthComponent;

namespace mc {
namespace {

// ---------------------------------------------------------------------------
// Stub helpers (local to this translation unit)
// ---------------------------------------------------------------------------

class StubConfigService : public IConfigService
{
    std::vector<EnemyConfig> enemies_;
    std::vector<CardConfig>  cards_;
public:
    explicit StubConfigService(
        std::vector<EnemyConfig> enemies = {
            EnemyConfig{ 10, 1, 1, 1, 1, L"EnemyA", "Bunny" },
            EnemyConfig{ 12, 1, 2, 1, 1, L"EnemyB", "Bunny" },
            EnemyConfig{ 15, 1, 1, 3, 1, L"EnemyC", "Bunny" },
        },
        std::vector<CardConfig> cards = {
            { 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 },
        })
        : enemies_(std::move(enemies)), cards_(std::move(cards)) {}

    const std::vector<CardConfig>&  GetCardConfigs()  const override { return cards_;   }
    const std::vector<EnemyConfig>& GetEnemyConfigs() const override { return enemies_; }
};

class TaggedCharacter : public Character
{
public:
    explicit TaggedCharacter(ETag tag)
    {
        tags.push_back(tag);
    }
    void TakeDamage(int) const override {}
};

// ---------------------------------------------------------------------------
// Property 1: sequence always contains exactly 3 entries after StartStage().
// ---------------------------------------------------------------------------

RC_GTEST_PROP(BattleServiceProperties, SequenceAlwaysHasThreeEntries, ())
{
    StubConfigService cfg;
    auto cs = CreateCardService(cfg);
    auto bs = CreateBattleService(cfg, *cs);

    bs->StartStage();

    RC_ASSERT(bs->GetSequence().size() == 3u);
}

// ---------------------------------------------------------------------------
// Property 2: every entry in the sequence is drawn from the config pool
//             (matched by enemy name).
// ---------------------------------------------------------------------------

RC_GTEST_PROP(BattleServiceProperties, SequenceEntriesAreFromPool, ())
{
    StubConfigService cfg;
    auto cs = CreateCardService(cfg);
    auto bs = CreateBattleService(cfg, *cs);

    bs->StartStage();

    const auto& pool = cfg.GetEnemyConfigs();
    for (const auto& entry : bs->GetSequence())
    {
        bool found = false;
        for (const auto& poolEntry : pool)
        {
            if (poolEntry.name == entry.name)
            {
                found = true;
                break;
            }
        }
        RC_ASSERT(found);
    }
}

// ---------------------------------------------------------------------------
// Property 3: GetCurrentEnemyIndex() is always 0 immediately after StartStage().
// ---------------------------------------------------------------------------

RC_GTEST_PROP(BattleServiceProperties, CurrentIndexAlwaysStartsAtZero, ())
{
    StubConfigService cfg;
    auto cs = CreateCardService(cfg);
    auto bs = CreateBattleService(cfg, *cs);

    bs->StartStage();

    RC_ASSERT(bs->GetCurrentEnemyIndex() == 0);
}

// ---------------------------------------------------------------------------
// Property 4: one enemy death always increments the index from 0 to 1
//             (when index < 2).
// ---------------------------------------------------------------------------

RC_GTEST_PROP(BattleServiceProperties, EnemyDeathAlwaysIncrementsIndex, ())
{
    StubConfigService cfg;
    auto cs = CreateCardService(cfg);
    auto bs = CreateBattleService(cfg, *cs);

    bs->StartStage();
    RC_ASSERT(bs->GetCurrentEnemyIndex() == 0);

    TaggedCharacter enemy(ETag::Enemy);
    EventBus::Publish(DeathEvent(&enemy));

    RC_ASSERT(bs->GetCurrentEnemyIndex() == 1);
}

// ---------------------------------------------------------------------------
// Property 5: three consecutive enemy deaths always fire StageClearEvent
//             exactly once and reset the index to 0.
// ---------------------------------------------------------------------------

RC_GTEST_PROP(BattleServiceProperties, ThreeConsecutiveEnemyDeaths_StageClear, ())
{
    StubConfigService cfg;
    auto cs = CreateCardService(cfg);
    auto bs = CreateBattleService(cfg, *cs);

    bs->StartStage();

    int stageClearCount = 0;
    auto handle = EventBus::Subscribe<StageClearEvent>(
        [&](const StageClearEvent&) { ++stageClearCount; });

    TaggedCharacter enemy(ETag::Enemy);
    EventBus::Publish(DeathEvent(&enemy)); // kill 0 → index 1
    EventBus::Publish(DeathEvent(&enemy)); // kill 1 → index 2
    EventBus::Publish(DeathEvent(&enemy)); // kill 2 → StageClearEvent

    EventBus::Unsubscribe(handle);

    RC_ASSERT(stageClearCount == 1);
    RC_ASSERT(bs->GetCurrentEnemyIndex() == 0);
}

} // namespace
} // namespace mc
