// CoroutineTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <experimental/generator>
#include <iostream>

void print(std::string_view sv) {
	std::cout << sv;
}

std::experimental::generator<int> TwoReturns(int n) {
	co_yield n;
	co_yield n + 1;
	co_return;
}

std::experimental::generator<int> Counter(int n) {
	for (int i = 0; i < n; ++i)
	{
		std::cout << i;
		print("generated\n");
		co_yield i;
	}

	auto handle = TwoReturns(100);
	co_yield *handle.begin();
}
int main()
{
	auto handle = TwoReturns(100);
	std::cout << *handle.begin();

	for (auto i : Counter(10))
		std::cout << "consumed " << i << '\n';
}