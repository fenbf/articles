#include <algorithm>
#include <execution>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <ranges>
#include <set>
#include <string>
#include <vector>
#include <type_traits>

#include "simpleperf.h"

// filter - copy only those elements into out that satisfies the predicate
template <typename TContainer>
void Print(std::string_view intro, const TContainer& container) {
	std::cout << intro << ": ";
	for (size_t i = 0; auto && elem : container)
		std::cout << elem << (++i == container.size() ? "\n" : ", ");
};

// vec and out won't ever overlap
// but you can vec = Filter(vec)
template <typename T, typename Pred>
auto FilterRaw(const std::vector<T>& vec, Pred p) {
	std::vector<T> out;
	for (auto&& elem : vec)
		if (p(elem))
			out.push_back(elem);
	return out;
}

// to appreciate new range based loops
template <typename T, typename Pred>
auto FilterRawOld(const std::vector<T>& vec, Pred p) {
	std::vector<T> out;
	for (typename std::vector<T>::const_iterator it = begin(vec); it != end(vec); ++it)
		if (p(*it))
			out.push_back(*it);
	return out;
}

template <typename T, typename Pred>
auto FilterCopyIf(const std::vector<T>& vec, Pred p) {
	std::vector<T> out;
	std::copy_if(begin(vec), end(vec), std::back_inserter(out), p);
	return out;
}

template <typename T, typename = void>
struct has_push_back : std::false_type {};

template <typename T>
struct has_push_back<T
	, std::void_t<decltype(std::declval<T>().push_back(std::declval<typename T::value_type>()))>
	> : std::true_type {};

template <typename TCont, typename Pred>
auto FilterCopyIfGen(const TCont& cont, Pred p) {
	TCont out;
	if constexpr(has_push_back<TCont>::value)
		std::copy_if(begin(cont), end(cont), std::back_inserter(out), p);
	else
		std::copy_if(begin(cont), end(cont), std::inserter(out, out.begin()), p);

	return out;
}

template <typename T, std::predicate<const T&> Pred>
auto FilterCopyIfConcepts(const std::vector<T>& vec, Pred p) {
	std::vector<T> out;
	std::copy_if(begin(vec), end(vec), std::back_inserter(out), p);
	return out;
}

template <typename T, typename Pred>
auto FilterCopyIfPar(const std::vector<T>& vec, Pred p) {
	std::vector<T> out;
	std::mutex mut;
	std::for_each(std::execution::par, begin(vec), end(vec),
		[&out, &mut, p](auto&& elem) {
			if (p(elem)) {
				std::unique_lock lock(mut);
				out.push_back(elem);
			}
		});

	return out;
}

template <typename T, typename Pred>
auto FilterCopyIfParCompose(const std::vector<T>& vec, Pred p) {
	std::vector<uint32_t> buffer(vec.size());
	std::vector<uint32_t> idx(vec.size());
	std::transform(std::execution::par, begin(vec), end(vec), begin(buffer), [&p](const T& elem) {
		return p(elem) ? 1 : 0;
	});

	//Print("buffer", buffer);
	std::exclusive_scan(std::execution::par, begin(buffer), end(buffer), begin(idx), 0);
	//Print("idx", idx);

	std::vector<T> out(idx.back());
	std::vector<size_t> indexes(vec.size());
	std::iota(indexes.begin(), indexes.end(), 0);

	std::for_each(std::execution::par, begin(indexes), end(indexes), [&buffer, &vec, &idx, &out](size_t i) {
		if (buffer[i])
		{
			out[idx[i]] = vec[i];
		}
	});


	//Print("out", out);
	return out;
}

template <typename T, typename Pred>
auto FilterCopyIfParComposeSeq(const std::vector<T>& vec, Pred p) {
	std::vector<uint32_t> buffer(vec.size());
	std::vector<uint32_t> idx(vec.size());
	std::transform(std::execution::par, begin(vec), end(vec), begin(buffer), [&p](const T& elem) {
		return p(elem) ? 1 : 0;
		});

	//Print("buffer", buffer);
	std::exclusive_scan(std::execution::par, begin(buffer), end(buffer), begin(idx), 0);
	//Print("idx", idx);

	std::vector<T> out(idx.back());
	//std::vector<size_t> indexes(vec.size());
	//std::iota(indexes.begin(), indexes.end(), 0);

	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (buffer[i])
		{
			out[idx[i]] = vec[i];
		}
	}

	//Print("out", out);
	return out;
}

