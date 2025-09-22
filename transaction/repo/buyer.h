#ifndef BUYER_H
#define BUYER_H

#include "bankCustomer.h"
#include "items.h"

class Buyer
{
private:
   BankCustomer *customer;

public:
   Buyer(BankCustomer *customer) : customer(customer) {}

   BankCustomer *getCustomer() const { return customer; }

   void buyItem(Items &items, unsigned int itemId, int qty)
   {
      for (auto &item : items.getItems())
      {
         if (item.getId() == itemId)
         {
            double totalPrice = item.getPrice() * qty;

            if (customer->getBalance() < totalPrice)
               throw std::runtime_error("Not enough balance");

            customer->withdraw(totalPrice);

            item.decreaseQuantity(qty);

            cout << customer->getName() << " bought " << qty
                 << " x " << item.getName()
                 << " for " << totalPrice << endl;
            return;
         }
      }
      throw std::runtime_error("Item not found");
   }
};

#endif
