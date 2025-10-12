#include "../repo/seller.h"

Seller::Seller(Buyer *buyerRef, const std::string &storeName, const std::string &storeAddress, const std::string &storeEmail)
    : buyer(buyerRef), storeName(storeName), storeAddress(storeAddress), storeEmail(storeEmail)
{
}

Buyer *Seller::getBuyer() const
{
    return buyer;
}

std::string Seller::getStoreName() const
{
    return storeName;
}

BankCustomer *Seller::getCustomerAccount() const
{
    return buyer->getCustomer();
}

void Seller::addItem(Items &allItems, const Item &newItem)
{
    allItems.addItem(newItem);
    std::cout << "Seller " << buyer->getName() << " from store " << storeName
              << " added item " << newItem.getName() << std::endl;
}