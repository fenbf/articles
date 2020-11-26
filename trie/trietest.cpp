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

	Trie() = default;
	Trie(std::initializer_list<std::string_view> words) {
		for (auto& w : words)
			Insert(w);
	}

	size_t Size() const { return size; }
	size_t NumNodes() const { return numNodes; }

	void Insert(std::string_view word) {
		if (word.empty())
			return;

		TrieNode* pNode = &root;
		for (const auto& ch : word) {
			pNode = &(pNode->children[ch]);
			++numNodes;
		}
		pNode->isWord = true;
		++size;
	}

	bool Find(std::string_view word) const {
		auto pNode = FindNode(word, &root);
		return pNode ? pNode->isWord : false;
	}

	bool Remove(std::string_view word) {
		auto pNode = FindNode(word, &root);
		if (pNode && pNode->isWord) {
			pNode->isWord = false;
			--size;
			return true;
		}
		return false;
	}

	bool RemoveAndDeleteNodes(std::string_view word) {
		if (word.empty())
			return false;

		auto pNode = &root;
		TrieNode* pLowestParentWithSeveralChildren = nullptr;
		char lastChartoDelete = 0;
		size_t distanceToParent = 0;
		for (const auto& ch : word) {
			const auto it = pNode->children.find(ch);
			if (it == pNode->children.end())
				return false;

			if (pNode->children.size() > 1)	{ // it must be root at least
				pLowestParentWithSeveralChildren = pNode;
				lastChartoDelete = ch;
				distanceToParent = 1;
			}
			else
				++distanceToParent;

			pNode = &(it->second);
		}

		if (pNode && pNode->isWord && pLowestParentWithSeveralChildren)
		{
			pLowestParentWithSeveralChildren->children.erase(lastChartoDelete);			
			--size;
			numNodes -= distanceToParent;
			return true;
		}

		return false;
	}

	std::vector<std::string> Match(std::string_view prefix) const {
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
	template <typename T> requires std::is_same_v<TrieNode, std::remove_cv_t<T>>
	static T* FindNode(std::string_view word, T* pRoot)  {
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
	TrieNode root;
	size_t size{ 0 };
	size_t numNodes{ 0 }; // excluding root
};

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