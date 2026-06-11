cmake_minimum_required(VERSION 3.28)
project(MagicCat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DXLIB_DIR "" CACHE PATH "Path to DxLib")
if (NOT EXISTS "${DXLIB_DIR}")
    message(WARNING "DXLIB_DIR is not set or does not exist. Please configure it via CMakeUserPresets.json or provide it as a CMake parameter.")
endif ()

if (MSVC)
    add_compile_options("/utf-8")
    add_compile_definitions(WIN32 _WINDOWS UNICODE _UNICODE)
endif ()

add_subdirectory(common)
add_subdirectory(MagicCat)

target_precompile_headers(MagicCat PRIVATE
        <dxe.h>
        <tweeny-3.2.1.h>
)

find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif ()

target_link_options(MagicCat PRIVATE "-fuse-ld=lld")