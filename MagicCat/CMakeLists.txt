add_executable(MagicCat WIN32
    MagicCat.cpp
    program/gm_main.cpp
    program/Core/GameService.cpp
    program/Input/InputService.cpp
    program/UI/CombatUi.cpp
    program/UI/StartUi.cpp
    program/UI/UiService.cpp
    program/Character/Enmey/EnemyService.cpp
)

target_sources(MagicCat
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    program/Card/CardService.ixx
    program/Character/Character.ixx
    program/Character/Enmey/Enemy.ixx
    program/Character/Enmey/EnemyService.ixx
    program/Character/Health/HealthComponent.ixx
    program/Character/Health/IDamagable.ixx
    program/Character/Player/Player.ixx
    program/Core/GameService.ixx
    program/Input/InputService.ixx
    program/ServiceLocator.ixx
    program/UI/UiService.ixx
)

target_include_directories(MagicCat PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/program
    ${DXLIB_DIR}
)

target_link_directories(MagicCat PRIVATE
    ${DXLIB_DIR}
    $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/common/DirectXTex/lib_debug>
    $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/common/DirectXTex/lib_release>
    $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/common/DirectXTK/lib_debug>
    $<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/common/DirectXTK/lib_release>
)

target_link_libraries(MagicCat PRIVATE
    dxe
    DirectXTex.lib
    DirectXTK.lib
    D3DCompiler.lib
)
