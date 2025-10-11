#include "../repo/buyer.h"

Buyer::Buyer(BankCustomer *customerParams) : customer(customerParams) {}

BankCustomer *Buyer::getCustomer() const { return customer; }

void Buyer::buyItem(Items &items, uint itemId, int qty)
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
