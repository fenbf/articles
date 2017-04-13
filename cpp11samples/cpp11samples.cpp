#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>
#include <chrono>
#include <memory>


void testThreads()
{
	const int NUM_THREADS = 20;

	std::thread myThreads[NUM_THREADS];
	
    for (int i = 0; i < NUM_THREADS; ++i)
	{
		myThreads[i] = std::thread([i](){
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			std::cout << "Hello World " << "from thread: " << i << std::endl;
			std::this_thread::yield();
		});
	}
	for (auto &t : myThreads)
		t.join();

	std::cout << "All threads are done..." << std::endl;
}

double perform_long_computation()
{
	// a very  time consuming algorithm...
	return 10;
}

void futureTest()
{
	std::future<double> result = std::async([]() { return perform_long_computation(); });
	double finalResult = result.get();
}

class Value
{
public:
	std::string name;
	double age;

	Value(std::string s, double a) : name(s), age(a) { }
};

void testFor()
{
	// 1
	std::vector<int> vec{ 0, 1, 2, 3, 4 };
	auto it = std::cbegin(vec); // "it" is now a vector<int>::const_iterator 

	// 2
	for (int i : { 0, 1, 2, 3, 4 }) // C-style array
		std::cout << i << ", ";

	//// 4
	//for (auto i : container)
	//	// i is a copy of i-th element

	//for (auto &i : container)
	//	// i is reference to i-th element

	//for (const auto &i : container)
	//	// i is const reference to i-th element

	// 3
	// uniform initialization for string->pair (city name, (lat, lon))
	std::map<std::string, std::pair<double, double>> mapOfCities{
		{ "London, UK", { 51.508, -0.125 } },
		{ "Cracow, PL", { 50.064, 19.944 } },
		{ "New York, US", { 40.714, -74.005 } }
	};

	//std::sort(std::begin(mapOfCities), std::end(mapOfCities), [](const ))

	for (const auto &keyValue : mapOfCities)
	{
		std::cout << keyValue.first << ": " <<
			keyValue.second.first << ", " <<
			keyValue.second.second <<
			std::endl;
	}


	std::vector<int> nums{ 0, 1, 2, 3, 4, 5, 6 };

}

bool reverseOrder(const double &a, const double &b)
{
	return b < a;
}

void testLamda()
{
	std::vector<double> vecMarks{ 0, 1, 2, 3, 4 };

	std::sort(vecMarks.begin(), vecMarks.end(),
		reverseOrder);

	std::sort(begin(vecMarks), end(vecMarks), 
		[](const double &a, const double &b){
		return b < a;
	});

	int counter = 0;
	std::sort(begin(vecMarks), end(vecMarks),
		[&](const double &a, const double &b){
		++counter;
		return b < a;
	});

	auto lam = [](int a) -> double { return (double)a; };
	double d = lam(0);
}

//enum CarType { ctNone, ctSport, ctCoupe, ctVan };
//
//class Car
//{
//protected:
//	std::string mName;  // default is "none"
//	int         mAge;   // default is "0"
//	CarType     mType;  // default is ctNone
//public:
//	Car() :
//		mName("none"),
//		mAge(0),
//		mType(ctNone)
//	{ }
//	Car(const char *name) :
//		mName(name),
//		mAge(0),
//		mType(ctNone)
//	{ }
//	Car(const char *name, int age) :
//		mName(name),
//		mAge(age),
//		mType(ctNone)
//	{ }
//	Car(const char *name, int age, CarType ct) :
//		mName(name),
//		mAge(age),
//		mType(ct)
//	{ }
//	virtual ~Car() { }
//
//	virtual void Drive() { }
//};
//
//class SportCar : public Car
//{
//public:
//	SportCar(const char *name, int age) :
//		Car(name, age, ctSport)
//	{ }
//
//	virtual void Drive() { }
//};

enum class CarType { None, Sport, Coupe, Van };

class Car
{
protected:
	std::string mName{ "none" };
	int         mAge{ 0 };
	CarType     mType{ CarType::None };
public:
	Car(const char *name) :
		mName{ name }
	{ }
	Car(const char *name, int age) :
		mName{ name },
		mAge{ age }
	{ }
	Car(const char *name, int age, CarType ct) :
		mName{ name },
		mAge{ age },
		mType{ ct }
	{ }
	virtual ~Car() { }

	Car(const Car&) = delete;
	Car& operator=(const Car &) = delete;

	virtual void Drive(int distance) { }
	virtual void Stop() { }
};

//class FlyingCar : public Car
//{
//public:
//	FlyingCar(const char *name, int age) :
//		Car(name, age, CarType::Sport)
//	{ }
//
//	virtual void Drive(int distance) override { }
//	virtual void Stop() override { }
//	virtual void Fly() override { } // error: "Fly" is not a virtual function on the base class
//};

class SportCar final : public Car
{
public:
	SportCar(const char *name, int age) :
		Car(name, age, CarType::Sport)
	{ }

	virtual void Drive(int distance) override { }
	virtual void Stop() override { }
};

// error: cannot derive from a 'final' class (SportCar)
/*class SuperExtraCar : public SportCar
{

};*/

void carTest()
{
	Car car("super");

	// error: cannot copy car, copy constructor is 'deleted'
	// Car c = car;
}

void testCont()
{
	std::map<std::string, double> test1 = {
		{ "AA", 0.0 },
		{ "BB", 1.0 }
	};

	for (auto &a : test1)
	{
		std::cout << a.second << std::endl;
	}

	std::map<int, Value> test = {
		{ 1, { "aa", 0.0 } },
		{ 2, { "bb", 0.0 } }
	};

	for (auto &a : test)
	{
		std::cout << a.second.name << std::endl;
	}

	Value v = { "AA", 0.1 };
}

void badPointers()
{
	Car *car = new Car("my");

	bool some_condition = true;
	if (some_condition)
		return;

	delete car;
}

void betterPointers()
{
	std::unique_ptr<Car> car(new Car("my"));

	bool some_condition = true;
	if (some_condition)
		return;
}

int main()
{
	// carTest();	
	// testThreads();
	// badPointers();
	// betterPointers();
	// testCont();
	// testFor();
	testLamda();

	std::cout << "\n\n\nHello World!" << std::endl;
}
