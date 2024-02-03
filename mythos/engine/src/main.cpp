#include <mythos/version.hpp>

#include <iostream>

auto main(int arg_count, char** args) -> int {
    std::cout << "Major: " << MYTHOS_VERSION_MAJOR << std::endl;
    std::cout << "Minor: " << MYTHOS_VERSION_MINOR << std::endl;
    std::cout << "Patch: " << MYTHOS_VERSION_PATCH << std::endl;

    return 0;
}