#include <iostream>
#include <string>
#include <algorithm>
#include <functional>
#include <chrono>
#include <fstream>
#include <string_view>
#include <sstream>
#include <numeric>
#include "simpleperf.h"
#include <string.h>
#include <ranges>

using namespace std::literals;

const std::string_view LoremIpsumStrv{ "Lorem ipsum dolor sit amet, consectetur adipiscing elit, "
"sed do eiusmod tempor incididuntsuperlongwordsuper ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
"quis nostrud exercitation ullamco laboris nisi ut aliquipsuperlongword ex ea commodo consequat. Duis aute "
"irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. "
"Excepteur sint occaecat cupidatatsuperlongword non proident, sunt in culpa qui officia deserunt mollit anim id est laborum." };

std::string GetNeedleString(int argc, const char** argv, std::string &testString)
{
	const auto testStringLen = testString.length();
	if (argc > 3)
	{
		const size_t tempLen = atoi(argv[3]);
		if (tempLen == 0) // some word?
		{
			std::cout << "needle is a string...\n";
			return argv[3];
		}
		else
		{
			const size_t PATTERN_LEN = tempLen > testStringLen ? testStringLen : tempLen;
			const int pos = argc > 4 ? atoi(argv[4]) : 0;

			if (pos == 0)
			{
				std::cout << "needle from the start...\n";
				return testString.substr(0, PATTERN_LEN);
				
			}
			else if (pos == 1)
			{
				std::cout << "needle from the center...\n";
				return testString.substr(testStringLen / 2 - PATTERN_LEN / 2 - 1, PATTERN_LEN);
			}
			else
			{
				std::cout << "needle from the end\n";				
				return testString.substr(testStringLen - PATTERN_LEN - 1, PATTERN_LEN);
			}
		}
	}
	
	// just take the 1/4 of the input string from the end...
	return testString.substr(testStringLen - testStringLen/4 - 1, testStringLen/4);
}

// based on a twitter suggestion - Gregory Pakosz 
// https://gist.github.com/gpakosz/22ff3793017d6823c65727c8ba3480b7
static char* strchr_find(const char* haystackBegin, const char* haystackEnd, const char* needleBegin, const char* needleEnd)
{
	if (haystackEnd == NULL)
		haystackEnd = haystackBegin + strlen(haystackBegin);

	if (needleEnd == NULL)
		needleEnd = needleBegin + strlen(needleBegin);

	size_t haystackLength = haystackEnd - haystackBegin;
	size_t needleLength = needleEnd - needleBegin;

	if (haystackLength < needleLength)
		return NULL;

	if (needleBegin == needleEnd)
		return (char*)haystackBegin;

	if (needleLength == 1)
		return (char*)memchr(haystackBegin, needleBegin[0], haystackLength);

	while ((haystackBegin = (const char*)memchr(haystackBegin, needleBegin[0], haystackLength)) != NULL)
	{
		haystackLength = (size_t)(haystackEnd - haystackBegin);
		if (haystackLength == 0 || haystackLength < needleLength)
			return NULL;

		if (haystackBegin[needleLength - 1] == needleBegin[needleLength - 1])
		{
			if (needleLength == 2 || memcmp(haystackBegin + 1, needleBegin + 1, needleLength - 2) == 0)
				return (char*)haystackBegin;
		}

		++haystackBegin;
		--haystackLength;
	}

	return nullptr;
}

