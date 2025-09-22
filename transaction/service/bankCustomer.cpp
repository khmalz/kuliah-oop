#include "../repo/bankCustomer.h"
#include "../repo/bank.h"
#include <string>
#include <iostream>

using namespace std;

BankCustomer::BankCustomer(uint id, string name, uint bankAccountId, int balance) : id(id), name(name), bankAccountId(bankAccountId), balance(balance) {}
uint BankCustomer::getId() const { return id; }
string BankCustomer::getName() const { return name; }
uint BankCustomer::getBankAccountId() const { return bankAccountId; }

int BankCustomer::getBalance() const
{
   return balance;
}

void BankCustomer::withdraw(double amount)
{
   balance -= amount;
}

void BankCustomer::deposit(double amount)
{
   balance += amount;
}
