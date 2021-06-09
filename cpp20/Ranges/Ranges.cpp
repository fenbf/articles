#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>

int main()
{
	const std::vector numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	auto even = [](int i) { return 0 == i % 2; };

	{ // "standard way"
		std::vector<int> temp(numbers.begin() + 2, numbers.end());
		std::vector<int> temp2;
		std::copy_if(begin(temp), end(temp), std::back_inserter(temp2), even);
		for (auto iter = rbegin(temp2); iter != rend(temp2); ++iter)
			std::cout << *iter << ' ';
		std::cout << '\n';
	}

	std::ranges::reverse_view rv{
		std::ranges::filter_view{
			std::ranges::drop_view { numbers, 2 }, even
		}
	};
	for (int i : rv)
		std::cout << i << ' ';

	std::cout << '\n';

	for (int i : numbers | std::views::drop(2) | std::views::filter(even) | std::views::reverse)
		std::cout << i << ' ';
}