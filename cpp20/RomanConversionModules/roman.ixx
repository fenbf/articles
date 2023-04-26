module;

#include <string_view>	// in global module fragment

export module roman;

/*
Good morning! Here's your coding interview problem for today.

This problem was asked by Facebook.

Given a number in Roman numeral format, convert it to decimal.

The values of Roman numerals are as follows:

{
	'M': 1000,
	'D': 500,
	'C': 100,
	'L': 50,
	'X': 10,
	'V': 5,
	'I': 1
}

In addition, note that the Roman numeral system uses subtractive notation for numbers such as IV and XL.

For the input XIV, for instance, you should return 14.

4 (IV) and 9 (IX)
40 (XL), 90 (XC), 400 (CD) and 900 (CM)
These are the only subtractive forms in standard use.
*/

export int ConvertFromRoman(std::string_view str) {
	if (str.empty())
		return -1;

	auto dict = [](char ch) {
		if (ch == 'I') return 1;
		else if (ch == 'V') return 5;
		else if (ch == 'X') return 10;
		else if (ch == 'L') return 50;
		else if (ch == 'C') return 100;
		else if (ch == 'D') return 500;
		else if (ch == 'M') return 1000;
		return -1;
	};

	int num = 0;
	int prev = -1;
	int i = 0;
	while (i < str.length())  {
		auto ret = dict(str[i]);
		if (ret == -1)
			return -1;
		
		if (prev == -1) {
			if (ret == 1 || ret == 10 || ret == 100)
				prev = ret;
			else
				num += ret;
		}
		else {
			if (prev == 1 && ret == 5)
			{
				num += 4; prev = -1;
			}
			else if (prev == 1 && ret == 10)
			{
				num += 9; prev = -1;
			}
			else if (prev == 10 && ret == 50)
			{
				num += 40; prev = -1;
			}
			else if (prev == 10 && ret == 100)
			{
				num += 90; prev = -1;
			}
			else if (prev == 100 && ret == 500)
			{
				num += 400; prev = -1;
			}
			else if (prev == 100 && ret == 1000)
			{
				num += 900; prev = -1;
			}
			else {
				num += prev;
				prev = ret;
			}			
		}
		++i;
	}
	if (prev > 0)
		num += prev;

	return num;
}

export int ConvertFromRoman2(std::string_view str) {
	if (str.empty())
		return -1;

	auto dict = [](char ch) {
		if (ch == 'I') return 1;
		else if (ch == 'V') return 5;
		else if (ch == 'X') return 10;
		else if (ch == 'L') return 50;
		else if (ch == 'C') return 100;
		else if (ch == 'D') return 500;
		else if (ch == 'M') return 1000;
		return -1;
	};

	int num = 0;
	for (int i = 0; i < str.length(); ++i) {
		int current = dict(str[i]);
		int next = i + 1 < str.length() ? dict(str[i + 1]) : 0;
		if (current < next) {
			num -= current; // subtractive notation
		}
		else {
			num += current; // additive notation
		}
	}

	return num;
}