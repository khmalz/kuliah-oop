#ifndef SELLER_H
#define SELLER_H

#include "bankCustomer.h"
#include "item.h"
#include "items.h"

#include <iostream>
using namespace std;

class Seller
{
private:
   BankCustomer *customer;
   Items *items;

public:
   Seller(BankCustomer *customerSeller, Items *itemsSeller);
   BankCustomer *getCustomer() const;
   Items *getItems() const;
   void addItem(const Item &item);
   void displayItem(uint id);
};

#endif