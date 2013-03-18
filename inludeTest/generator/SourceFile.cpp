#include "stdafx.h"
#include "SourceFile.h"
#include "ClassGenerator.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////

static void writeIncludeBlock(ofstream &sourceFile, vector<HeaderFile *> &includes,  bool useIfDef)
{
	if (includes.size() == 0)
		return;

	sourceFile << "\n// includes:" << endl;
	
	for (auto header : includes)
	{
		if (useIfDef) 
			sourceFile << "#ifndef " << header->defineName() << endl;
		    
		sourceFile << "    #include \"" << header->fileName() << "\"" << endl;
		
		if (useIfDef)
			sourceFile << "#endif " << endl;
	}
	sourceFile << "// includes end\n" << endl;
}

static string getDefineName(string fileName)
{
	size_t pos = fileName.find_last_of('.');
	fileName.replace(pos, 1, 1, '_');
	transform(fileName.begin(), fileName.end(), fileName.begin(), ::toupper);
	return fileName;
}

static string getClassName(string fileName)
{
	return string(fileName, 0, fileName.find_last_of('.'));
}

///////////////////////////////////////////////////////////////////////////////

bool SourceFile::s_useAdditionalIfDef = false;

bool HeaderFile::s_usePragmaOnce = false;

HeaderFile::HeaderFile(const std::string &fileName) 
	: SourceFile(SourceFileType::tHeader, fileName)
	, m_defineName("_INCLUDED_HEADER_" + getDefineName(fileName))
	, m_className(getClassName(fileName) + "_Class")
{ 

}

void HeaderFile::create(const string &path, ClassGenerator *classGen, const std::string &strExtra)
{
	ofstream headerFile(path + m_fileName, ios_base::trunc);

	if (!headerFile)
	{
		cout << "cannot create header: " << m_fileName << endl;
		return;
	}

	if (s_usePragmaOnce)
	{
		headerFile << "#pragma once" << endl;
	}

	headerFile << "#ifndef " << m_defineName << endl;
	headerFile << "    #define " << m_defineName << endl;
	headerFile << "\n//" << m_fileName << endl;

	if (strExtra != "")
		headerFile << endl << "// extra " << endl << strExtra << endl << endl;

	writeIncludeBlock(headerFile, m_includes, s_useAdditionalIfDef);

	assert(classGen);
	classGen->writeClassDecl(headerFile, m_className);

	headerFile << "\n#endif " << m_defineName << endl;
	
	headerFile.close();
}

///////////////////////////////////////////////////////////////////////////////

void CppFile::create(const string &path, ClassGenerator *, const std::string &strExtra)
{
	ofstream sourceFile(path + m_fileName, ios_base::trunc);

	if (!sourceFile)
	{
		cout << "cannot create: " << m_fileName << endl;
		return;
	}

	sourceFile << "// " << m_fileName << endl;

	if (strExtra != "")
		sourceFile << endl << "// extra " << endl << strExtra << endl << endl;

	writeIncludeBlock(sourceFile, m_includes, s_useAdditionalIfDef);

	sourceFile << "\nint " << m_functionName << "()" << endl;
	sourceFile << "{" << endl;

	// declare objects:
	for (auto header : m_includes)
	{
		sourceFile << "    " << header->className() << " object_" << header->className() << ";" << endl;
	}

	sourceFile << "\n    return 0;" << endl;
	sourceFile << "}" << endl;

	sourceFile.close();
}