// input:
// num_of_header_files - N
// complexity, length - LN
//
// output:
// creates N header files with around LN lines
// n file includes header0.h up to header(n-1).h
// one cpp file that inludes all created previously headers

// one header:
// - include guard
// - parent headers include
// - declaration of class HeaderX
// - LN different member variables
// - LN - setters and getters
// - one constructor

#include "stdafx.h"

///////////////////////////////////////////////////////////////////////////////
class Generator
{
	struct Header
	{
		std::string m_fileName;	// without path
		std::string m_defineName;
		std::string m_className;
	};

private:
	bool m_usePragmaOnce;
	bool m_useIfDef;
	bool m_useAll;
	unsigned int m_headerCount;
	unsigned int m_complexCount;
	std::string m_path;
	std::vector<Header> m_headers;
public:
	Generator(unsigned int hCount, unsigned int cCount, const char *path, bool up, bool ud, bool ua) 
		: m_headerCount(hCount)
		, m_complexCount(cCount)
		, m_path(path) 
		, m_headers(hCount)
		, m_usePragmaOnce(up)
		, m_useIfDef(ud)
		, m_useAll(ua)
	{ 

	}
	~Generator() { }

	void buildAll();

private:
	void buildHeader(unsigned int id);
	void buildHeaderBlock(unsigned int id, std::ofstream &headerFile);
	void buildClassBlock(unsigned int id, std::ofstream &headerFile);
	void buildCppFile();
};

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	std::vector<std::string> cmd(argc);

	for (int i = 0; i < argc; ++i)
		cmd.push_back(argv[i]);

	if (argc >= 4)
	{
		// by default all false
		bool usePragmaOnce = std::find(cmd.begin()+3, cmd.end(), "pragmaOnce") != cmd.end();
		bool useIfDef = std::find(cmd.begin()+3, cmd.end(), "ifDef") != cmd.end();
		bool useAll = std::find(cmd.begin()+3, cmd.end(), "all") != cmd.end();

		Generator gen(atoi(argv[1]), atoi(argv[2]), argv[3], usePragmaOnce, useIfDef, useAll);
		gen.buildAll();
	}
	else
	{
		std::cout << "command:\n    generator.exe X Y path [options]" << std::endl;
		std::cout << "X - num of files\nY - complexity (num of members in class)\npath - output path" << std::endl;
		std::cout << "options: " << std::endl;
		std::cout << "    pragmaOnce - use pragma once at the beginning of each file " << std::endl;
		std::cout << "    ifDef      - ifdef before include of other header files " << std::endl;
		std::cout << "    all        - each file includes all other header " << std::endl;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
void Generator::buildAll()
{
	// generate header names
	for (unsigned int id = 0; id < m_headerCount; ++id)
	{
		m_headers[id].m_fileName = "header_" + std::to_string(id) + ".h";
		m_headers[id].m_defineName = std::string("INCLUDED_") + "HEADER_" + std::to_string(id);
		m_headers[id].m_className = "CHeaderClassNumber" + std::to_string(id);
	}	
	for (unsigned int id = 0; id < m_headerCount; ++id)
	{
		buildHeader(id);
	}

	buildCppFile();
}

///////////////////////////////////////////////////////////////////////////////
void Generator::buildHeader(unsigned id)
{	
	std::ofstream headerFile(m_path + m_headers[id].m_fileName, std::ios_base::trunc);

	if (!headerFile)
	{
		std::cout << "cannot create: " << m_headers[id].m_fileName << std::endl;
		return;
	}

	//
	// write to file
	//
	if (m_usePragmaOnce)
	{
		headerFile << "#pragma once" << std::endl;
	}

	headerFile << "#ifndef " << m_headers[id].m_defineName << std::endl;
	headerFile << "#define " << m_headers[id].m_defineName << std::endl;
	headerFile << "\n//" << m_headers[id].m_defineName << std::endl;

	buildHeaderBlock(id, headerFile);
	buildClassBlock(id, headerFile);

	headerFile << "\n#endif " << m_headers[id].m_defineName << std::endl;
	
	headerFile.close();
}

///////////////////////////////////////////////////////////////////////////////
void Generator::buildHeaderBlock(unsigned int id, std::ofstream &headerFile)
{
	headerFile << "\n// includes:" << std::endl;
	headerFile << "\n#include <string>" << std::endl;
	
	unsigned int count = m_useAll ? m_headerCount : id;
	
	for (unsigned int i = 0; i < count; ++i)
	{
		if (m_useAll && i == id) 
			continue;
		
		if (m_useIfDef) 
			headerFile << "#ifndef " << m_headers[i].m_defineName << std::endl;
		    
		headerFile << "    #include \"" << m_headers[i].m_fileName << "\"" << std::endl;
		
		if (m_useIfDef)
			headerFile << "#endif " << std::endl;
	}
	headerFile << "// includes end\n" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
static const int TYPES_COUNT = 4;
static const std::string gTypes[TYPES_COUNT] = { "int", "double", "float", "std::string" };
static const std::string gDefaults[TYPES_COUNT] = { "0", "0.0", "0.0f", "\"Hello World\"" };

std::string VarName(unsigned int i)
{
	return "m_variable_" + std::to_string(i);
}

std::string GetVariableDecl(unsigned int i)
{	
	return gTypes[i%TYPES_COUNT] + " " + VarName(i) + ";";
}

std::string GetVariableDefaultInit(unsigned int i)
{
	return std::string(i == 0 ? " :  " : " ,  ") + VarName(i) + "( " + gDefaults[i%TYPES_COUNT] + " )";
}

std::string GetVariableSetter(unsigned int i)
{	
	return std::string("void") + " set" + VarName(i) + "(" + gTypes[i%TYPES_COUNT] + " v) { " + VarName(i) + " = v; }";
}

std::string GetVariableGetter(unsigned int i)
{	
	return gTypes[i%TYPES_COUNT]  + " get" + VarName(i) + "() const { return " + VarName(i) + "; }";
}

///////////////////////////////////////////////////////////////////////////////
void Generator::buildClassBlock(unsigned int id, std::ofstream &headerFile)
{
	headerFile << "\n// class: " << std::endl;

	headerFile << "class " << m_headers[id].m_className << std::endl;
	headerFile << "{" << std::endl;
	
	headerFile << "private:" << std::endl;
	for (unsigned int i = 0; i < m_complexCount; ++i)
	{
		headerFile << "    " << GetVariableDecl(i) << std::endl;
	}
	headerFile << "\npublic:" << std::endl;

	headerFile << "\n//default constructor" << std::endl;
	headerFile << "    " << m_headers[id].m_className << "()" << std::endl;
	for (unsigned int i = 0; i < m_complexCount; ++i)
	{
		headerFile << "    " << GetVariableDefaultInit(i) << std::endl;
	}
	headerFile << "    {\n    }" << std::endl;


	headerFile << "\n//getters and setters" << std::endl;
	for (unsigned int i = 0; i < m_complexCount; ++i)
	{
		headerFile << "    " << GetVariableSetter(i) << std::endl << std::endl;
		headerFile << "    " << GetVariableGetter(i) << std::endl << std::endl;
	}

	headerFile << "};" << std::endl;
	headerFile << "\n// class end " << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
void Generator::buildCppFile()
{
	std::string fileName = "testHeaders.cpp";
	std::ofstream sourceFile(m_path + fileName, std::ios_base::trunc);

	if (!sourceFile)
	{
		std::cout << "cannot create: " << fileName << std::endl;
		return;
	}

	sourceFile << "\n// includes:" << std::endl;
	for (auto header : m_headers)
	{
		sourceFile << "#include \"" << header.m_fileName << "\"" << std::endl;
	}
	sourceFile << "// includes end\n" << std::endl;

	sourceFile << "\nint main()" << std::endl;
	sourceFile << "{" << std::endl;

	for (auto header : m_headers)
	{
		sourceFile << "    " << header.m_className << " object_" << header.m_className << ";" << std::endl;
	}

	sourceFile << "\n    return 0;" << std::endl;
	sourceFile << "}" << std::endl;

	sourceFile.close();
}