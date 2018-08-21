#pragma once

#include <iostream>

// from https://stackoverflow.com/questions/33975479/escape-and-clobber-equivalent-in-msvc
/**
 * Call doNotOptimizeAway(var) against variables that you use for
 * benchmarking but otherwise are useless. The compiler tends to do a
 * good job at eliminating unused variables, and this function fools
 * it into thinking var is in fact needed.
 */
#ifdef _MSC_VER

#pragma optimize("", off)

template <class T>
void DoNotOptimizeAway(T&& datum) {
	datum = datum;
}

#pragma optimize("", on)

#elif defined(__clang__)

template <class T>
__attribute__((__optnone__)) void DoNotOptimizeAway(T&& /* datum */) {}

#else

template <class T>
void DoNotOptimizeAway(T&& datum) {
	asm volatile("" : "+r" (datum));
}

#endif

template <typename TFunc> void RunAndMeasure(const char* title, TFunc func)
{
	const auto start = std::chrono::steady_clock::now();
	auto ret = func();
	const auto end = std::chrono::steady_clock::now();
	DoNotOptimizeAway(ret);
	std::cout << title << ": " << std::chrono::duration <double, std::milli>(end - start).count() << " ms\n";
}

