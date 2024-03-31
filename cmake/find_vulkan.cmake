find_package(Vulkan REQUIRED)

if (NOT Vulkan_FOUND)
    message(STATUS "Vulkan SDK not found")
endif()

if (NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
    string(REGEX REPLACE "/Include" "" VULKAN_SDK_PATH ${Vulkan_INCLUDE_DIRS})
endif()

set(VULKAN_INCLUDE_PATH "${VULKAN_SDK_PATH}/Include")
set(VULKAN_LIB_PATH "${VULKAN_SDK_PATH}/Lib")