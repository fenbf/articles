#include <iostream>
#include <format>
#include <chrono>
#include <iomanip>

int main() {
	using namespace std::literals;
	std::chrono::year_month_day world_day = std::chrono::May / 28d / 2021y;
	std::cout << std::format("Hello on {}!\n", world_day);

	std::chrono::year_month_day today = floor<std::chrono::days>(std::chrono::system_clock::now());
	std::cout << std::format("Hello on {}!\n", today) << '\n';
	std::cout << std::format("Hello on {:%F}!\n", std::chrono::system_clock::now());

	auto now = floor<std::chrono::seconds>(std::chrono::system_clock::now());
	std::cout << std::format("The current time is {} UTC\n", now);
	auto localtime = std::chrono::zoned_time<std::chrono::seconds>(std::chrono::current_zone(), now);
	std::cout << std::format("The current time is: {}\n", localtime);

	auto date = std::chrono::year_month_weekday{ floor<std::chrono::days>(now) };
	std::cout << std::format("The current date is: {}, and the year is {}\n", date, date.year());

	std::cout << std::format("Hello {} {} {} {}\n", "world", 1, 2, 3);
	std::cout << std::format("{:-^10}\n{:-<10}\n{:->10}\n", 7, 8, 9);

	{
		using namespace std::chrono;
		year y{ 2021 };
		constexpr year_month_day today = std::chrono::year{ 2021 } / 7 / 7;
		auto inFuture2 = today + months{ 12 };
		auto inFuture3 = today + days{ 12 };
		constexpr year_month_day today2 = 2021y / July / 7;
		static_assert(today == today2);
		auto inFuture = std::chrono::sys_days{ today } + std::chrono::days{ 365 };		
		std::cout << std::format("{}", inFuture);
		year_month_day nextYear = inFuture;
	}
}