cmake_minimum_required(VERSION 3.21)

if(PROJECT_IS_TOP_LEVEL)
    message("Engine is a main project - some additional flags will be available")

    # force Debug build type if nothing else has been provided
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
    endif()
endif()

option(COMPILE_PLAYGROUND "Compile engine's playground" OFF)
option(DRAW_DEBUG "Draw debug info, such as hitboxes" OFF)

# set project name and version
project(engine
    LANGUAGES CXX
    VERSION 0.1
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prefer new GLVND over OLD
set(OpenGL_GL_PREFERENCE GLVND)

add_library(engine STATIC
    engine/raybuff.cpp
    engine/raybuff.hpp
    engine/node.cpp
    engine/node.hpp
    engine/scene.cpp
    engine/scene.hpp
    engine/core.cpp
    engine/core.hpp
    engine/mapgen.hpp
    engine/quadtree.hpp
    engine/settings.cpp
    engine/settings.hpp
    engine/sound.hpp
    engine/sprite.cpp
    engine/sprite.hpp
    engine/text.cpp
    engine/text.hpp
    engine/ui.cpp
    engine/ui.hpp
    engine/utility.cpp
    engine/utility.hpp
    engine/storage.cpp
    engine/storage.hpp
    engine/tasks.hpp
)

target_include_directories(engine PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${PROJECT_SOURCE_DIR}/engine>"
)

# Disable warnings about vsprintf being deprecated on windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR MINGW)
    target_compile_definitions(engine PRIVATE
        _CRT_SECURE_NO_WARNINGS
    )
endif()

# Draw debug info if specific flag has been received
# I hope this won't eat previous target_compile_definitions statement.
if(DRAW_DEBUG)
    target_compile_definitions(engine PRIVATE
        "DRAW_DEBUG"
    )
endif()

if(PROJECT_IS_TOP_LEVEL)
    target_compile_options(engine PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra -Wpedantic -Werror -Wextra-semi -Wsuggest-override -Wno-missing-field-initializers>
        $<$<CXX_COMPILER_ID:MSVC>:/Wall /w34263 /w34266>
    )

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# If building with debug - enable sanitizer for hard-to-debug moments
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

# Setup raylib
add_subdirectory("${PROJECT_SOURCE_DIR}/dependencies/raylib")
target_link_libraries(engine raylib)
target_include_directories(engine PUBLIC ${raylib_INCLUDE_DIRS})

# Setup fmt
add_subdirectory("${PROJECT_SOURCE_DIR}/dependencies/fmt")
target_link_libraries(engine fmt)
target_include_directories(engine PUBLIC ${fmt_INCLUDE_DIRS})

# Setup spdlog
add_subdirectory("${PROJECT_SOURCE_DIR}/dependencies/spdlog")
target_link_libraries(engine spdlog)
target_include_directories(engine PUBLIC ${spdlog_INCLUDE_DIRS})

# Setup tomlplusplus
add_subdirectory("${PROJECT_SOURCE_DIR}/dependencies/tomlplusplus")
target_link_libraries(engine tomlplusplus::tomlplusplus)
target_include_directories(engine PUBLIC ${tomlplusplus_INCLUDE_DIRS})

if(PROJECT_IS_TOP_LEVEL)
    add_custom_target(compile_commands
        WORKING_DIRECTORY ${CMAKE_BUILD_DIR}
        BYPRODUCTS ${CMAKE_SOURCE_DIR}/compile_commands.json
        COMMAND cp -f ./compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json
        VERBATIM)

    add_custom_target(format
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND find engine \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format -style=file -i {} \;
        VERBATIM)
endif()

if(COMPILE_PLAYGROUND)
    add_subdirectory(playground)
endif()
