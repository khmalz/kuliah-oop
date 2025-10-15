#ifndef USER_H
#define USER_H

#include <string>

using namespace std;

class User
{
private:
   uint id;
   string name;
   string phone_number;

public:
   User(uint id, const string &name, const string &phone_number) : id(id), name(name), phone_number(phone_number) {};

   uint getId() const { return id; }
   string getName() const { return name; }
   string getPhoneNumber() const { return phone_number; }
};

#endif