file(GLOB_RECURSE MAGICCAT_SOURCES CONFIGURE_DEPENDS
        "program/*.cpp"
)
file(GLOB_RECURSE MAGICCAT_MODULES CONFIGURE_DEPENDS
        "program/*.ixx"
)

add_executable(MagicCat WIN32
    MagicCat.cpp
    ${MAGICCAT_SOURCES}
)

# DxLib.hはwindows.hをインクルードしているため、以下のマクロの競合が発生し、tweenyライブラリ内のstd::minおよびstd::maxが使用できなくなってしまいました
# #define min(a,b) (((a) < (b)) ? (a) : (b))
# #define max(a,b) (((a) > (b)) ? (a) : (b))
target_compile_definitions(MagicCat PRIVATE NOMINMAX)

set_target_properties(MagicCat PROPERTIES UNITY_BUILD ON)

target_sources(MagicCat
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
        ${MAGICCAT_MODULES}
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
