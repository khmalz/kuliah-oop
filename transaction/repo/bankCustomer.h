#ifndef BANK_CUSTOMER_H
#define BANK_CUSTOMER_H

#include <iostream>
#include <string>

class BankCustomer
{
private:
   uint id;
   std::string name;
   std::string email;
   uint bankAccountId;
   double balance;

public:
   BankCustomer(uint id, const std::string &name, const std::string &email, uint bankAccountId, double initialDeposit);

   BankCustomer() = default;

   uint getId() const;
   std::string getName() const;
   std::string getEmail() const;
   uint getBankAccountId() const;
   double getBalance() const;

   void withdraw(double amount);
   void deposit(double amount);
};

#endif