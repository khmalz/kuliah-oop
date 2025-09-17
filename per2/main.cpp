#include <iostream>
#include <string>
#include "employee.cpp"
#include <vector>
#include <algorithm>

using namespace std;

vector<Employee> initEmployees()
{
   Employee john{1, "John Doe", 30, 50000.0};
   Employee jane{2, "Jane Melly", 25, 60000.0};
   Employee bob{};
   bob.id = 3;
   bob.name = "Bob Smith";
   bob.age = 35;
   bob.salary = 60000.0;

   vector<Employee> employees{john, jane, bob};
   return employees;
}

void printEmployees(vector<Employee> employees)
{
   // for (int i = 0; i < employees.size(); i++)
   // {
   //    cout << "Employee " << i + 1 << ": " << employees[i].id << ", " << employees[i].name << ", " << employees[i].age << ", " << employees[i].salary << endl;
   // }

   for (const Employee &employee : employees)
   {
      cout << "Employee: " << employee.id << ", " << employee.name << ", " << employee.age << ", " << employee.salary << endl;
   }
}

vector<Employee> addEmployee(vector<Employee> employees, Employee employee)
{
   employees.push_back(employee);
   return employees;
}

vector<Employee> removeEmployeeWithName(vector<Employee> employees, string name)
{
   vector<Employee>::iterator it = remove_if(employees.begin(), employees.end(),
                                             [&name](const Employee &employee)
                                             { return employee.name == name; });

   employees.erase(it, employees.end());

   return employees;
}

vector<Employee> mergeVectors(vector<Employee> employees1, vector<Employee> employees2)
{
   vector<Employee> allEmployees = employees1;
   allEmployees.insert(allEmployees.end(), employees2.begin(), employees2.end());

   return allEmployees;
}

int main()
{
   vector<Employee> employees = initEmployees();
   Employee john = employees[0];
   Employee jane = employees[1];
   Employee bob = employees[2];

   int totalAge{john.age + jane.age + bob.age};
   cout << "Total age: " << totalAge << endl;

   vector<Employee> employees2 = initEmployees();

   vector<Employee> allEmployees = mergeVectors(employees, employees2);

   printEmployees(allEmployees);

   cout << "=========================" << endl;

   vector<Employee> newallEmployees = removeEmployeeWithName(allEmployees, "John Doe");

   printEmployees(newallEmployees);
};