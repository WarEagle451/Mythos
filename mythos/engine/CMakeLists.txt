cmake_minimum_required(VERSION 3.25.0)
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

# Find Vulkan
include("${PROJECT_SOURCE_DIR}/cmake/find_vulkan.cmake")

# Setup include directories
target_include_directories(mythos_engine
    PUBLIC
        "${PROJECT_SOURCE_DIR}/vendor/myl/include"
        "${PROJECT_SOURCE_DIR}/vendor/spdlog/include"
        ${VULKAN_INCLUDE_PATH}
        ${MYTHOS_ENGINE_INCLUDE_DIR}
)

# Libraries
# TODO: Make a function that gathers all the libs into 1 variable
target_link_libraries(mythos_engine
    PUBLIC
        "${VULKAN_LIB_PATH}/vulkan-1.lib"
        "${VULKAN_LIB_PATH}/VkLayer_utils.lib"
        "${VULKAN_LIB_PATH}/shaderc_shared.lib"
        "${VULKAN_LIB_PATH}/spirv-cross-core.lib"
        "${VULKAN_LIB_PATH}/spirv-cross-glsl.lib"
)

# Definitions
target_compile_definitions(mythos_engine
    PRIVATE
        "MYL_SHARED_LIBRARY"
)