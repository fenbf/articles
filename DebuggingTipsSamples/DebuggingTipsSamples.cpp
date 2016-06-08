#define _CRT_SECURE_NO_WARNINGS // to make life a bit easier...

#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <windows.h>
#include <string>

#define MY_TRACE(msg, ...) MyTraceImpl(__LINE__, __FILE__, msg, __VA_ARGS__)

// implementation us
void MyTraceImpl(int line, const char *fileName, const char *msg, ...)
{
	va_list args;
	char buffer[128];
	sprintf(buffer, "%s(%d) : ", fileName, line);
	OutputDebugString(buffer);

	// retrieve the variable arguments
	va_start(args, msg);
	vsprintf(buffer, msg, args);
	OutputDebugString(buffer);
	va_end(args);
}

void MyFunc(const std::string &one, const std::string &two)
{
	auto res = one + two;
	std::cout << res << "\n";
}

int main()
{
	MY_TRACE("Hello World %d\n", 5);
	MyFunc("Hello ", "World");
	int a = 0; 
}
