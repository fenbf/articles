#include <algorithm>
#include <execution>
#include <functional>
#include <iostream>
#include <memory>
#include <ranges>
#include <set>
#include <string>
#include <vector>

// filter - copy only those elements into out that satisfies the predicate


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

template <typename T, std::predicate<const T&> Pred>
auto FilterCopyIfConcepts(const std::vector<T>& vec, Pred p) {
	std::vector<T> out;
	std::copy_if(begin(vec), end(vec), std::back_inserter(out), p);
	return out;
}

template <typename T, typename Pred>
auto FilterCopyIfPar(const std::vector<T>& vec, Pred p) {
	std::vector<T> out;
	// error C2338: Parallel algorithms require forward iterators or stronger.
	// VS 2019 16.8
	// copy each satisfying _Pred
	// not parallelized at present, parallelism expected to be feasible in a future release
	//std::copy_if(std::execution::par, begin(vec), end(vec), std::back_inserter(out), p);
	std::mutex mut;
	std::for_each(std::execution::par, begin(vec), end(vec),
		[&out, &mut, p](auto&& elem) {
			if (p(elem))
			{
				std::unique_lock lock(mut);
				out.push_back(elem);
			}
		});

	// other options:
	// helper vector with 1 and 0, 1 when we should copy
	// another for each loop and put elements of 1 in a proper destination exlusive_scan
	// create out vector of the size scan[last]+1 (probably)
	// another loop to copy elements into positions from that scan

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

int main() {
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
		auto filtered = FilterCopyIf(vec, [](auto& elem, int a) { return !elem.starts_with('*'); });
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
		std::set<std::string> mySet{ "Hello", "**txt", "World", "error", "warning", "C++", "****" };
		printVec("initial set", mySet);
		auto filtered = FilterEraseIfGen(mySet, [](auto& elem) { return !elem.starts_with('*'); });
		printVec("FilterRangesCopyIf", filtered);
	}
}