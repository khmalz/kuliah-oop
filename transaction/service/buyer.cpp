#include "../repo/buyer.h"
#include "../repo/transaction.h"
#include "../repo/seller.h"

#include <stdexcept>
#include <iostream>

using namespace std;

Buyer::Buyer(uint id, const string &name, const string &email, double initialDeposit)
    : id(id), name(name), email(email),
      customer(id, name, email, id + 1000, initialDeposit)
{
}

uint Buyer::getId() const { return id; }
string Buyer::getName() const { return name; }
string Buyer::getEmail() const { return email; }

BankCustomer *Buyer::getCustomer() { return &customer; }

void Buyer::buyItem(Seller *seller, Items &items, unsigned int itemId, int qty, vector<Transaction> &log)
{
   Item *item = items.findItemById(itemId);
   if (!item)
   {
      throw runtime_error("Item not found");
   }

   double totalPrice = item->getPrice() * qty;
   if (customer.getBalance() < totalPrice)
   {
      throw runtime_error("Not enough balance");
   }

   customer.withdraw(totalPrice);
   seller->getCustomerAccount()->deposit(totalPrice);
   item->decreaseQuantity(qty);

   log.emplace_back(this->id, seller->getBuyer()->getId(), itemId, qty, totalPrice);

   cout << customer.getName() << " bought " << qty
        << " x " << item->getName()
        << " for " << totalPrice << endl;
}