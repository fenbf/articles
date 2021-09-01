#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <ranges>
#include <variant>
#include <iostream>

constexpr std::vector<std::string_view>
splitSV(std::string_view strv, std::string_view delims = " ") {
    std::vector<std::string_view> output;
    size_t first = 0;

    while (first < strv.size()) {
        const auto second = strv.find_first_of(delims, first);

        if (first != second)
            output.emplace_back(strv.substr(first, second - first));

        if (second == std::string_view::npos)
            break;

        first = second + 1;
    }

    return output;
}

constexpr size_t MaxStrLen = 24;
using ArrayString = std::array<char, MaxStrLen>;
constexpr const char* WhiteSpaceChars = " \t\n\r\f\v";
constexpr const char* NumericChars = "1234567890";

std::ostream& operator<<(std::ostream& os, ArrayString s) {
    for (size_t i = 0; i < MaxStrLen && s[i] != 0; ++i)
        os << s[i];
    return os;
}

struct Param {
    ArrayString name_{ 0 };
    ArrayString strVal_{ 0 };
    int intVal_{ INT_MAX };
    // waiting for constexpr std::variant here...

    constexpr bool hasString() const noexcept { return strVal_[0] != 0; }
    constexpr bool hasInt() const noexcept { return intVal_ != INT_MAX; }
};

constexpr ArrayString copyFrom(std::string_view str) {
    ArrayString out{ 0 };
    const auto start = str.find_first_not_of(WhiteSpaceChars);
    // std::copy_n had some issues at compile time...
    for (size_t i = 0; i + start < str.size() && i < MaxStrLen; ++i)
        out[i] = str[start + i];
    return out;
}

constexpr bool operator==(const ArrayString& a, std::string_view str) {
    if (str.size() >= MaxStrLen)
        return false;

    for (size_t i = 0; i < str.size(); ++i)
        if (str[i] != a[i])
            return false;

    return true;
}

constexpr bool allDigits(std::string_view str) {
    const auto start = str.find_first_not_of(WhiteSpaceChars);
    return str.find_first_not_of(NumericChars, start) != std::string_view::npos;
}

constexpr int parseInt(std::string_view str) {
    const auto start = str.find_first_not_of(WhiteSpaceChars);
    int dec = 1;
    int val = 0;
    for (char ch : str | std::views::drop(start) | std::views::reverse) {
        val += (static_cast<int>(ch) - '0') * dec;
        dec *= 10;
    }
    return val;
}

constexpr Param parsePair(std::string_view str) {
    Param out;

    const auto words = splitSV(str, ":");

    if (words.size() == 2)
    {
        out.name_ = copyFrom(words[0]);
        if (allDigits(words[1]))
            out.strVal_ = copyFrom(words[1]);
        else
            out.intVal_ = parseInt(words[1]);
    }

    return out;
}

constexpr size_t numPairs(std::string_view str, std::string_view delims = ",") {
    return 1 + std::count_if(begin(str), end(str), [&delims](const auto& ch) {
            return delims.find(ch) != std::string_view::npos;
        }
    );
}

template <size_t N>
constexpr std::array<Param, N> parseParams(std::string_view str) {   
    const auto params = splitSV(str, ",");
    std::array<Param, N> out;
    for (size_t i = 0; auto & singleParamStr : params) {
        out[i++] = parsePair(singleParamStr);
    }

    return out;
}

int main() {
    //static_assert(numWords(str, ",") == 2);
    //static_assert(longestWordSize("hello world abc") == 5);
    //static_assert(std::ranges::equal(word, "PROGRAMMING"));
    {
        constexpr auto p = parsePair("a: hello");
        std::cout << p.name_;
        std::cout << '\n';
        std::cout << p.strVal_;
        static_assert(p.name_ == "a");
        static_assert(p.strVal_ == "hello");
    }
    {
        auto p = parsePair("param: 1976");
        std::cout << p.name_;
        std::cout << '\n';
        std::cout << p.intVal_;
        std::cout << p.strVal_;
        //static_assert(p.name_ == "param");
        //static_assert(p.intVal_ == 1976);
    }
    static_assert(parseInt(" 123") == 123);
    static_assert(parseInt("   1976") == 1976);

    static_assert(numPairs("a:100, b:hello, longerName:1975") == 3);
    constexpr auto str = "a:1, b:2, c:text";
    constexpr auto len = numPairs(str);
    constexpr std::array<Param, len> params = parseParams<len>(str);
    static_assert(len == 3);
    static_assert(params[0].name_ == "a");
    static_assert(params[1].name_ == "b");
    static_assert(params[2].name_ == "c");
}