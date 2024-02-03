# Setup for executable
include("${PROJECT_SOURCE_DIR}/cmake/utility.cmake")
set(MYTHOS_ENGINE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
gather(MYTHOS_ENGINE_FILES ${MYTHOS_ENGINE_INCLUDE_DIR} "*.hpp;*.cpp")
add_executable(mythos
    ${MYL_FILES}
    ${MYTHOS_ENGINE_FILES}
)

# Setup include directories
include("${PROJECT_SOURCE_DIR}/vendor/myl/cmake/files.cmake")
target_include_directories(
    mythos PUBLIC
    ${MYL_INCLUDE_DIR}
    ${MYTHOS_ENGINE_INCLUDE_DIR}
)

# Definitions
include("${PROJECT_SOURCE_DIR}/cmake/definitions.cmake")
target_compile_definitions(mythos
    PUBLIC
        ${MYTHOS_DEFINITIONS}
    PRIVATE
        _CRT_SECURE_NO_WARNINGS
)