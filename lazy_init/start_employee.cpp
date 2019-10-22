#include <iostream>
#include <string>
#include <vector>

class CompanyDatabase {
public:
    int GenerateID() { return _nextAvailableID++; }
    
    std::string FetchName(int id) const    { return s_table.at(id)._name; }
    std::string FetchSurname(int id) const { return s_table.at(id)._surname; }
    std::string FetchAddress(int id) const { return s_table.at(id)._address; }
    double      FetchSalary(int id) const  { return s_table.at(id)._salary; }
private:
    int _nextAvailableID { 0 };
    
    struct EmployeeRecord { std::string _name; std::string _surname; std::string _address; double _salary; };
    
    static inline const std::vector<EmployeeRecord> s_table {
        { "John", "Doe", "Poland, ABC Street 67", 100.00 },
        { "John", "Doe", "Poland, ABC Street 67", 100.00 },
        { "John", "Doe", "Poland, ABC Street 67", 100.00 },
        { "John", "Doe", "Poland, ABC Street 67", 100.00 }
    };
};

class Employee {
public:
    Employee(int id) : _id(id) { } // not initialised!
    
    void Initialise(CompanyDatabase& db) {
         _name = db.FetchName(_id);
         _surname = db.FetchSurname(_id);
         _address = db.FetchAddress(_id);
         _salary = db.FetchSalary(_id);
        _isInitialised = true;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Employee& em)
    {
        os << em._id << ": " << em._name << ", " << em._surname << ", " << em._address << ", " << em._salary << '\n';
        return os;
    }
private:
    bool _isInitialised { false };
    int _id { -1 };
    std::string _name;
    std::string _surname;
    std::string _address;
    double _salary { 0.0 };
};

int main() {
    CompanyDatabase db;
    Employee john(db.GenerateID());
    john.Initialise(db);
    std::cout << john;
}