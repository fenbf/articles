// filesystem_path_example.cpp  
// compile by using: /EHsc  
#include <string>  
#include <iostream>  
#include <sstream>  
#include <filesystem>  
#include "filesystemTest.h"

using namespace std;
namespace fs = std::experimental::filesystem;

// example adapted from https://docs.microsoft.com/pl-pl/cpp/standard-library/file-system-navigation
void DisplayPathInfo(const fs::path& pathToShow)
{
	int i = 0;	
	cout << "Displaying path info for: " << pathToShow << "\n";
	for (const auto& part : pathToShow)
	{
		cout << "path part: " << i++ << " = " << part << "\n";
	}

	cout << "exists() = " << fs::exists(pathToShow) << "\n"
		<< "root_name() = " << pathToShow.root_name() << "\n"
		<< "root_path() = " << pathToShow.root_path() << "\n"
		<< "relative_path() = " << pathToShow.relative_path() << "\n"
		<< "parent_path() = " << pathToShow.parent_path() << "\n"
		<< "filename() = " << pathToShow.filename() << "\n"
		<< "stem() = " << pathToShow.stem() << "\n"
		<< "extension() = " << pathToShow.extension() << "\n";

	try
	{
		cout << "canonical() = " << fs::canonical(pathToShow) << "\n";
	}
	catch (fs::filesystem_error err)
	{
		cout << "exception: " << err.what() << "\n";
	}
}

std::uintmax_t ComputeFileSize(const fs::path& pathToCheck)
{
	if (fs::exists(pathToCheck) && fs::is_regular_file(pathToCheck))
	{
		auto err = std::error_code{};
		auto filesize = fs::file_size(pathToCheck, err);
		if (filesize != static_cast<uintmax_t>(-1))
			return filesize;
	}

	return static_cast<uintmax_t>(-1);
}

std::uintmax_t ComputeDirectorySize(const fs::path& pathToCheck)
{
	auto size = static_cast<uintmax_t>(-1);
	if (fs::exists(pathToCheck) && fs::is_directory(pathToCheck))
	{
		for (auto const & entry : fs::recursive_directory_iterator(pathToCheck))
		{
			if (fs::is_regular_file(entry.status()) || fs::is_symlink(entry.status()))
			{
				auto err = std::error_code{};
				auto filesize = fs::file_size(entry, err);
				if (filesize != static_cast<uintmax_t>(-1) && err != std::error_code())
					size += filesize;
			}
		}
	}
	return size;
}

void DisplayFileInfo(const std::experimental::filesystem::v1::directory_entry & entry, std::string &lead, std::experimental::filesystem::v1::path &filename)
{
	time_t cftime = chrono::system_clock::to_time_t(fs::last_write_time(entry));
	cout << lead << " " << filename << ", " 
		 << ComputeFileSize(entry) 
		 << ", time: " << std::asctime(std::localtime(&cftime));
}

void DisplayDirectoryTreeImp(const fs::path& pathToShow, int level)
{
	if (fs::exists(pathToShow) && fs::is_directory(pathToShow))
	{
		auto lead = std::string(level * 3, ' ');
		for (const auto& entry : fs::directory_iterator(pathToShow))
		{
			auto filename = entry.path().filename();
			if (fs::is_directory(entry.status()))
			{
				cout << lead << "[+] " << filename << "\n";
				DisplayDirectoryTreeImp(entry, level + 1);
				cout << "\n";
			}
			else if (fs::is_regular_file(entry.status()))
				DisplayFileInfo(entry, lead, filename);
			else
				cout << lead << " [?]" << filename << "\n";
		}
	}
}

// adapted from Modern C++ Programming Cookbook
void DisplayDirectoryTree(const fs::path& pathToShow)
{
	DisplayDirectoryTreeImp(pathToShow, 0);
}


int main(int argc, char* argv[])
{
	const fs::path pathToShow{ argc >= 2 ? argv[1] : fs::current_path() };

	DisplayPathInfo(pathToShow);

	DisplayDirectoryTree(pathToShow);

	const auto dirSize = ComputeDirectorySize(pathToShow);
	if (dirSize != static_cast<uintmax_t>(-1))
		cout << "Directory size: " << dirSize << "\n";

	cout << "path concat/append:\n";
	fs::path p1("C:\\temp");
	p1 /= "user";
	p1 /= "data";
	cout << p1 << "\n";

	fs::path p2("C:\\temp\\");
	p2 += "user";
	p2 += "data";
	cout << p2 << "\n";
}