template <typename T, typename Pred>
auto FilterCopyIfParChunks(const std::vector<T>& vec, Pred p) {
	const auto chunks = std::thread::hardware_concurrency();
	const auto chunkLen = vec.size() / chunks;
	std::vector<size_t> indexes(chunks);
	std::iota(indexes.begin(), indexes.end(), 0);

	std::vector<std::vector<T>> copiedChunks(chunks);

	std::for_each(std::execution::par, begin(indexes), end(indexes), [&](size_t i) {
		auto startIt = std::next(std::begin(vec), i * chunkLen);
		auto endIt = std::next(startIt, chunkLen);
		std::copy_if(startIt, endIt, std::back_inserter(copiedChunks[i]), p);
	});

	std::vector<T> out;

	for (const auto& part : copiedChunks)
		out.insert(out.end(), part.begin(), part.end());

	// remaining part:
	if (vec.size() % chunks != 0) {
		auto startIt = std::next(std::begin(vec), chunks * chunkLen);
		std::copy_if(startIt, end(vec), std::back_inserter(out), p);
	}

	return out;
}

template <typename T, typename Pred>
auto FilterCopyIfParChunksFuture(const std::vector<T>& vec, Pred p) {
	const auto chunks = std::thread::hardware_concurrency();
	const auto chunkLen = vec.size() / chunks;

	std::vector<std::future<std::vector<T>>> tasks(chunks);

	for (size_t i = 0; i < chunks; ++i) {
		auto startIt = std::next(std::begin(vec), i * chunkLen);
		auto endIt = std::next(startIt, chunkLen);
		tasks[i] = std::async(std::launch::async, [=, &p] {
			std::vector<T> chunkOut;
			std::copy_if(startIt, endIt, std::back_inserter(chunkOut), p);
			return chunkOut;
		});
	}

	std::vector<T> out;

	for (auto& ft : tasks)
	{
		auto part = ft.get();
		out.insert(out.end(), part.begin(), part.end());
	}

	// remaining part:
	if (vec.size() % chunks != 0) {
		auto startIt = std::next(std::begin(vec), chunks * chunkLen);
		std::copy_if(startIt, end(vec), std::back_inserter(out), p);
	}

	return out;
}

template <typename T, typename Pred>
auto FilterCopyIfParTransformPush(const std::vector<T>& vec, Pred p) {
	std::vector<uint32_t> buffer(vec.size());
	std::vector<uint32_t> idx(vec.size());
	std::transform(std::execution::par, begin(vec), end(vec), begin(buffer), [&p](const T& elem) {
		return p(elem) ? 1 : 0;
		});

	//Print("buffer", buffer);
	//std::exclusive_scan(std::execution::par, begin(buffer), end(buffer), begin(idx), 0);
	//Print("idx", idx);

	std::vector<T> out;// (idx.back());
	//std::vector<size_t> indexes(vec.size());
	//std::iota(indexes.begin(), indexes.end(), 0);

	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (buffer[i])
		{
			out.push_back(vec[i]);
		}
	}

	//Print("out", out);
	return out;
}

template <typename T, typename Pred>
auto FilterRemoveCopyIf(const std::vector<T>& vec, Pred p) {
	std::vector<T> out;
	std::remove_copy_if(begin(vec), end(vec), std::back_inserter(out), std::not_fn(p));
	return out;
}

template <typename T, typename Pred>
auto FilterRemoveErase(const std::vector<T>& vec, Pred p) {
	auto out = vec;
	out.erase(std::remove_if(begin(out), end(out), std::not_fn(p)), end(out));
	return out;
}

template <typename T, typename Pred>
auto FilterEraseIf(const std::vector<T>& vec, Pred p) {
	auto out = vec;
	std::erase_if(out, std::not_fn(p));
	return out;
}

template <typename TCont, typename Pred>
auto FilterEraseIfGen(const TCont& cont, Pred p) {
	auto out = cont;
	std::erase_if(out, std::not_fn(p));
	return out;
}

//template <typename TCont, std::predicate Pred>
//auto FilterEraseIfGenConcepts(const TCont& cont, Pred p) {
//	auto out = cont;
//	std::erase_if(out, std::not_fn(p));
//	return out;
//}

template <typename T, typename Pred>
auto FilterRangesCopyIf(const std::vector<T>& vec, Pred p) {
	std::vector<T> out;
	std::ranges::copy_if(vec, std::back_inserter(out), p);
	return out;
}

template <typename TCont, typename Pred>
auto FilterRangesCopyIfGen(const TCont& vec, Pred p) {
	TCont out;
	std::ranges::copy_if(vec, std::back_inserter(out), p);
	return out;
}

