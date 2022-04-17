#pragma once
#include "app.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main() {
	try {
		myl::app app{};
		app.run();
	}
	catch (std::exception& e) { std::cout << e.what() << std::endl; return EXIT_FAILURE; }
	catch (...) { std::cout << "an exception occured" << std::endl; return EXIT_FAILURE; }

	return EXIT_SUCCESS;
}