#ifndef BUYER_H
#define BUYER_H

#include "bankCustomer.h"
#include "items.h"
#include "transaction.h"
#include <string>

using namespace std;

class Seller;

class Buyer
{
private:
   uint id;
   string name;
   string email;
   BankCustomer customer;

public:
   Buyer(uint id, const string &name, const string &email, double initialDeposit);

   uint getId() const;
   string getName() const;
   string getEmail() const;
   BankCustomer *getCustomer();

   void buyItem(Seller *seller, Items &items, unsigned int itemId, int qty, vector<Transaction> &log);
};

#endif