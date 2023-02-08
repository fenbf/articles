import roman;
import checker;

int main() {
	Tester tester;

	// Basic conversion:
	tester.check(1, ConvertFromRoman("I"));
	tester.check(5, ConvertFromRoman("V"));
	tester.check(10, ConvertFromRoman("X"));
	tester.check(50, ConvertFromRoman("L"));
	tester.check(100, ConvertFromRoman("C"));
	tester.check(500, ConvertFromRoman("D"));
	tester.check(1000, ConvertFromRoman("M"));

	// Additive notation:
	tester.check(3, ConvertFromRoman("III"));
	tester.check(20, ConvertFromRoman("XX"));
	tester.check(200, ConvertFromRoman("CC"));

	// Subtractive notation:
	tester.check(4, ConvertFromRoman("IV"));
	tester.check(9, ConvertFromRoman("IX"));
	tester.check(90, ConvertFromRoman("XC"));
	tester.check(900, ConvertFromRoman("CM"));

	// Complex cases:
	tester.check(1994, ConvertFromRoman("MCMXCIV"));
	tester.check(3994, ConvertFromRoman("MMMCMXCIV"));
	tester.check(2421, ConvertFromRoman("MMCDXXI"));	// extra from wiki
	tester.check(2023, ConvertFromRoman("MMXXIII"));	// extra from wiki
	tester.check(1066, ConvertFromRoman("MLXVI"));		// extra from wiki

	// Invalid input:
	tester.check(-1, ConvertFromRoman("ABC"));
	tester.check(-1, ConvertFromRoman(""));

	tester.printStats();

	return 0;
}
