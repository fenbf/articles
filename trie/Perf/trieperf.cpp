#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <string_view>
#include <string>
#include <vector> 

#include "../trie.h"

using namespace std::literals;

const std::string_view LoremIpsumStrv{ "Lorem ipsum dolor sit amet, consectetur adipiscing elit, "
"sed do eiusmod tempor incididuntsuperlongwordsuper ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
"quis nostrud exercitation ullamco laboris nisi ut aliquipsuperlongword ex ea commodo consequat. Duis aute "
"irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. "
"Excepteur sint occaecat cupidatatsuperlongword non proident, sunt in culpa qui officia deserunt mollit anim id est laborum." };

std::vector<std::string_view>
splitSVStd(std::string_view strv, std::string_view delims = " ")
{
	std::vector<std::string_view> output;
	//output.reserve(strv.length() / 4);
	auto first = strv.begin();

	while (first != strv.end())
	{
		const auto second = std::find_first_of(first, std::cend(strv),
			std::cbegin(delims), std::cend(delims));
		//std::cout << first << ", " << second << '\n';
		if (first != second)
		{
			output.emplace_back(strv.substr(std::distance(strv.begin(), first), std::distance(first, second)));
		}

		if (second == strv.end())
			break;

		first = std::next(second);
	}

	return output;
}

int main(int argc, const char** argv) {
	std::string testString{ LoremIpsumStrv };

	if (argc == 1)
	{
		std::cout << "trie-perf.exe filename iterations\nNow using default params...\n\n";
	}

	if (argc > 1 && "nofile"s != argv[1])
	{
		std::ifstream inFile(argv[1]);

		std::stringstream strStream;
		strStream << inFile.rdbuf();
		testString = strStream.str();
	}

	std::cout << "string length: " << testString.length() << '\n';

	auto extractedWords = splitSVStd(testString, " ,.\n");

	std::cout << "extracted words: " << extractedWords.size() << '\n';

	const size_t ITERS = argc > 2 ? atoi(argv[2]) : 1000;
	std::cout << "test iterations: " << ITERS << '\n';
}