#ifndef BUYER_H
#define BUYER_H

#include "bankCustomer.h"
#include "items.h"

class Buyer
{
private:
   BankCustomer *customer;

public:
   Buyer(BankCustomer *customerParams);
   BankCustomer *getCustomer() const;
   void buyItem(Items &items, uint itemId, int qty);
};

#endif
