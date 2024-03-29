# Target Platform and Target Architecture
set(CMAKE_SYSTEM_NAME "Windows")
set(CMAKE_SYSTEM_PROCESSOR "X86")

# Compiler / Linker Binaries and Respective Flags
include("${CMAKE_CURRENT_LIST_DIR}/clang-cl.cmake")  # Setup Clang-CL and LLD for C and C++

include("${CMAKE_CURRENT_LIST_DIR}/windows.cmake")

# Toolchain sysroot Settings
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM     NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY     ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE     ONLY)

# Optionally reduce compiler sanity check when cross-compiling.
#set(CMAKE_TRY_COMPILE_TARGET_TYPE         STATIC_LIBRARY)