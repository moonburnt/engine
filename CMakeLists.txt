cmake_minimum_required(VERSION 3.10)

#set(CMAKE_CXX_COMPILER clang++)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
    engine/core.cpp
    engine/core.hpp
    engine/mapgen.hpp
    engine/quadtree.hpp
    engine/settings.cpp
    engine/settings.hpp
    engine/sound.cpp
    engine/sound.hpp
    engine/sprite.cpp
    engine/sprite.hpp
    engine/ui.cpp
    engine/ui.hpp
    engine/utility.cpp
    engine/utility.hpp
    engine/storage.cpp
    engine/storage.hpp
)

target_compile_options(engine
    PUBLIC
    -Wall -Wextra -Wpedantic -Werror -Wextra-semi -Wsuggest-override -Wno-missing-field-initializers
)

target_include_directories(engine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

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
# Jk. Not setting it up, coz its CMakeLists is GARBAGE and does not work properly.
# Instead, we will just include the header where necessary and thats it.
