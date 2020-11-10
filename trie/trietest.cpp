#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector> 

class Trie {

	struct TrieNode {
		std::map<char, TrieNode> children;
		bool isWord{ false };
	};

public:
	void Insert(std::string_view word) {
		if (word.empty())
			return;

		TrieNode* pNode = &_root;
		for (const auto& ch : word) {
			pNode = &(pNode->children[ch]);
		}
		pNode->isWord = true;
	}

	bool Find(std::string_view word) const {
		auto pNode = FindNode(word, &_root);
		return pNode ? pNode->isWord : false;
	}

	bool Remove(std::string_view word) {
		auto pNode = FindNode(word, &_root);
		if (pNode) {
			pNode->isWord = false;
			return true;
		}
		return false;
	}

	std::vector<std::string> Match(std::string_view prefix) const {
		std::vector<std::string> out;

		auto pNode = FindNode(prefix, const_cast<TrieNode*>(&_root));
		if (!pNode)
			return out;

		// collect words from this subtree:
		std::stack<decltype(pNode)> stack;
		std::string word{ prefix.data(), prefix.size() };
		std::stack<std::string> wordStack;
		for (;;) {
			if (pNode->isWord)
				out.push_back(word);

			for (auto& child : pNode->children)
			{
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

private:
	// template to deduce the constness, good idea according to 
	// https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#es50-dont-cast-away-const
	template <typename T>
	static T* FindNode(std::string_view word, T* pRoot) {
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

private:
	TrieNode _root;
};

int main() {
	Trie tr;
	tr.Insert("hello");
	tr.Insert("head");
	tr.Insert("heap");

	auto vec = tr.Match("he");
	for (auto& word : vec)
		std::cout << word << '\n';

	tr.Remove("hello");

	vec = tr.Match("he");
	for (auto& word : vec)
		std::cout << word << '\n';
}