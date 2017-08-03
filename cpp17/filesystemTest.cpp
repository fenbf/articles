// filesystem_path_example.cpp  
// compile by using: /EHsc  
#include <string>  
#include <iostream>  
#include <sstream>  
#include <filesystem>  

using namespace std;
namespace fs = std::experimental::filesystem;

// example adapted from https://docs.microsoft.com/pl-pl/cpp/standard-library/file-system-navigation
string DisplayPathInfo(const fs::path& pathToShow)
{
	ostringstream os;
	
	int i = 0;	
	os << "Displaying path info for: " << pathToShow << "\n";
	for (const auto& part : pathToShow)
	{
		os << "path part: " << i++ << " = " << part << "\n";
	}

	os	<< "exists() = " << fs::exists(pathToShow) << "\n"
		<< "root_name() = " << pathToShow.root_name() << "\n"
		<< "root_path() = " << pathToShow.root_path() << "\n"
		<< "relative_path() = " << pathToShow.relative_path() << "\n"
		<< "parent_path() = " << pathToShow.parent_path() << "\n"
		<< "filename() = " << pathToShow.filename() << "\n"
		<< "stem() = " << pathToShow.stem() << "\n"
		<< "extension() = " << pathToShow.extension() << "\n";

	try
	{
		os << "canonical() = " << fs::canonical(pathToShow) << "\n";
	}
	catch (fs::filesystem_error err)
	{
		cout << "exception: " << err.what() << "\n";
	}

	return os.str();
}

int main(int argc, char* argv[])
{
	const fs::path pathToShow{ argc >= 2 ? argv[1] : fs::current_path() };

	cout << DisplayPathInfo(pathToShow) << endl;
}