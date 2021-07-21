set(PLUGIN_NAME sdl)
set(INCLUDES ${CSOUND_INCLUDE_DIRS})
set(LIBS "")

# Dependencies
find_package(SDL2)
check_deps(SDL2_FOUND)
list(APPEND LIBS ${SDL2_LIBRARY})
list(APPEND INCLUDES ${SDL2_INCLUDE_DIR})

# Source files
set(CPPFILES src/opcodes.cpp)
make_plugin(${PLUGIN_NAME} "${CPPFILES}" ${LIBS})
target_include_directories(${PLUGIN_NAME} PRIVATE ${INCLUDES})
