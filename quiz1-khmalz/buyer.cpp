#include "buyer.h"
#include "seller.h"

void Buyer::checkout(Seller &seller)
{
   if (cart.empty())
   {
      cout << "Cart is empty.\n";
      return;
   }

   if (!account)
   {
      cout << "You need a linked bank account to checkout.\n";
      return;
   }

   double total = 0;
   for (const auto &item : cart)
      total += item.getPrice() * item.getQuantity();

   if (account->getBalance() < total)
   {
      cout << "Insufficient balance.\n";
      return;
   }

   account->withdrawBalance(total);

   int orderId = static_cast<int>(rand());
   string buyerName = name;
   string status = "PAID";
   Order newOrder(orderId, buyerName, status, cart, total);

   seller.addOrder(newOrder);
   cout << "Checkout successful. Order sent to " << seller.getStoreName() << ".\n";

   cart.clear();
}
