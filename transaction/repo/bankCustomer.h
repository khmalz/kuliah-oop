#ifndef BUYER_CUSTOMER_H
#define BUYER_CUSTOMER_H

#include <iostream>
#include <string>

using namespace std;

class BankCustomer
{
private:
   uint id;
   string name;
   uint bankAccountId;
   int balance;

public:
   BankCustomer(uint id, string name, uint bankAccountId, int balance);
   uint getId() const;
   string getName() const;
   uint getBankAccountId() const;
   int getBalance() const;
   void withdraw(double amount);
   void deposit(double amount);
};

#endif
