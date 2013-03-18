#include "stdafx.h"
#include "ClassGenerator.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
static const int TYPES_COUNT = 4;
static const string gTypes[TYPES_COUNT] = { "int", "double", "float", "std::string" };
static const string gDefaults[TYPES_COUNT] = { "0", "0.0", "0.0f", "\"Hello World\"" };

static string VarName(unsigned int i)
{
	return "m_variable_" + to_string(i);
}

static string GetVariableDecl(unsigned int i)
{	
	return gTypes[i%TYPES_COUNT] + " " + VarName(i) + ";";
}

static string GetVariableDefaultInit(unsigned int i)
{
	return string(i == 0 ? " :  " : " ,  ") + VarName(i) + "( " + gDefaults[i%TYPES_COUNT] + " )";
}

static string GetVariableSetter(unsigned int i)
{	
	return string("void") + " set" + VarName(i) + "(" + gTypes[i%TYPES_COUNT] + " v) { " + VarName(i) + " = v; }";
}

static string GetVariableGetter(unsigned int i)
{	
	return gTypes[i%TYPES_COUNT]  + " get" + VarName(i) + "() const { return " + VarName(i) + "; }";
}

///////////////////////////////////////////////////////////////////////////////
void ClassGenerator::writeClassDecl(ofstream &headerFile, const string &className)
{
	headerFile << "\n// class: " << endl;

	headerFile << "class " << className << endl;
	headerFile << "{" << endl;
	
	headerFile << "private:" << endl;
	for (unsigned int i = 0; i < m_varCount; ++i)
	{
		headerFile << "    " << GetVariableDecl(i) << endl;
	}
	headerFile << "\npublic:" << endl;

	headerFile << "\n//default constructor" << endl;
	headerFile << "    " << className << "()" << endl;
	for (unsigned int i = 0; i < m_varCount; ++i)
	{
		headerFile << "    " << GetVariableDefaultInit(i) << endl;
	}
	headerFile << "    {\n    }" << endl;


	headerFile << "\n//getters and setters" << endl;
	for (unsigned int i = 0; i < m_varCount; ++i)
	{
		headerFile << "    " << GetVariableSetter(i) << endl << endl;
		headerFile << "    " << GetVariableGetter(i) << endl << endl;
	}

	headerFile << "};" << endl;
	headerFile << "\n// class end " << endl;
}
