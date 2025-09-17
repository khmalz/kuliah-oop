#include "buyerService.h"
#include <iostream>

void BuyerService::addBuyer(const Buyer &buyer)
{
   buyers.push_back(buyer);
}

Buyer *BuyerService::findBuyerById(int id)
{
   for (auto &b : buyers)
   {
      if (b.getId() == id)
         return &b;
   }
   return nullptr;
}

Buyer BuyerService::findBuyerByIdValue(int id)
{
   for (auto &b : buyers)
   {
      if (b.getId() == id)
         return b;
   }

   throw runtime_error("Buyer not found");
}

Buyer &BuyerService::findBuyerByIdRef(int id)
{
   for (auto &b : buyers)
   {
      if (b.getId() == id)
      {
         return b;
      }
   }
   throw std::runtime_error("Buyer not found");
}

vector<Buyer> BuyerService::getAllBuyers()
{
   return buyers;
}
