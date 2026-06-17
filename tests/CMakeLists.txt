add_executable(MagicCatTests
    test_main.cpp
)

target_compile_features(MagicCatTests PRIVATE cxx_std_23)

target_include_directories(MagicCatTests PRIVATE
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/MagicCat/program
)

target_link_libraries(MagicCatTests PRIVATE
    gtest_main
    gmock
)

include(GoogleTest)
gtest_discover_tests(MagicCatTests)
