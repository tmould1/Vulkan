#include "HelloTriangleApplication.h"

#include <iostream>
#include <functional>
#include <cstdlib>

int main() {
	HelloTriangleApplication app;

	try {
		app.Run();
	}
	catch (const std::runtime_error& e) {
		char s;
		std::cerr << e.what() << std::endl;
		std::cin >> s;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}