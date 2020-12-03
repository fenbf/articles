#include "trie.h"
#include <stack>

namespace {

	// template to deduce the constness, good idea according to 
	// https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es50-dont-cast-away-const
	// this also works with a private type, as we're not using private Names
	template <typename T> requires std::is_same_v<Trie::TrieNode, std::remove_cv_t<T>>
		T* FindNode(std::string_view word, T* pRoot) {
		if (word.empty())
			return nullptr;

		auto pNode = pRoot;
		for (const auto& ch : word) {
			const auto it = pNode->children.find(ch);
			if (it == pNode->children.end())
				return nullptr;

			pNode = &(it->second);
		}

		return pNode;
	}
}

Trie::Trie(std::initializer_list<std::string_view> words) {
	for (auto& w : words)
		Insert(w);
}

void Trie::Insert(std::string_view word) {
	if (word.empty())
		return;

	TrieNode* pNode = &root;
	for (const auto& ch : word) {
		auto [iter, wasInserted] = pNode->children.try_emplace(ch);
		pNode = &iter->second;
		if (wasInserted)
			++numNodes;
	}
	pNode->isWord = true;
	++size;
}

bool Trie::Find(std::string_view word) const {
	auto pNode = FindNode(word, &root);
	return pNode ? pNode->isWord : false;
}

bool Trie::Remove(std::string_view word) {
	auto pNode = FindNode(word, &root);
	if (pNode && pNode->isWord) {
		pNode->isWord = false;
		--size;
		return true;
	}
	return false;
}

bool Trie::RemoveAndDeleteNodes(std::string_view word) {
	if (word.empty())
		return false;

	auto pNode = &root;
	TrieNode* pLowestParentWithSeveralChildren = &root;
	char lastChartoDelete = word[0];
	size_t distanceToParent = 0; // we start from root
	for (const auto& ch : word) {
		const auto it = pNode->children.find(ch);
		if (it == pNode->children.end())
			return false;

		if (pNode->children.size() > 1) {
			pLowestParentWithSeveralChildren = pNode;
			lastChartoDelete = ch;
			distanceToParent = 1;
		}
		else
			++distanceToParent;

		pNode = &(it->second);
	}

	if (pNode && pNode->isWord && pLowestParentWithSeveralChildren) {
		pLowestParentWithSeveralChildren->children.erase(lastChartoDelete);
		--size;
		numNodes -= distanceToParent;
		return true;
	}

	return false;
}

std::vector<std::string> Trie::Match(std::string_view prefix) const {
	std::vector<std::string> out;

	auto pNode = prefix.empty() ? &root : FindNode(prefix, &root);
	if (!pNode)
		return out;

	// collect words from this subtree:
	std::stack<decltype(pNode)> stack;
	std::string word{ prefix.data(), prefix.size() };
	std::stack<std::string> wordStack;
	for (;;) {
		if (pNode->isWord)
			out.push_back(word);

		for (auto& child : pNode->children) {
			wordStack.push(word + child.first);
			stack.push(&(child.second));
		}

		if (stack.empty())
			break;

		pNode = stack.top();
		stack.pop();
		word = wordStack.top();
		wordStack.pop();
	}

	return out;
}
