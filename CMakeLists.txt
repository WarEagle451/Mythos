cmake_minimum_required(VERSION 3.28.0)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Override generators that modify the following variables
string(APPEND CMAKE_ARCHIVE_OUTPUT_DIRECTORY "/$<0:>")
string(APPEND CMAKE_LIBRARY_OUTPUT_DIRECTORY "/$<0:>")
string(APPEND CMAKE_RUNTIME_OUTPUT_DIRECTORY "/$<0:>")

include("cmake/utility.cmake")
extract_version("MYTHOS_VERSION" "mythos/engine/src/mythos/version.hpp" MYTHOS_VERSION)
project(
    MYTHOS
    VERSION ${MYTHOS_VERSION}
    DESCRIPTION "Mythos game engine."
    HOMEPAGE_URL "https://github.com/WarEagle451/Mythos"
    LANGUAGES CXX
)
message(STATUS "Build Mythos: ${MYTHOS_VERSION}")

# add_compile_options
#set(SPDLOG_BUILD_WARNINGS OFF)
set(SPDLOG_NO_EXCEPTIONS ON)
#set(SPDLOG_USE_STD_FORMAT ON)
add_compile_definitions("SPDLOG_NO_EXCEPTIONS") #why
add_compile_definitions("SPDLOG_BUILD_WARNINGS") #why
add_compile_definitions("FMT_EXCEPTIONS=0")#why

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE) # Must be set to TRUE for spdlog to work with shared libraries

#Use spdlog static lib

add_subdirectory("vendor/myl")
add_subdirectory("vendor/spdlog")
add_subdirectory("mythos/engine")
add_subdirectory("mythos/editor")