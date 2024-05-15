cmake_minimum_required(VERSION 3.25.0)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Setup engine library
include("${PROJECT_SOURCE_DIR}/cmake/utility.cmake")
set(MYTHOS_TESTBED_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
gather(MYTHOS_TESTBED_FILES ${MYTHOS_TESTBED_INCLUDE_DIR} "*.hpp;*.cpp")

file(GLOB_RECURSE MYTHOS_ENGINE_HEADERS "${PROJECT_SOURCE_DIR}/mythos/engine/src/*.hpp")
add_executable(testbed
    ${MYTHOS_ENGINE_HEADERS}
    ${MYTHOS_TESTBED_FILES}
)

# Setup include directories
target_include_directories(testbed
    PUBLIC
        "${PROJECT_SOURCE_DIR}/vendor/myl/include"
        "${PROJECT_SOURCE_DIR}/vendor/spdlog/include"
        "${PROJECT_SOURCE_DIR}/mythos/engine/src"
        ${MYTHOS_TESTBED_INCLUDE_DIR}
)

# Linking
get_build_type_nickname(BUILD_EXTENSION)
string(REGEX REPLACE "bin-int(.+)" "bin\\1-${BUILD_EXTENSION}/lib/mythos_engine.lib" MYTHOS_ENGINE_LIB_PATH "${CMAKE_BINARY_DIR}")
target_link_libraries(testbed
    PRIVATE
        ${MYTHOS_ENGINE_LIB_PATH}
)