#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <string>
#include <sstream>

#define MY_TRACE(msg, ...) MyTraceImpl(__LINE__, __FILE__, msg, __VA_ARGS__)

void MyTraceImpl(int line, const char* fileName, const char* msg, ...)
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

#define MY_TRACE_TMP(...) MyTraceImplTmp(__LINE__, __FILE__, __VA_ARGS__)

template <typename ...Args>
void MyTraceImplTmp(int line, const char* fileName, Args&& ...args)
{
	std::ostringstream stream;
	stream << fileName << "(" << line << ") : ";
	(stream << ... << std::forward<Args>(args)) << '\n';

	OutputDebugString(stream.str().c_str());
}

int main()
{
	// this will show on the output window, not console...
	MY_TRACE("hello world! %d, %d\n", 10, 42);
	MY_TRACE_TMP("hello world! ", 10, ", ", 42);
}
