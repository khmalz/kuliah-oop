#include "../repo/seller.h"
#include "../repo/item.h"
#include "../repo/items.h"

Seller::Seller(BankCustomer *customerSeller, Items *itemsSeller)
    : customer(customerSeller), items(itemsSeller) {}

BankCustomer *Seller::getCustomer() const
{
    return customer;
}

Items *Seller::getItems() const { return items; }

void Seller::addItem(const Item &item)
{
    items->addItem(item);
}

void Seller::displayItem(uint id)
{
    for (auto &item : items->getItems())
    {
        if (item.getId() == id)
        {
            item.showItem();
            return;
        }
    }
    throw std::runtime_error("Item not found");
}