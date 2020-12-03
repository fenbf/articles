#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector> 
#include "trie.h"

int main() {
	Trie tr{ 
		{"hello"},
		{"head"},
		{"heap"},
		{"abc"},
		{"abstract"},
		{"absolute"}  
	};

	auto PrintWords = [](const auto& vecWords) {
		for (auto& word : vecWords)
			std::cout << "    " << word << '\n';
	};

	std::cout << "num words in the Trie: " << tr.Size() << " num nodes: " << tr.NumNodes() << '\n';

	std::cout << "match all:\n";
	PrintWords(tr.Match(""));

	std::cout << "match 'he':\n";
	PrintWords(tr.Match("he"));

	std::cout << "removing 'hello'... " << std::boolalpha << tr.Remove("hello") << '\n';
	std::cout << "removing 'absolute'... " << std::boolalpha << tr.Remove("absolute") << '\n';
	std::cout << "num words in the Trie: " << tr.Size() << " num nodes: " << tr.NumNodes() << '\n';

	std::cout << "match 'he' again:\n";
	PrintWords(tr.Match("he"));

	std::cout << "match all:\n";
	PrintWords(tr.Match(""));

	tr.Insert("barter");
	std::cout << "after inserting \'barter\':\n";
	std::cout << "num words in the Trie: " << tr.Size() << " num nodes: " << tr.NumNodes() << '\n';
	PrintWords(tr.Match(""));
	tr.RemoveAndDeleteNodes("barter");
	std::cout << "num words in the Trie: " << tr.Size() << " num nodes: " << tr.NumNodes() << '\n';
}