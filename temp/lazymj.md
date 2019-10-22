# Lazy initialization in C++

Lazy initialization is one of those design patterns which is in use in almost all programming languages. Its goal is to move some object's construction forward in time, just because it's somehow expensive, or it's complete removal in case external conditions implies it's completely unnecessary. Let's talk about using this pattern with C++ standard library.

## Problem description

Let's make a real-life example. We have an RAII object that represents a file on the hard drive. We deliberately won't use `std::ifstream` class, as it allows us late file opening so that using *late initialization* pattern would be pointless. So let's consider following class:

```c++
class File
{
public:
	File(std::string_view fileName)
	{
		std::cout << "Opening file " << fileName << std::endl;
	}
	~File()
	{
		std::cout << "Closing file" << std::endl;
	}
	File(const File&) = delete;
	File(File&&) = default;
	File& operator=(const File&) = delete;
	File& operator=(File&&) = default;

	void write(std::string_view str)
	{
		std::cout << "Writing to file: " << str << std::endl;
	}
};
```

As we can see, the file is being opened in the constructor, and we cannot open it after the object construction. We can use such class to save a configuration file:

```c++
class Config
{
	File file;
public:
	Config() : file{"config.txt"}
	{
		std::cout << "Config object created" << std::endl;
	}
	
	void addOption(std::string_view name, std::string_view value)
	{
		file.write(name);
		file.write(" = ");
		file.write(value);
		file.write("\n");
	}
};
```

Here's a simple usage:

```c++
int main()
{
	Config c;
	std::cout << "Some operations..." << std::endl;
	c.addOption("dark_mode", "true");
	c.addOption("font", "DejaVu Sans Mono");
}
```

[Run on Wandbox](https://wandbox.org/permlink/hc0jKV9JdZypQ2IU)

The problem with this implementation is that we presumably open the file long time before we really need to write to it. This may block other processes from manipulating this file, which is an unnecessary side effect. We would rather open the file when the first call to `addOption` function occurs. We can achieve such behavior in several ways...

## The first way - uninitialized pointer:

Pointers seem to be the solution at first glance – they can point to some value or to "nothing" (*nullptr*). Let's go back to the example and then discuss why this is rather a bad idea.

```c++
class Config
{
	File* file{nullptr};
public:
	Config()
	{
		std::cout << "Config object created" << std::endl;
	}

	~Config()
	{
		if (file)
			delete file;
	}
	
	void addOption(std::string_view name, std::string_view value)
	{
		if (!file)
			file = new File{"config.txt"};
		file->write(name);
		file->write(" = ");
		file->write(value);
		file->write("\n");
	}
};
```

[Run on Wandbox](https://wandbox.org/permlink/8fB8ZdfFX9Pirtml)

In modern C++, holding allocated memory on heap, under a raw pointer is considered to be a bad idea in most scenarios. First of all, mixing them with the exception mechanism can lead us to memory leaks. They also require manual memory freeing, which can be bypassed using the handy and lightweight RAII design pattern.

## The second way – smart pointer

```c++
class Config
{
	std::unique_ptr<File> file{};
public:
	Config()
	{
		std::cout << "Config object created" << std::endl;
	}
	
	void addOption(std::string_view name, std::string_view value)
	{
		if (!file)
			file = std::make_unique<File>("config.txt");
		file->write(name);
		file->write(" = ");
		file->write(value);
		file->write("\n");
	}
};
```

[Run on Wandbox](https://wandbox.org/permlink/7BzF8RbCo6juykiR)

Our problem is solved in a much more elegant way. Compared to the original implementation, this method has one drawback though - the object is allocated on the heap. Allocation on the heap requires a system call (*syscall*), and the number of system calls should be rather minimized. Using objects from under the pointer might also cause less possibility of program optimization compared to objects referenced from the stack. That can lead us to another solution ...

## The third way – std::optional (C++17)

```c++
class Config
{
	std::optional<File> file{};
public:
	Config()
	{
		std::cout << "Config object created" << std::endl;
	}
	
	void addOption(std::string_view name, std::string_view value)
	{
		if (!file)
			file.emplace("config.txt");
		file->write(name);
		file->write(" = ");
		file->write(value);
		file->write("\n");
	}
};
```

[Run on Wandbox](https://wandbox.org/permlink/cCBguHZn1cQVk1ki)

We can notice that the above code doesn't differ very much with the previous one. The `unique_ptr` and `optional` references are similar, but the implementation and purpose of those classes differ significantly. First of all, in case of `std::optional` our objects memory is on the stack.

It is worth mentioning that if you are not using C++17, but some older language version, you can use the [Boost.Optional library](https://www.boost.org/doc/libs/1_71_0/libs/optional/doc/html/index.html), which implements almost identical class.

## Smart pointers vs. optional

* `unique_ptr` is – as the name implies – a wrapper around the raw pointer, while `optional` object contains memory required to its reservation as a part of the object.
* **Default constructor** of `unique_ptr` class just sets the underlying pointer to `nullptr`, while `optional` object allocation also allocates (on the stack) memory for underlying object.
* **make_unique** helper function does two things – it reserves memory required to object construction on the heap, and after that it construct an object using that memory. Its behavior can be compared to the ordinary *new operator*. On the other hand, the member function`optional::emplace`, which could be considered as an equivalent, only calls object construction with the usage of on-stack preallocated memory – so it works like less known *placement new operator*.

The consequences of above features are:

* **Copy constructor** of `unique_ptr` doesn't exist. We can use another smart pointer – `shared_ptr` so that we could copy the pointer, but it would still point one object on the heap. The `optional` class on the other hand invokes deep copy of underlying object when is copied itself. The situation is similar in the case of the operator `=`.
* **Move constructor** of `unique_ptr` class doesn't invoke deep copy neither. It just moves underlying object management to the different instance. The `optional` class invokes underlying object move constructor.
* **Destructor** of `unique_ptr` class not only destroys underlying object (calls destructor of it), but also frees memory occupied by it – so it works exactly like `operator delete`. `optional`'s destructor calls underlying object's destructor but it doesn't have to free any memory – it will be available to next objects appearing on the stack.

## Which option should I use?

The use of the `optional` class described earlier may not be the first that comes to mind those who use it. Rather, it is a class that expresses that an object *is present* or *is not*. Here we expressed the fact that the object *does not yet exist, but it will probably be in the future*. This is yet perfectly valid usage of this class.

The answer to the question "which method should I use to express late initialization?" isn't that trivial though. I would advise beginners to use `optional` by default (form *std* or *boost*). However, if we examine this issue in more detail, we can draw the following conclusions:

* **Smart pointers** should be used mainly when we want to postpone the reservation of some large amount of memory, e.g. intended for storing the contents of an image file.
* `optional` should be preferred when not the memory (its amount) is important, but the reservation of other types of resources (such as file handles, network sockets, threads, processes). It is also worth using it when the construction of the object is not possible immediately, but depends on some parameter whose value is not yet known. In addition, using this class will usually be more efficient - especially if we have, for example, a large vector of such objects and want to iterate over them.

We also cannot forget about the properties of the described classes, especially about how they are copied and moved.
