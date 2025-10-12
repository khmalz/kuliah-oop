#include "../repo/bankCustomer.h"
#include <string>
#include <iostream>

BankCustomer::BankCustomer(uint id, const std::string &name, const std::string &email, uint bankAccountId, double initialDeposit)
    : id(id), name(name), email(email), bankAccountId(bankAccountId), balance(initialDeposit) {}

uint BankCustomer::getId() const { return id; }
std::string BankCustomer::getName() const { return name; }
std::string BankCustomer::getEmail() const { return email; }
uint BankCustomer::getBankAccountId() const { return bankAccountId; }
double BankCustomer::getBalance() const { return balance; }

void BankCustomer::withdraw(double amount)
{
   if (amount > balance)
   {
      throw std::runtime_error("Withdrawal amount exceeds balance.");
   }
   balance -= amount;
}

void BankCustomer::deposit(double amount)
{
   balance += amount;
}