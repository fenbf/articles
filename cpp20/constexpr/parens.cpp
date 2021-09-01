#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <format>

enum class ParenCategory { None, Round, Curly, Square };
constexpr ParenCategory getParenCategory(char ch) {
	if (ch == '(' || ch == ')')
		return ParenCategory::Round;
	if (ch == '{' || ch == '}')
		return ParenCategory::Curly;
	if (ch == '[' || ch == ']')
		return ParenCategory::Square;

	return ParenCategory::None;
}

constexpr bool isOpeningParen(char ch) {
    return ch == '(' || ch == '{' || ch == '[';
}

constexpr bool isBalanced(std::string_view expr) {
    std::vector<ParenCategory> parens;
    for (const auto& ch : expr) {
        const auto cat = getParenCategory(ch);
        if (isOpeningParen(ch))
            parens.push_back(cat);
        else {
            if (parens.empty())
                return false;

            const auto lastCat = parens.back();
            if (lastCat != cat)
                return false;
            parens.pop_back();
        }
    }

    return parens.empty();
}

int main() {
    std::cout << std::format("isBalanced(\"()\"): {} \n", isBalanced("()"));
    std::cout << std::format("isBalanced(\"{{[[[()]]]}}\"): {} \n", isBalanced("{[[[()]]]}"));
    std::cout << std::format("isBalanced(\"{{[[[()]]]}}}}}}}}}}}}\"): {} \n", isBalanced("{[[[()]]]}}}}}}"));
    std::cout << std::format("isBalanced(\"([)]\"): {} \n", isBalanced("([)]"));
    std::cout << std::format("isBalanced(\"((()\"): {} \n", isBalanced("((()"));
    std::cout << std::format("isBalanced(\"((()[][][]]]]]]]]]]]]\"): {} \n", isBalanced("((()[][][]]]]]]]]]]]]"));
}
