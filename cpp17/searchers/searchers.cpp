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

std::vector<std::string> GetNeedleStrings(int argc, const char** argv, std::string &testString, size_t needleCount)
{
	const auto testStringLen = testString.length();
	std::vector<std::string> out;
	out.resize(needleCount);
	long posStart = 0;
	size_t patternLen = testStringLen;
	if (argc > 3)
	{
		const size_t tempLen = atoi(argv[3]);
		if (tempLen == 0) // some word?
		{
			std::cout << "needle is a string...\n";
			out[0] = argv[3];
		}
		else
		{
			patternLen = tempLen > testStringLen ? testStringLen : tempLen;
			const int pos = argc > 4 ? atoi(argv[4]) : 0;

			if (pos == 0)
			{
				std::cout << "needle from the start...\n";
				posStart = 0;				
			}
			else if (pos == 1)
			{
				std::cout << "needle from the center...\n";
				posStart = static_cast<long>(testStringLen / 2 - patternLen / 2 - needleCount/2);
			}
			else
			{
				std::cout << "needle from the end\n";				
				posStart = static_cast<long>(testStringLen - patternLen - needleCount);
			}
		}
	}
	else
	{
		// just take the 1/4 of the input string from the end...
		posStart = static_cast<long>(testStringLen - testStringLen / 4 - needleCount / 2);
	}

	posStart = std::max(0L, posStart);
	for (size_t i = 0; i < needleCount; ++i)
	{
		out[i] = testString.substr(posStart + i, patternLen);
	}

	return out;
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

	const size_t PATTERNS = ITERS / 10;
	const auto needles = GetNeedleStrings(argc, argv, testString, PATTERNS);
	std::cout << "patterns count: " << needles.size() << '\n';
	std::cout << "patterns len: " << needles[0].length() << '\n';

	std::cout << "5 first patterns, 30 letters max: \n";
	for (auto pat : needles | std::views::take(5)) {
		if (pat.length() > 30)
			pat[30] = '\0';
		std::cout << pat.c_str() << '\n';
	}

	RunAndMeasure("string::find", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			std::size_t found = testString.find(needles[i % PATTERNS]);
			if (found == std::string::npos)
				std::cout << "The string " << needles[i % PATTERNS] << " not found\n";
		}
		return 0;
	});

	RunAndMeasure("strchr_find", [&]() {		
		for (size_t i = 0; i < ITERS; ++i)
		{
			const char* strEnd = &testString[0] + testString.length();
			const char* needleEnd = &needles[i % PATTERNS][0] + needles[i % PATTERNS].length();
			auto found = strchr_find(&testString[0], strEnd, &needles[i % PATTERNS][0], needleEnd);
			if (found == nullptr)
				std::cout << "The string " << needles[i % PATTERNS] << " not found\n";
		}
		return 0;
	});

	RunAndMeasure("std::ranges::search", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			auto res = std::ranges::search(testString, needles[i % PATTERNS]);
			if (res.empty())
				std::cout << "The string " << needles[i % PATTERNS] << " not found\n";
		}
		return 0;
	});

	RunAndMeasure("default searcher", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			auto it = std::search(testString.begin(), testString.end(),
				std::default_searcher(
					needles[i % PATTERNS].begin(), needles[i % PATTERNS].end()));
			if (it == testString.end())
				std::cout << "The string " << needles[i % PATTERNS] << " not found\n";
		}
		return 0;
	});

	RunAndMeasure("boyer_moore_searcher init only", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			std::boyer_moore_searcher b(needles[i % PATTERNS].begin(), needles[i % PATTERNS].end());
			DoNotOptimizeAway(&b);
		}
		return 0;
	});

	RunAndMeasure("boyer_moore_searcher", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			auto it = std::search(testString.begin(), testString.end(),
				std::boyer_moore_searcher(
					needles[i % PATTERNS].begin(), needles[i % PATTERNS].end()));
			if (it == testString.end())
				std::cout << "The string " << needles[i % PATTERNS] << " not found\n";
		}
		return 0;
	});

	RunAndMeasure("boyer_moore_horspool_searcher init only", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			std::boyer_moore_horspool_searcher b(needles[i % PATTERNS].begin(), needles[i % PATTERNS].end());
			DoNotOptimizeAway(&b);
		}
		return 0;
	});

	RunAndMeasure("boyer_moore_horspool_searcher", [&]() {
		for (size_t i = 0; i < ITERS; ++i)
		{
			auto it = std::search(testString.begin(), testString.end(),
				std::boyer_moore_horspool_searcher(
					needles[i % PATTERNS].begin(), needles[i % PATTERNS].end()));
			if (it == testString.end())
				std::cout << "The string " << needles[i % PATTERNS] << " not found\n";
		}
		return 0;
	});

	/*std::vector<int> testVector(1000000);
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
	});*/
}

/*example run:
PS C:\Users\Admin\Documents\GitHub\articles\cpp17\x64\Release> .\searchers.exe ..\..\..\..\GutenbergBooks\largest.txt 1000 10000 2
string length: 547412
test iterations: 1000
needle from the end
patterns count: 100
patterns len: 10000
5 first patterns, 30 letters max:
ject Gutenberg-tm trademark.
ect Gutenberg-tm trademark.  C
ct Gutenberg-tm trademark.  Co
t Gutenberg-tm trademark.  Con
 Gutenberg-tm trademark.  Cont
string::find: 393.926 ms
strchr_find: 270.201 ms
std::ranges::search: 1706.21 ms
default searcher: 756.361 ms
boyer_moore_searcher init only: 29.7993 ms
boyer_moore_searcher: 56.3499 ms
boyer_moore_horspool_searcher init only: 5.3273 ms
boyer_moore_horspool_searcher: 29.3569 ms
*/