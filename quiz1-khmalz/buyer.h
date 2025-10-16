#ifndef BUYER_H
#define BUYER_H

#include <cstddef>
#include <string>

#include "item.h"
#include "order.h"
#include "bank_customer.h"

using namespace std;

class Seller;

class Buyer
{
private:
    int id;
    string name;
    string homeAddress;
    string phoneNumber;
    string email;
    BankCustomer *account = nullptr; // Association with BankCustomer
    vector<Item> cart;

public:
    Buyer(int id,
          const string &name,
          const string &homeAddress,
          const string &phoneNumber,
          const string &email)
        : id(id), name(name), homeAddress(homeAddress),
          phoneNumber(phoneNumber), email(email), account(nullptr) {}

    int getId() const { return id; }
    void setId(int newId) { id = newId; }

    string getName() const { return name; }
    string getAddress() const { return homeAddress; }
    string getPhone() const { return phoneNumber; }
    string getEmail() const { return email; }
    BankCustomer *getAccount() const { return account; }
    bool hasAccount() const { return account != nullptr; }
    void setAccount(BankCustomer *acc)
    {
        account = acc;
    }

    void addToCart(const Item &item)
    {
        cart.push_back(item);
        cout << "Item '" << item.getName() << "' added to cart.\n";
    };
    void removeFromCart(int itemId)
    {
        for (auto it = cart.begin(); it != cart.end(); ++it)
        {
            if (it->getId() == itemId)
            {
                cout << "Item '" << it->getName() << "' removed from cart.\n";
                cart.erase(it);
                return;
            }
        }
        cout << "Item not found in cart.\n";
    };
    void viewCart() const
    {
        cout << "\n=== Your Cart ===\n";
        if (cart.empty())
        {
            cout << "Cart is empty.\n";
            return;
        }

        double total = 0;
        for (const auto &item : cart)
        {
            cout << "- " << item.getName()
                 << " x" << item.getQuantity()
                 << " ($" << item.getPrice() * item.getQuantity() << ")\n";
            total += item.getPrice() * item.getQuantity();
        }
        cout << "Total: $" << total << "\n";
    };

    void checkout(Seller &seller);

    void printInfo() const
    {
        cout << "\nBuyer ID: " << id << endl;
        cout << "Buyer Name: " << name << endl;
        cout << "Buyer Address: " << homeAddress << endl;
        cout << "Buyer Phone: " << phoneNumber << endl;
        cout << "Buyer Email: " << email << endl;

        if (account)
        {
            account->printInfo();
        }
        else
        {
            cout << "No bank account linked.\n";
        }
    }
};

#endif // BUYER_H