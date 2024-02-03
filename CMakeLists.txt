cmake_minimum_required(VERSION 3.28.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Override generators that modify the following variables
string(APPEND CMAKE_ARCHIVE_OUTPUT_DIRECTORY "/$<0:>")
string(APPEND CMAKE_LIBRARY_OUTPUT_DIRECTORY "/$<0:>")
string(APPEND CMAKE_RUNTIME_OUTPUT_DIRECTORY "/$<0:>")

project(
    MYTHOS
    VERSION 0.0.0
    DESCRIPTION "Mythos game engine."
    HOMEPAGE_URL "https://github.com/WarEagle451/Mythos"
    LANGUAGES CXX
)

add_subdirectory("vendor/myl")
add_subdirectory("mythos/engine")