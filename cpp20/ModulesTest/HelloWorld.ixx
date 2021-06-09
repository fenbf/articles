module;

#include <iostream>

export module HelloWorldModule;


// interface:
export void HelloWorld();

// implementation:
void HelperFunction(); // private

void HelloWorld() {
	HelperFunction();
	std::cout << "Hello World from a module!\n";
}

// private implementation:
void HelperFunction() {
	std::cout << "some computation in a helper function...\n";
}