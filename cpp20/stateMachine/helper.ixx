export module helper;

export namespace helper {
	template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
	//template<class... Ts> overload(Ts...)->overload<Ts...>; // no need in C++20, MSVC?
}