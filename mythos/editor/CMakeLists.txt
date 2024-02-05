cmake_minimum_required(VERSION 3.28.0)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Setup engine library
include("${PROJECT_SOURCE_DIR}/cmake/utility.cmake")
set(MYTHOS_EDITOR_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
gather(MYTHOS_EDITOR_FILES ${MYTHOS_EDITOR_INCLUDE_DIR} "*.hpp;*.cpp")

file(GLOB_RECURSE MYTHOS_ENGINE_HEADERS "${PROJECT_SOURCE_DIR}/mythos/engine/src/*.hpp")
add_executable(editor
    ${MYTHOS_ENGINE_HEADERS}
    ${MYTHOS_EDITOR_FILES}
)

# Setup include directories
target_include_directories(editor
    PUBLIC
        "${PROJECT_SOURCE_DIR}/vendor/myl/include"
        "${PROJECT_SOURCE_DIR}/vendor/spdlog/include"
        "${PROJECT_SOURCE_DIR}/mythos/engine/src"
        ${MYTHOS_EDITOR_INCLUDE_DIR}
)

# Definitions
target_link_libraries(editor
    PRIVATE
        #"${CMAKE_BINARY_DIR}/mythos_engine${CMAKE_SHARED_LIBRARY_SUFFIX}"
        #"D:/Dev/C++/Mythos/build/bin/windows-x86_64-clang-cl-debug/mythos_engine.dll"
        "D:/Dev/C++/Mythos/build/bin/windows-x86_64-clang-cl-debug/lib/mythos_engine.lib"
)