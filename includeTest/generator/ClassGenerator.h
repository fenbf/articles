#pragma once

class ClassGenerator
{
private:
	unsigned m_varCount;
public:
	ClassGenerator(unsigned v) : m_varCount(v) { }
	~ClassGenerator() { }

	void writeClassDecl(std::ofstream &sourceFile, const std::string & className);
};

