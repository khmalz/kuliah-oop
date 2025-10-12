#include "../repo/seller.h"
#include "../repo/buyer.h"

Seller::Seller(Buyer *buyerRef, const string &storeName, const string &storeAddress, const string &storeEmail)
    : buyer(buyerRef), storeName(storeName), storeAddress(storeAddress), storeEmail(storeEmail)
{
}

Buyer *Seller::getBuyer() const
{
    return buyer;
}

string Seller::getStoreName() const
{
    return storeName;
}

BankCustomer *Seller::getCustomerAccount() const
{
    return buyer->getCustomer();
}

Items *Seller::getStoreItems()
{
    return &storeItems;
}