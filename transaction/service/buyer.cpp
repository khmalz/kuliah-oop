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

void Buyer::buyItem(uint transactionId, Seller *seller, Items &items, uint itemId, int qty, vector<Transaction> &log)
{
   Item *item = items.findItemById(itemId);
   if (!item)
   {
      throw runtime_error("Item not found");
   }

   if (item->getQuantity() < qty)
   {
      throw runtime_error("Stock not enough");
   }

   double totalPrice = item->getPrice() * qty;
   if (customer.getBalance() < totalPrice)
   {
      throw runtime_error("Not enough balance");
   }

   customer.withdraw(totalPrice);
   seller->getCustomerAccount()->deposit(totalPrice);
   item->decreaseQuantity(qty);

   log.emplace_back(
       transactionId,
       this->id,
       seller->getBuyer()->getId(),
       itemId,
       item->getName(),
       seller->getStoreName(),
       qty,
       totalPrice);
}