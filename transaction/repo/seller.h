#ifndef SELLER_H
#define SELLER_H

#include "buyer.h"
#include "items.h"
#include <string>

class Seller
{
private:
   Buyer *buyer;
   std::string storeName;
   std::string storeAddress;
   std::string storeEmail;

public:
   Seller(Buyer *buyer, const std::string &storeName, const std::string &storeAddress, const std::string &storeEmail);

   Buyer *getBuyer() const;
   std::string getStoreName() const;
   BankCustomer *getCustomerAccount() const;

   void addItem(Items &allItems, const Item &newItem);
};

#endif