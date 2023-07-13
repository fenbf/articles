#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <WinInet.h>
#include <vector>
#include <chrono>

#pragma comment(lib, "wininet.lib")

std::string downloadHolidays(const std::string& url) {
    std::string result;
    HINTERNET hInternet = InternetOpen("MyApp", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hInternet) {
        HINTERNET hUrlFile = InternetOpenUrl(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (hUrlFile) {
            char buffer[4096]{ 0 };
            DWORD bytesRead = 0;

            while (InternetReadFile(hUrlFile, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
                result.append(buffer, bytesRead);
            }

            InternetCloseHandle(hUrlFile);
        }
        else {
            std::cerr << "Error: Unable to open URL" << std::endl;
        }
        InternetCloseHandle(hInternet);
    }
    else {
        std::cerr << "Error: Unable to initialize WinInet" << std::endl;
    }
    return result;
}

void extractAndSaveHolidays(const std::string& json, const std::string& outputFilename) {
    std::ofstream outFile(outputFilename);
    size_t pos = 0;
    std::string dateKey = "\"date\":\"";
    std::string localNameKey = "\"localName\":\"";
    std::string nameKey = "\"name\":\"";
    std::string name, localName, date;

    while ((pos = json.find(dateKey, pos)) != std::string::npos) {
        pos += dateKey.length();
        size_t dateEnd = json.find('\"', pos);
        if (dateEnd == std::string::npos) {
            break;
        }
        date = json.substr(pos, dateEnd - pos);

        size_t localNamePos = json.find(localNameKey, dateEnd);
        if (localNamePos == std::string::npos) {
            break;
        }
        localNamePos += localNameKey.length();
        size_t localNameEnd = json.find('\"', localNamePos);
        if (localNameEnd == std::string::npos) {
            break;
        }
        localName = json.substr(localNamePos, localNameEnd - localNamePos);

        size_t namePos = json.find(nameKey, localNameEnd);
        if (namePos == std::string::npos) {
            break;
        }
        namePos += nameKey.length();
        size_t nameEnd = json.find('\"', namePos);
        if (nameEnd == std::string::npos) {
            break;
        }
        name = json.substr(namePos, nameEnd - namePos);

        outFile << name << " (" << localName << "): " << date << std::endl;
        pos = nameEnd;
    }

    outFile.close();
}

std::vector<std::chrono::year_month_day> extractHolidaysToVector(const std::string& json) {
    std::vector<std::chrono::year_month_day> holidays;
    size_t pos = 0;
    const std::string dateKey = "\"date\":\"";
    std::string date;

    while ((pos = json.find(dateKey, pos)) != std::string::npos) {
        pos += dateKey.length();
        size_t dateEnd = json.find('\"', pos);
        if (dateEnd == std::string::npos)
            break;
        date = json.substr(pos, dateEnd - pos);

        std::istringstream dateStream(date);
        std::chrono::sys_days parsedDate;
        dateStream >> std::chrono::parse("%F", parsedDate);

        std::chrono::weekday wd {parsedDate};
        if (wd != std::chrono::Saturday && wd != std::chrono::Sunday) {
            std::cout << "adding " << parsedDate << '\n';
            holidays.push_back(parsedDate);
        }
        else
            std::cout << "skipping " << parsedDate << " as it's weekend...\n";

        pos = dateEnd;
    }

    return holidays;
}

std::chrono::year_month_day next_business_date(std::chrono::year_month_day d) {
    std::chrono::month orig_mth{ d.month() };

    d = std::chrono::sys_days(d) + std::chrono::days(1);

    std::chrono::weekday wd{ std::chrono::sys_days(d) };

    if (wd == std::chrono::Saturday) d = std::chrono::sys_days(d) + std::chrono::days(2);
    if (wd == std::chrono::Sunday) d = std::chrono::sys_days(d) + std::chrono::days(1);

    // it's Monday now, but we have to check if we're in the next month...
    if (orig_mth < d.month() || (orig_mth == std::chrono::December && d.month() == std::chrono::January))
        d = std::chrono::sys_days(d) - std::chrono::days(3); // back to Friday

    return d;
}

std::chrono::year_month_day next_business_date(std::chrono::year_month_day d, const std::vector<std::chrono::year_month_day>& holidays) {
    std::chrono::year_month orig_month{ d.year(), d.month() };
    auto dCopy = d;

    do {
        d = std::chrono::sys_days(d) + std::chrono::days(1);
        auto wd = std::chrono::weekday{ std::chrono::sys_days(d) };
        if (wd == std::chrono::Saturday) d = std::chrono::sys_days(d) + std::chrono::days(2);
        else if (wd == std::chrono::Sunday) d = std::chrono::sys_days(d) + std::chrono::days(1);
    } while (std::ranges::find(holidays, d) != holidays.end());

    // it's next day now, but we have to check if we're in the next month, in that case roll to a prev business day
    if (orig_month != std::chrono::year_month{ d.year(), d.month() }) {
        d = dCopy;
        do {
            d = std::chrono::sys_days(d) - std::chrono::days(1);
            auto wd = std::chrono::weekday{ std::chrono::sys_days(d) };
            if (wd == std::chrono::Sunday) d = std::chrono::sys_days(d) - std::chrono::days(2);
            else if (wd == std::chrono::Saturday) d = std::chrono::sys_days(d) - std::chrono::days(1);
        } while (std::ranges::find(holidays, d) != holidays.end());
    }

    return d;
}

void test_next_business_date(
    const std::chrono::year_month_day& input_date,
    const std::vector<std::chrono::year_month_day>& holidays,
    const std::chrono::year_month_day& expected_output,
    const std::string& test_name
) {
    auto result = next_business_date(input_date, holidays);
    if (result == expected_output) {
        std::cout << test_name << ": PASSED" << std::endl;
    }
    else {
        std::cout << test_name << ": FAILED" << std::endl;
        std::cout << "Expected: " << expected_output << ", Got: " << result << std::endl;
    }
}

int main() {
    std::string country = "PL";
    std::string year = "2023";
    std::string apiUrl = "https://date.nager.at/api/v3/PublicHolidays/" + year + "/" + country;

    std::string jsonResponse = downloadHolidays(apiUrl);
    if (!jsonResponse.empty()) {
        extractAndSaveHolidays(jsonResponse, "polish_holidays.txt");

        auto vec = extractHolidaysToVector(jsonResponse);
        for (auto& date : vec)
            std::cout << date << '\n';

        std::cout << "Holidays saved to polish_holidays.txt" << std::endl;
    }

    // tests for date rolling:
    // create a vector of holidays for 2023 hardcoded
    std::vector<std::chrono::year_month_day> holidays;
    // populate holidays with some values

    holidays.push_back(std::chrono::year{ 2023 } / 1 / 6); // Friday, Epiphany 
    holidays.push_back(std::chrono::year{ 2023 } / 4 / 10);    // Easter Monday

    auto t = next_business_date(std::chrono::year{ 2023 } / 1 / 5, holidays) == std::chrono::year{ 2023 } / 1 / 9;
    std::cout << t << '\n';
    t = next_business_date(std::chrono::year{ 2023 } / 4 / 7, holidays) == std::chrono::year{ 2023 } / 4 / 11;
    std::cout << t << '\n';

    // artificial test:
    holidays.push_back(std::chrono::year{ 2023 } / 4 / 26); // Wed make a holiday 
    holidays.push_back(std::chrono::year{ 2023 } / 4 / 28); // Friday make a holiday 

    // roll to a prev day
    t = next_business_date(std::chrono::year{ 2023 } / 4 / 27, holidays) == std::chrono::year{ 2023 } / 4 / 25;
    std::cout << t << '\n';

    using namespace std::chrono_literals;

    std::vector<std::chrono::year_month_day> holidays2 = {
        2023y / 4 / 14, // Good Friday
        2023y / 4 / 17, // Easter Monday
        2023y / 5 / 1,  // Labor Day
    };

    test_next_business_date(2023y / 4 / 13, holidays2, 2023y / 4 / 18, "Test 1: Skipping Good Friday and Easter Monday");
    test_next_business_date(2023y / 4 / 30, holidays2, 2023y / 4 / 28, "Test 2: Rolling back to April");
    test_next_business_date(2023y / 4 / 19, holidays2, 2023y / 4 / 20, "Test 3: No holidays or weekends");
    test_next_business_date(2023y / 4 / 28, holidays2, 2023y / 4 / 27, "Test 4: Input date is a Friday");


    return 0;
}
