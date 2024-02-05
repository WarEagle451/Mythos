cmake_minimum_required(VERSION 3.28.0)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Setup engine library
include("${PROJECT_SOURCE_DIR}/cmake/utility.cmake")
set(MYTHOS_ENGINE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
gather(MYTHOS_ENGINE_FILES ${MYTHOS_ENGINE_INCLUDE_DIR} "*.hpp;*.cpp")

add_library(mythos_engine SHARED
    ${MYTHOS_ENGINE_FILES}
)

# Setup include directories
target_include_directories(mythos_engine
    PUBLIC
        "${PROJECT_SOURCE_DIR}/vendor/myl/include"
        "${PROJECT_SOURCE_DIR}/vendor/spdlog/include"
        ${MYTHOS_ENGINE_INCLUDE_DIR}
)

# Definitions
include("${PROJECT_SOURCE_DIR}/cmake/definitions.cmake")
target_compile_definitions(mythos_engine
    PUBLIC
        ${MYTHOS_DEFINITIONS}
)