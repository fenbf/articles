#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <random>
#include <sstream>
#include <stack>
#include <set>
#include <string_view>
#include <string>
#include <vector> 

#include "../trie.h"
#include "simpleperf.h"

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
		std::cout << "trie-perf.exe filename iterations\nNow using default params...\n\n";

	if (argc > 1 && "nofile"s != argv[1]) {
		std::ifstream inFile(argv[1]);

		std::stringstream strStream;
		strStream << inFile.rdbuf();
		testString = strStream.str();
	}

	std::cout << "string length: " << testString.length() << '\n';

	const auto extractedWords = splitSVStd(testString, " ,.\n");

	std::cout << "extracted words: " << extractedWords.size() << '\n';

	const size_t ITERS = argc > 2 ? atoi(argv[2]) : 10;
	std::cout << "test iterations: " << ITERS << '\n';

	std::set<std::string> setWords;
	Trie trieWords;

	RunAndMeasure("set insert words", [&setWords, &extractedWords]() {
		for (auto& word : extractedWords)
			setWords.insert({ word.data(), word.length() });
		return 0;
	});

	RunAndMeasure("trie insert words", [&trieWords, &extractedWords]() {
		for (auto& word : extractedWords)
			trieWords.Insert(word);
		return 0;
	});

	std::uniform_int_distribution<size_t> distr{ 0, extractedWords.size()-1 };
	std::random_device engine;
	std::mt19937 noise{ engine() };

	std::vector<std::string> wordsToSearch(ITERS);
	std::ranges::generate(wordsToSearch, [&distr, &noise, &extractedWords]() {
		auto w = extractedWords[distr(noise)];
		return std::string{ w.data(), w.length() };
	});

	RunAndMeasure("set search ITER random words", [&setWords, &wordsToSearch]() {
		size_t cnt = 0;
		for (auto& word : wordsToSearch)
		{
			if (setWords.find(word) != setWords.end())
				++cnt;
		}
		return cnt;
	});

	RunAndMeasure("trie search ITER random words", [&trieWords, &wordsToSearch]() {
		size_t cnt = 0;
		for (auto& word : wordsToSearch)
		{
			if (trieWords.Find(word))
				++cnt;
		}
		return cnt;
	});

	std::vector<std::string> prefixWords(ITERS);
	std::transform(wordsToSearch.begin(), wordsToSearch.end(), prefixWords.begin(), [](const std::string& word) {
		if (word.length() > 4) {
			return word.substr(0, word.length() / 2);
		}
		return word;
	});

	RunAndMeasure("set prefix search", [&setWords, &prefixWords]() {
		size_t cnt = 0;
		for (auto& prefix : prefixWords) {
			std::vector<std::string> out;
			for (auto it = setWords.lower_bound(prefix); it != setWords.end(); ++it) {
				if (it->starts_with(prefix)) {
					out.push_back({ it->data(), it->length() });
					//std::cout << *it << ", ";
				}
				else
					break;
			}
			cnt += out.size();
			//std::cout << '\n';
		}
		return cnt;
		});

	RunAndMeasure("trie prefix search", [&trieWords, &prefixWords]() {
		size_t cnt = 0;
		for (auto& prefix : prefixWords) {
			auto vec = trieWords.Match(prefix);
			/*for (auto& w : vec)
				std::cout << w << ", ";*/
			cnt += vec.size();
			//std::cout << '\n';
		}
		return cnt;
		});
}