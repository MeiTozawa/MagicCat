include(FetchContent)

FetchContent_Declare(
    rapidcheck
    GIT_REPOSITORY https://github.com/emil-e/rapidcheck.git
    GIT_TAG        master
)

set(RC_ENABLE_GTEST ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(rapidcheck)

add_executable(MagicCatTests
    test_main.cpp
    test_HealthComponent.cpp
    test_Character.cpp
    test_Player.cpp
    test_Enemy.cpp
    test_EventBus.cpp
    test_CardService.cpp
    test_CardServiceUtils.cpp
    test_ConfigService.cpp
    test_SceneService.cpp
    test_CombatIntegration.cpp
    test_GameFlowIntegration.cpp
    test_BattleService.cpp
    test_BattleServiceProperties.cpp
    test_SequentialBattlesIntegration.cpp
    test_BattleServiceCodeQualityProperties.cpp
)

target_compile_features(MagicCatTests PRIVATE cxx_std_23)

target_include_directories(MagicCatTests PRIVATE
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/MagicCat/program
)

target_link_libraries(MagicCatTests PRIVATE
    MagicCatCore
    gtest_main
    gmock
    rapidcheck
    rapidcheck_gtest
)

include(GoogleTest)
gtest_discover_tests(MagicCatTests)
