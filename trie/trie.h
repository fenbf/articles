#pragma once

#include <map>
#include <vector>
#include <string>
#include <string_view>

class Trie {

	struct TrieNode {
		std::map<char, TrieNode> children;
		bool isWord{ false };
	};

public:

	Trie() = default;
	Trie(std::initializer_list<std::string_view> words);

	size_t Size() const { return size; }
	size_t NumNodes() const { return numNodes; }

	void Insert(std::string_view word);

	bool Find(std::string_view word) const;

	bool Remove(std::string_view word);

	bool RemoveAndDeleteNodes(std::string_view word);

	std::vector<std::string> Match(std::string_view prefix) const;

private:
	TrieNode root;
	size_t size{ 0 };
	size_t numNodes{ 0 }; // excluding root
};

