# Setup raylib
add_subdirectory(raylib)
target_link_libraries(engine PUBLIC raylib)
target_include_directories(engine PUBLIC ${raylib_INCLUDE_DIRS})

# Setup angelscript
add_subdirectory(angelscript)
target_link_libraries(engine PUBLIC angelscript)

# Setup fmt
add_subdirectory(fmt)
target_link_libraries(engine PUBLIC fmt)
target_include_directories(engine PUBLIC ${fmt_INCLUDE_DIRS})

# Setup spdlog
add_subdirectory(spdlog)
target_link_libraries(engine PUBLIC spdlog)
target_include_directories(engine PUBLIC ${spdlog_INCLUDE_DIRS})

# Setup tomlplusplus
add_subdirectory(tomlplusplus)
target_link_libraries(engine PUBLIC tomlplusplus::tomlplusplus)
target_include_directories(engine PUBLIC ${tomlplusplus_INCLUDE_DIRS})
