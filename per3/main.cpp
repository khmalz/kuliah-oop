#include <iostream>
#include "models/buyerService.h"
using namespace std;

int main()
{
   BuyerService service;

   service.addBuyer(Buyer(1, "Alice", 101));
   service.addBuyer(Buyer(2, "Bob", 102));

   Buyer *b = service.findBuyerById(1);
   if (b != nullptr)
      cout << "Found: " << b->getName() << endl;

   Buyer buyer = service.findBuyerByIdValue(1);
   if (buyer.getId() == 1)
   {
      cout << "Found: " << buyer.getName() << endl;
   }

   buyer = service.findBuyerByIdRef(1);
   if (buyer.getId() == 1)
   {
      cout << "Found: " << buyer.getName() << endl;
   }

   for (auto &buyer : service.getAllBuyers())
   {
      cout << buyer.getId() << " - " << buyer.getName() << endl;
   }

   return 0;
}
