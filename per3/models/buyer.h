#ifndef BUYER_H
#define BUYER_H

#include <string>
using namespace std;

class Buyer
{
private:
   int id;
   string name;
   int bankAccountId;

public:
   Buyer(int id, string name, int bankAccountId);

   int getId() const;
   string getName() const;
   int getBankAccountId() const;
};

#endif
