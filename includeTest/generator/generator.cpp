#include "stdafx.h"
#include "SourceFile.h"
#include "ClassGenerator.h"


using namespace std;


///////////////////////////////////////////////////////////////////////////////
class Generator
{
private:
	bool m_usePragmaOnce;
	bool m_useIfDef;
	unsigned int m_headerCount;
	unsigned int m_complexCount;
	string m_path;
	
	vector<HeaderFile> m_headers;
	unique_ptr<CppFile> m_mainFile;
	unique_ptr<ClassGenerator> m_classGen;
public:
	Generator(unsigned int hCount, unsigned int cCount, const char *path, bool up, bool ud) 
		: m_headerCount(hCount)
		, m_complexCount(cCount)
		, m_path(path) 
		, m_usePragmaOnce(up)
		, m_useIfDef(ud)
	{ 

	}
	~Generator() { }

	void buildAll();
};

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	vector<string> cmd;

	for (int i = 0; i < argc; ++i)
		cmd.emplace_back(argv[i]);

	if (argc >= 4)
	{
		// by default all false
		bool usePragmaOnce = find(cmd.begin()+3, cmd.end(), "pragmaOnce") != cmd.end();
		bool useIfDef = find(cmd.begin()+3, cmd.end(), "ifDef") != cmd.end();

		Generator gen(atoi(argv[1]), atoi(argv[2]), argv[3], usePragmaOnce, useIfDef);
		gen.buildAll();
	}
	else
	{
		cout << "command:\n    generator.exe X Y path [options]" << endl;
		cout << "X - num of files\nY - complexity (num of members in class)\npath - output path" << endl;
		cout << "options: " << endl;
		cout << "    pragmaOnce - use pragma once at the beginning of each file " << endl;
		cout << "    ifDef      - ifdef before include of other header files " << endl;
	}


	return 0;
}

///////////////////////////////////////////////////////////////////////////////
void Generator::buildAll()
{
	SourceFile::s_useAdditionalIfDef = m_useIfDef;
	HeaderFile::s_usePragmaOnce = m_usePragmaOnce;
	
	// generate header names
	for (unsigned int id = 0; id < m_headerCount; ++id)
	{
		m_headers.emplace_back(HeaderFile("header_" + to_string(id) + ".h"));
	}	

	// set cross includes...
	for (unsigned int id = 0; id < m_headerCount; ++id)
	{
		for (unsigned int j = 0; j < m_headerCount; ++j)
		{
			if (id != j)
				m_headers[id].addInclude(&m_headers[j]);
		}
	}

	// single main file
	m_mainFile = unique_ptr<CppFile>(new CppFile("testHeaders.cpp", "main"));

	// this source file includes all the header files
	for (unsigned int i = 0; i < m_headerCount; ++i)
	{
		m_mainFile->addInclude(&m_headers[i]);
	}

	// class gen
	m_classGen = unique_ptr<ClassGenerator>(new ClassGenerator(m_complexCount));

	// additional <string> include:
	string extraStr = "#include <string>";
	if (m_useIfDef)
	{
		extraStr = "#ifndef STD_STRING_INCLUDED\n#define STD_STRING_INCLUDED\n#include <string>\n#endif\n";
	}

	// generate headers:
	for (unsigned int id = 0; id < m_headerCount; ++id)
	{
		m_headers[id].create(m_path, m_classGen.get(), extraStr);
	}

	// generate main file:
	m_mainFile->create(m_path);
}
