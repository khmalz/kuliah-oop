#include "../repo/bank.h"
#include "../repo/bankCustomer.h"
#include <iostream>

using namespace std;

void Bank::addCustomer(const BankCustomer &customer)
{
   customers.push_back(customer);
}

BankCustomer *Bank::findCustomerById(uint id)
{
   for (auto &c : customers)
   {
      if (c.getId() == id)
         return &c;
   }
   return nullptr;
}

const vector<BankCustomer> &Bank::getAllCustomers() const
{
   return customers;
}

void Bank::showAllCustomers()
{
   const vector<BankCustomer> getCustomers = getAllCustomers();

   for (auto &c : getCustomers)
   {
      cout << "Customer: "
           << c.getId() << ", Name: "
           << c.getName() << ", BankID: "
           << c.getBankAccountId() << ", Balance: "
           << c.getBalance()
           << endl;
   }
}

void Bank::transfer(uint from, uint to, double amount)
{
   findCustomerById(from)->withdraw(amount);
   findCustomerById(to)->deposit(amount);
}
