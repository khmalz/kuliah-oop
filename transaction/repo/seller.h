#ifndef SELLER_H
#define SELLER_H

#include "items.h"
#include <string>

using namespace std;

class Buyer;
class BankCustomer;

class Seller
{
private:
   Buyer *buyer;
   string storeName;
   string storeAddress;
   string storeEmail;
   Items storeItems;

public:
   Seller(Buyer *buyer, const string &storeName, const string &storeAddress, const string &storeEmail);

   Buyer *getBuyer() const;
   string getStoreName() const;
   BankCustomer *getCustomerAccount() const;
   Items *getStoreItems();
};

#endif