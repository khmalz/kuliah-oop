#include "bank_customer.h"
#include <iostream>

BankCustomer::BankCustomer(int id, const string &name, double balance)
    : id(id), name(name), balance(balance) {}

int BankCustomer::getId() const { return this->id; }
string BankCustomer::getName() const { return this->name; }
double BankCustomer::getBalance() const { return this->balance; }

void BankCustomer::setBalance(double amount) { this->balance = amount; }

void BankCustomer::addBalance(double amount)
{
    this->balance += amount;
    addTransaction("Deposited $" + to_string(amount));
}

bool BankCustomer::withdrawBalance(double amount)
{
    if (amount > this->balance)
    {
        cout << "Rejected: Insufficient funds!" << endl;
        addTransaction("Failed withdrawal $" + to_string(amount) + " (insufficient funds)");
        return false;
    }
    this->balance -= amount;
    addTransaction("Withdrew $" + to_string(amount));
    return true;
}

void BankCustomer::printInfo() const
{
    cout << "Customer Name: " << this->name << endl;
    cout << "Customer ID: " << this->id << endl;
    cout << "Balance: $" << this->balance << endl;
}

void BankCustomer::addTransaction(const string &desc)
{
    transactionHistory.push_back(desc);
}

void BankCustomer::printTransactionHistory() const
{
    cout << "\nTransaction History for " << name << ":\n";
    if (transactionHistory.empty())
    {
        cout << "No transactions recorded.\n";
    }
    else
    {
        for (const auto &t : transactionHistory)
            cout << "- " << t << endl;
    }
}
