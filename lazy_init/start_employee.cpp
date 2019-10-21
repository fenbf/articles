#include <iostream>
#include <string>

class Database {
      
};

class Employee {
public:
    Employee(int id) : _id(id) { } // not initialised!
    
    void Initialise() {
        // skip error handling for now...
        _initialised = true;
    }
    
private:
    bool _isInitialised { false };
    int _id { -1 }
    std::string _name;
    std::string _surname;
    std::string _address;
    double _salary { 0.0 };
};