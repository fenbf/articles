#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <numeric>
#include <optional>
#include <utility>

class CompanyDatabase {
public:
    struct EmployeeRecord { std::string _name; std::string _surname; std::string _city; double _salary; };    
    
public:
    int GenerateID() { return _nextAvailableID++; }
    size_t MaxEntries() const { return s_table.size(); }
    
    // we use .at so that it might throw...
    std::string FetchName(int id) const    { return s_table.at(id)._name; }
    std::string FetchSurname(int id) const { return s_table.at(id)._surname; }
    std::string FetchCity(int id) const { return s_table.at(id)._city; }
    double      FetchSalary(int id) const  { return s_table.at(id)._salary; }
    
    EmployeeRecord FetchRecord(int id) const { return s_table.at(id); }
    
private:
    int _nextAvailableID { 0 };
    
    static inline const std::vector<EmployeeRecord> s_table {
        { "John", "Doe", "Cracow", 120.00 },
        { "Kate", "Doe", "Cracow", 80.00 },
        { "Linda", "Doe", "Warsaw", 200.00 },
        { "Marc", "Doe", "Warsaw", 100.00 }
    };
};

class Employee {
public:
    explicit Employee(int id) : _id(id) { } // not initialised!
    
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

void InitTest() {
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

class EmployeeInited {
public:
    EmployeeInited(int id, CompanyDatabase& db) : _id(id) { 
        _name = db.FetchName(_id);
         _surname = db.FetchSurname(_id);
         _city = db.FetchCity(_id);
         _salary = db.FetchSalary(_id);
    } 
    
    std::string City() const { return _city; }
    double Salary() const { return _salary; }
    
    friend std::ostream& operator<<(std::ostream& os, const EmployeeInited& em)
    {
        os << em._id << ": " << em._name << ", " << em._surname << ", " << em._city << ", " << em._salary << '\n';
        return os;
    }
private:
    int _id { -1 };
    std::string _name;
    std::string _surname;
    std::string _city;
    double _salary { 0.0 };
};

void InitedTest() {
    CompanyDatabase db;
    std::vector<std::pair<int, std::optional<EmployeeInited>>> workers;
    for (size_t i = 0; i < db.MaxEntries(); ++i)
        workers.emplace_back(std::make_pair( db.GenerateID(), std::nullopt ));
        
    auto SalaryOp = [&db](double curr, std::pair<int, std::optional<EmployeeInited>>& entry) {
        if (!entry.second.has_value())
            entry.second = EmployeeInited(entry.first, db);
        return curr + entry.second->Salary();
    };
    const double sumSalary = std::accumulate(std::begin(workers), std::end(workers), 0.0, SalaryOp); 
    std::cout << "Sum salary: " << sumSalary << '\n';
    
    // by cities:
    std::map<std::string, int> mapByCity;
    for (auto& entry : workers)
    {
        if (!entry.second.has_value())
            entry.second = EmployeeInited(entry.first, db);
            
        mapByCity[entry.second->City()]++;
    }
    
    for (const auto&[city, num] : mapByCity)
        std::cout << city << ": " << num << '\n';
}

class EmployeeOpt {
public:
    explicit EmployeeOpt(int id) : _id(id) { } 
    
    void Initialise(CompanyDatabase& db) {
         _rec = db.FetchRecord(_id);
    }
    
    bool IsInitialised() const { return _rec.has_value(); }
    
    std::string City() const { return _rec->_city; }
    double Salary() const { return _rec->_salary; }
    
    friend std::ostream& operator<<(std::ostream& os, const EmployeeOpt& em)
    {
        os << em._id << ": " << em._rec->_name << ", " << em._rec->_surname << ", " << em._rec->_city << ", " << em._rec->_salary << '\n';
        return os;
    }
private:
    int _id { -1 };
    std::optional<CompanyDatabase::EmployeeRecord> _rec;
};

void OptionalTest() {
    CompanyDatabase db;
    std::vector<EmployeeOpt> workers;
    for (size_t i = 0; i < db.MaxEntries(); ++i)
         workers.emplace_back(EmployeeOpt { db.GenerateID() });
        
    auto SalaryOp = [&db](double curr, EmployeeOpt& em) {
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

int main() {
    InitTest();
    InitedTest();
    OptionalTest();
}