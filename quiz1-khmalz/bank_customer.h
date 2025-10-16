#ifndef BANK_CUSTOMER_H
#define BANK_CUSTOMER_H

#include <string>
#include <vector>

using namespace std;

class BankCustomer
{
private:
    int id;
    string name;
    double balance;
    vector<string> transactionHistory;

public:
    BankCustomer(int id, const string &name, double balance);

    int getId() const;
    string getName() const;
    double getBalance() const;

    void setName(const string &name);
    void setBalance(double balance);
    void addBalance(double amount);
    bool withdrawBalance(double amount);

    void printInfo() const;

    void addTransaction(const string &desc);
    void printTransactionHistory() const;
};

#endif // BANK_CUSTOMER_H