#ifndef BANK_H
#define BANK_H

#include "bankCustomer.h"
#include <vector>
using namespace std;

class Bank
{
private:
   vector<BankCustomer> customers;

public:
   void addCustomer(const BankCustomer &customer);
   BankCustomer *findCustomerById(uint id);
   const vector<BankCustomer> &getAllCustomers() const;
   void showAllCustomers();
   void transfer(uint from, uint to, double amount);
};

#endif