int main(int argc, const char** argv)
{
	std::string testString{ LoremIpsumStrv };

	if (argc == 1)
	{
		std::cout << "searchers.exe filename iterations pattern_len\n";
		return 0;
	}

	if (argc > 1 && "nofile"s != argv[1])
	{
		std::ifstream inFile(argv[1]);

		std::stringstream strStream;
		strStream << inFile.rdbuf();
		testString = strStream.str();
	}

	std::cout << "string length: " << testString.length() << '\n';

	const size_t ITERS = argc > 2 ? atoi(argv[2]) : 1000;
	std::cout << "test iterations: " << ITERS << '\n';

	const auto needle = GetNeedleString(argc, argv, testString);
	std::cout << "pattern length: " << needle.length() << '\n';

	RunAndMeasure("string::find", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			std::size_t found = testString.find(needle);
			if (found == std::string::npos)
				std::cout << "The string " << needle << " not found\n";
		}
		return 0;
	});

	RunAndMeasure("strchr_find", [&]() {
		const char* strEnd = &testString[0] + testString.length();
		const char* needleEnd = &needle[0] + needle.length();
		for (size_t i = 0; i < ITERS; ++i)
		{
			auto found = strchr_find(&testString[0], strEnd, &needle[0], needleEnd);
			if (found == nullptr)
				std::cout << "The string " << needle << " not found\n";
		}
		return 0;
	});

	RunAndMeasure("std::ranges::search", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			auto res = std::ranges::search(testString, needle);
			if (res.empty())
				std::cout << "The string " << needle << " not found\n";
		}
		return 0;
	});

	RunAndMeasure("std::ranges::search 2", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			auto res = std::ranges::search(testString.begin(), testString.end(), needle.begin(), needle.end());
			if (res.empty())
				std::cout << "The string " << needle << " not found\n";
		}
		return 0;
		});

	RunAndMeasure("default searcher", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			auto it = std::search(testString.begin(), testString.end(),
				std::default_searcher(
					needle.begin(), needle.end()));
			if (it == testString.end())
				std::cout << "The string " << needle << " not found\n";
		}
		return 0;
	});

	RunAndMeasure("boyer_moore_searcher init only", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			std::boyer_moore_searcher b(needle.begin(), needle.end());
			DoNotOptimizeAway(&b);
		}
		return 0;
	});

	RunAndMeasure("boyer_moore_searcher", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			auto it = std::search(testString.begin(), testString.end(),
				std::boyer_moore_searcher(
					needle.begin(), needle.end()));
			if (it == testString.end())
				std::cout << "The string " << needle << " not found\n";
		}
		return 0;
	});

	RunAndMeasure("boyer_moore_horspool_searcher init only", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			std::boyer_moore_horspool_searcher b(needle.begin(), needle.end());
			DoNotOptimizeAway(&b);
		}
		return 0;
	});

	RunAndMeasure("boyer_moore_horspool_searcher", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			auto it = std::search(testString.begin(), testString.end(),
				std::boyer_moore_horspool_searcher(
					needle.begin(), needle.end()));
			if (it == testString.end())
				std::cout << "The string " << needle << " not found\n";
		}
		return 0;
	});

	std::vector<int> testVector(1000000);
	std::iota(testVector.begin(), testVector.end(), 0);
	std::vector vecNeedle(testVector.end() - 1000, testVector.end());

	RunAndMeasure("vector of ints default", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			auto it = std::search(testVector.begin(), testVector.end(),
				std::default_searcher(
					vecNeedle.begin(), vecNeedle.end()));
			if (it == testVector.end())
				std::cout << "The pattern " << needle << " not found\n";
		}
		return 0;
	});

	RunAndMeasure("boyer_moore_horspool_searcher vec init only", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			std::boyer_moore_horspool_searcher b(vecNeedle.begin(), vecNeedle.end());
			DoNotOptimizeAway(&b);
		}
		return 0;
	});

	RunAndMeasure("vector of ints", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			auto it = std::search(testVector.begin(), testVector.end(),
				std::boyer_moore_horspool_searcher(
					vecNeedle.begin(), vecNeedle.end()));
			if (it == testVector.end())
				std::cout << "The pattern " << needle << " not found\n";
		}
		return 0;
	});
}

/*example run:
PS C:\Users\Admin\Documents\GitHub\articles\cpp17\x64\Release> .\searchers.exe ..\..\..\..\GutenbergBooks\largest.txt 1000 10000 2
string length: 547412
test iterations: 1000
needle from the end
pattern length: 10000
string::find: 735.263 ms
strchr_find: 429.721 ms
std::ranges::search: 1666.91 ms
std::ranges::search 2: 1900.37 ms
default searcher: 1147.43 ms
boyer_moore_searcher init only: 93.1346 ms
boyer_moore_searcher: 123.746 ms
boyer_moore_horspool_searcher init only: 4.5282 ms
boyer_moore_horspool_searcher: 29.2597 ms
vector of ints default: 986.965 ms
boyer_moore_horspool_searcher vec init only: 282.69 ms
vector of ints: 283.962 ms
*/