# Setup Clang-CL and LLD for C and C++

# Should search for the correct location, This will fail for 32 bit
# BUG: This is a hack
set(CMAKE_MT "C:/Program Files (x86)/Windows Kits/10/bin/10.0.19041.0/x64/mt.exe")

set(CMAKE_C_COMPILER "clang-cl")
set(CMAKE_C_FLAGS "/Wall /std:c${CMAKE_C_STANDARD}")
set(CMAKE_C_FLAGS_RELEASE        "/O2 /DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "/O2 /Zo /DDEBUG")
set(CMAKE_C_FLAGS_DEBUG          "/Od /Zi /DDEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL     "/O1 /DNDEBUG")

set(CMAKE_CXX_COMPILER "clang-cl")
set(CMAKE_CXX_FLAGS "/Wall /std:c++${CMAKE_CXX_STANDARD} -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-pre-c++14-compat")
set(CMAKE_CXX_FLAGS_RELEASE        "/O2 /DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /Zo /DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG          "/Od /Zi /DDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "/O1 /DNDEBUG")

set(CMAKE_LINKER "lld-link")
#set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld -manifest:no")