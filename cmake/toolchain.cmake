if(CMAKE_TOOLCHAIN_FILE STREQUAL "")
    message(ERROR "No toolchain file specified, using cmake defaults.")
else()
    include("${CMAKE_TOOLCHAIN_FILE}")
endif()