#pragma once

class ClassGenerator;

enum class SourceFileType { tUndefined, tHeader, tCpp };

///////////////////////////////////////////////////////////////////////////////
//
class SourceFile
{
public:	
	static bool s_useAdditionalIfDef;

protected:
	SourceFileType m_type;
	std::string m_fileName;
	std::vector<class HeaderFile *> m_includes;
public:
	SourceFile() : m_type(SourceFileType::tUndefined) { }
	SourceFile(SourceFileType t, const std::string &fileName = "") : m_type(t), m_fileName(fileName) { }
	virtual ~SourceFile() { }

	// creates file and generates its content
	virtual void create(const std::string &pat, ClassGenerator *classGen = nullptr, const std::string &strExtra = "") = 0;
	
	void addInclude(class HeaderFile *h) { m_includes.push_back(h); }

	const std::string &fileName() const { return m_fileName; }
};


///////////////////////////////////////////////////////////////////////////////
class HeaderFile : public SourceFile
{
public:
	static bool s_usePragmaOnce;

private:
	std::string m_defineName;	// define block based on filename
	std::string m_className;	// name of a class inside this header file

public:
	HeaderFile() { }
	HeaderFile(const std::string &fileName);
	~HeaderFile() { }

	//HeaderFile(const HeaderFile& h) : SourceFile(h), m_defineName(h.m_defineName), m_className(h.m_className) { }
	//HeaderFile& operator=(const HeaderFile& element) { return *this; }

	virtual void create(const std::string &path, ClassGenerator *classGen = nullptr, const std::string &strExtra = "") override;

	const std::string &defineName() const { return m_defineName; }
	const std::string &className() const { return m_className; }
};


///////////////////////////////////////////////////////////////////////////////
class CppFile : public SourceFile
{
private:
	std::string m_functionName;	// name of function inside this cpp file

public:
	CppFile() { }
	CppFile(const std::string &fileName, const std::string &funcName) : SourceFile(SourceFileType::tCpp, fileName), m_functionName(funcName) { }
	~CppFile() { }

	virtual void create(const std::string &path, ClassGenerator *classGen = nullptr, const std::string &strExtra = "") override;
};