int main(int argc, const char** argv) {
	const std::vector<std::string> vec{ "Hello", "**txt", "World", "error", "warning", "C++", "****" };

	auto printVec = [](std::string_view intro, const auto& container) {
		std::cout << intro << ": ";
		for (size_t i = 0; auto && elem : container)
			std::cout << elem << (++i == container.size() ? "\n" : ", ");
	};

	printVec("initial vec:\t", vec);
	{
		auto filtered = FilterRaw(vec, [](auto& elem) { return !elem.starts_with('*'); });
		printVec("FilterRaw", filtered);
	}
	{
		auto filtered = FilterRawOld(vec, [](auto& elem) { return !elem.starts_with('*'); });
		printVec("FilterRawOld", filtered);
	}
	{
		auto filtered = FilterCopyIf(vec, [](auto& elem) { return !elem.starts_with('*'); });
		printVec("FilterCopyIf", filtered);

		//filtered = FilterCopyIf(vec, [](auto& elem) { return elem; });
	}
	{
		auto filtered = FilterCopyIfConcepts(vec, [](auto& elem) { return !elem.starts_with('*'); });
		printVec("FilterCopyIfConcepts", filtered);

		//filtered = FilterCopyIf(vec, [](auto& elem) { return std::pair{ 1, 1 }; });
	}
	{
		auto filtered = FilterCopyIfPar(vec, [](auto& elem) { return !elem.starts_with('*'); });
		printVec("FilterCopyIfPar", filtered);
	}
	{
		auto filtered = FilterRemoveCopyIf(vec, [](auto& elem) { return !elem.starts_with('*'); });
		printVec("FilterRemoveCopyIf", filtered);
	}
	{
		auto filtered = FilterRemoveErase(vec, [](auto& elem) { return !elem.starts_with('*'); });
		printVec("FilterRemoveErase", filtered);
	}
	{
		auto filtered = FilterEraseIf(vec, [](auto& elem) { return !elem.starts_with('*'); });
		printVec("FilterEraseIf", filtered);
	}
	{
		auto filtered = FilterRangesCopyIf(vec, [](auto& elem) { return !elem.starts_with('*'); });
		printVec("FilterRangesCopyIf", filtered);
	}
	{
		auto filtered = FilterRangesCopyIfGen(vec, [](auto& elem) { return !elem.starts_with('*'); });
		printVec("FilterRangesCopyIf", filtered);
	}
	{
		std::set<std::string> mySet{ "Hello", "**txt", "World", "error", "warning", "C++", "****" };
		printVec("initial set", mySet);
		auto filtered = FilterEraseIfGen(mySet, [](auto& elem) { return !elem.starts_with('*'); });
		printVec("FilterRangesCopyIf", filtered);
	}

	{
		std::set<std::string> mySet{ "Hello", "**txt", "World", "error", "warning", "C++", "****" };
		auto filtered = FilterCopyIfGen(mySet, [](auto& elem) { return !elem.starts_with('*'); });
		printVec("FilterCopyIfGen", filtered);
	}

	{
		auto filtered = FilterCopyIfGen(vec, [](auto& elem) { return !elem.starts_with('*'); });
		printVec("FilterCopyIfGen", filtered);
	}

	// benchmark:
	std::cout << "\n benchmarks: \n\n";

	const size_t VEC_SIZE = argc > 1 ? atoi(argv[1]) : 62;
	std::cout << "benchmark vec size: " << VEC_SIZE << '\n';

	std::vector<std::pair<double, double>> testVec(VEC_SIZE);
	std::ranges::generate(testVec.begin(), testVec.end(), [start = testVec.front()]() mutable {
		return std::pair { start.first += 1.0, start.second += 0.5 };
	});


	auto test = [](auto& elem) {
		auto sn = sin(elem.first) * cos(elem.second + 10.0);
		return sn > 0.0;
	};

	//auto test = [](int elem) { return elem % 2 == 0; };

	//std::vector<int> testVec(VEC_SIZE);
	//std::iota(testVec.begin(), testVec.end(), 0);

	RunAndMeasure("FilterCopyIf                ", [&testVec, &test]() {
		auto filtered = FilterCopyIf(testVec, test);
		return filtered.size();
	});

	RunAndMeasure("FilterCopyIfPar             ", [&testVec, &test]() {
		auto filtered = FilterCopyIfPar(testVec, test);
		return filtered.size();
	});

	RunAndMeasure("FilterCopyIfParCompose      ", [&testVec, &test]() {
		auto filtered = FilterCopyIfParCompose(testVec, test);
		return filtered.size();
	});

	RunAndMeasure("FilterCopyIfParComposeSeq   ", [&testVec, &test]() {
		auto filtered = FilterCopyIfParComposeSeq(testVec, test);
		return filtered.size();
		});
	RunAndMeasure("FilterCopyIfParTransformPush", [&testVec, &test]() {
		auto filtered = FilterCopyIfParTransformPush(testVec, test);
		return filtered.size();
		});
	RunAndMeasure("FilterCopyIfParChunks       ", [&testVec, &test]() {
		auto filtered = FilterCopyIfParChunks(testVec, test);
		return filtered.size();
		});
	RunAndMeasure("FilterCopyIfParChunksFuture ", [&testVec, &test]() {
		auto filtered = FilterCopyIfParChunksFuture(testVec, test);
		return filtered.size();
		});
}