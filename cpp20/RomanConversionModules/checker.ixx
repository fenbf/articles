module;

#include <iostream>		// in global module fragment
#include <format>
#include <compare>

export module checker;

export class Tester {
public:
	void check(int expected, int actual) {
		if (expected != actual) {
			std::cout << "Test failed: expected " << expected << " but got " << actual << '\n';
			++failed;
		}
		else {
			std::cout << "Test passed: got expected result " << expected << '\n';
			++passed;
		}
	}

	void printStats() const {
		std::cout << std::format("Passed: {} / Failed : {}\n", getPassed(), getFailed());
	}

	int getPassed() const { return passed; }
	int getFailed() const { return failed; }

private:
	int passed = 0;
	int failed = 0;
};