add_executable(MagicCatTests
    test_main.cpp
    test_HealthComponent.cpp
    test_Character.cpp
    test_Player.cpp
    test_Enemy.cpp
    test_EventBus.cpp
    test_CardService.cpp
    test_ConfigService.cpp
    test_SceneService.cpp
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
)

include(GoogleTest)
gtest_discover_tests(MagicCatTests)
