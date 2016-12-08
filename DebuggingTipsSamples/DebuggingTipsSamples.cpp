#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <windows.h>
#include <string>
#include <vector>
#include <memory>

#define MY_TRACE(msg, ...) MyTraceImpl(__LINE__, __FILE__, msg, __VA_ARGS__)

// implementation us
void MyTraceImpl(int line, const char *fileName, const char *msg, ...)
{
	va_list args;
	char buffer[256] = { 0 };
	sprintf_s(buffer, "%s(%d) : ", fileName, line);
	OutputDebugString(buffer);

	// retrieve the variable arguments
	va_start(args, msg);
	vsprintf_s(buffer, msg, args);
	OutputDebugString(buffer);
	va_end(args);
}

class SimpleParam
{
public:
	SimpleParam(const std::string& str, int val) : mStr(str), mVal(val) 
	{ 
		Init();
	}

	void Init() { }
	std::string GetStrVal() const { return mStr + std::to_string(mVal); }

private:
	std::string mStr;
	int mVal;
};

void CallBackFunction(SimpleParam p1, SimpleParam p2)
{
	std::cout << p1.GetStrVal() << "\n";
	std::cout << p2.GetStrVal() << "\n";
}

void MyFunc(const std::string &one, const std::string &two)
{
	auto res = one + two;
	std::cout << res << "\n";
}

void MethodToFix()
{
	static bool bEnableFix = true;

	if (bEnableFix)
	{
		std::cout << "fixed...\n";
	}
	else
	{
		std::cout << "Old bug...\n";
	}
}

class VertexBase
{
public:
	void AddVertex(std::shared_ptr<VertexBase> pVtx) { m_vecNeighbours.push_back(pVtx); }
	virtual bool IsMapVertex() const { return false; }

protected:
	std::vector<std::shared_ptr<VertexBase>> m_vecNeighbours;
	int m_flags{ 0 };
	double m_weight{ 1.0 };
};

class MapVertex : public VertexBase
{
public:
	void SetName(const std::string& name) { m_name = name; }
	bool IsMapVertex() const override { return true; }
	std::string GetName() const { return m_name; }
protected:
	double m_range{ 0.0 };
	std::string m_name;
};

class MySpecialVertex : public MapVertex
{
public:
	MySpecialVertex() { }
	MySpecialVertex(const std::string& name) { SetName(name); }

#ifdef _DEBUG
	void UpdateSurroundingNames() {
		m_vecSurroundingNames.clear();
		for (const auto& v : m_vecNeighbours) {
			if (v->IsMapVertex())
				m_vecSurroundingNames.push_back(static_cast<const MapVertex *>(v.get())->GetName());
		}
	}
#endif

private:
	int m_val{ 0 };
	int m_size{ 0 };
#ifdef _DEBUG
	std::vector<std::string> m_vecSurroundingNames;
#endif
};

void ScanRectangle(RECT& r)
{
	if (r.right - r.left > 100)
	{
		r.left = 100;
		r.right = 90;
	}
}

void ScanForInvalidRects(const std::vector<RECT>& rects)
{
	int i = 0;
	for (const auto &r : rects)
	{
		if (r.right < r.left || r.top < r.bottom)
			MY_TRACE("invalid rectangle %d! \t -  l:%d t:%d r:%d b:%d\n", i, r.left, r.top, r.right, r.bottom);
		++i;
	}
}

bool CheckRect(const RECT& r)
{
	return r.right - r.left > 100;
}

void DebugRectLoop()
{
	std::vector<RECT> rects(100);
	for (auto &r : rects)
	{
		r.top = 100;
		r.bottom = 0;
		r.left = 10;
		r.right = 90;
	}
	// make a few larger
	// left, top, right, bottom
	rects[7] = { 10, 100, 200, 0 }; 
	rects[17] = { 10, 100, 200, 0 }; 
	rects[37] = { 10, 100, 200, 0 }; 
	
	ScanForInvalidRects(rects);

	for (auto &r : rects)
	{
		ScanRectangle(r);
	}

	ScanForInvalidRects(rects);
}

int main()
{
	OutputDebugString("DebuggingTipsSamples.cpp(32) : super\n");
	MY_TRACE("Hello World %d\n", 5);
	MyFunc("Hello ", "World");
	int a = 0; 



	// run method 5 times, try to change `bEnableFix` inside while debugging 
	for (int i = 0; i < 5; ++i)
		MethodToFix();

	CallBackFunction({ "Hello", 1 }, { "World", 2 });

	auto myVertex = std::make_shared<MySpecialVertex>("Cracow");
	myVertex->AddVertex(std::make_shared<MySpecialVertex>("London"));
	myVertex->AddVertex(std::make_shared<MySpecialVertex>("Berlin"));
	myVertex->AddVertex(std::make_shared<MySpecialVertex>("Paric"));
	myVertex->AddVertex(std::make_shared<MySpecialVertex>("Warsaw"));

#ifdef _DEBUG
	myVertex->UpdateSurroundingNames();
	myVertex->AddVertex(std::make_shared<MySpecialVertex>("New York"));
#endif // _DEBUG

	DebugRectLoop();
}
