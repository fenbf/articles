#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <numeric>

class CompanyDatabase {
public:
    int GenerateID() { return _nextAvailableID++; }
    size_t MaxEntries() const { return s_table.size(); }
    
    std::string FetchName(int id) const    { return s_table.at(id)._name; }
    std::string FetchSurname(int id) const { return s_table.at(id)._surname; }
    std::string FetchCity(int id) const { return s_table.at(id)._city; }
    double      FetchSalary(int id) const  { return s_table.at(id)._salary; }
private:
    int _nextAvailableID { 0 };
    
    struct EmployeeRecord { std::string _name; std::string _surname; std::string _city; double _salary; };
    
    static inline const std::vector<EmployeeRecord> s_table {
        { "John", "Doe", "Cracow", 120.00 },
        { "Kate", "Doe", "Cracow", 80.00 },
        { "Linda", "Doe", "Warsaw", 200.00 },
        { "Marc", "Doe", "Warsaw", 100.00 }
    };
};

class Employee {
public:
    Employee(int id) : _id(id) { } // not initialised!
    
    void Initialise(CompanyDatabase& db) {
         _name = db.FetchName(_id);
         _surname = db.FetchSurname(_id);
         _city = db.FetchCity(_id);
         _salary = db.FetchSalary(_id);
        _isInitialised = true;
    }
    
    bool IsInitialised() const { return _isInitialised; }
    
    std::string City() const { return _city; }
    double Salary() const { return _salary; }
    
    friend std::ostream& operator<<(std::ostream& os, const Employee& em)
    {
        os << em._id << ": " << em._name << ", " << em._surname << ", " << em._city << ", " << em._salary << '\n';
        return os;
    }
private:
    bool _isInitialised { false };
    int _id { -1 };
    std::string _name;
    std::string _surname;
    std::string _city;
    double _salary { 0.0 };
};

int main() {
    CompanyDatabase db;
    std::vector<Employee> workers;
    for (size_t i = 0; i < db.MaxEntries(); ++i)
        workers.emplace_back(Employee { db.GenerateID() });
        
    auto SalaryOp = [&db](double curr, Employee& em) {
        if (!em.IsInitialised())
            em.Initialise(db);
        return curr + em.Salary();
    };
    const double sumSalary = std::accumulate(std::begin(workers), std::end(workers), 0.0, SalaryOp); 
    std::cout << "Sum salary: " << sumSalary << '\n';
    
    // by cities:
    std::map<std::string, int> mapByCity;
    for (auto& em : workers)
    {
        if (!em.IsInitialised())
            em.Initialise(db);
            
        mapByCity[em.City()]++;
    }
    
    for (const auto&[city, num] : mapByCity)
        std::cout << city << ": " << num << '\n';
}