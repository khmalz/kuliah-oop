#ifndef BUYER_H
#define BUYER_H

#include "bankCustomer.h"
#include "items.h"
#include <string>

class Buyer
{
private:
   uint id;
   std::string name;
   std::string email;
   BankCustomer customer;

public:
   Buyer(uint id, const std::string &name, const std::string &email, double initialDeposit);

   // Getter
   uint getId() const;
   std::string getName() const;
   std::string getEmail() const;
   BankCustomer *getCustomer();

   void buyItem(Items &items, uint itemId, int qty);
};

#endif