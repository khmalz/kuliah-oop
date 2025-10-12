#ifndef TRANSACTION__H
#define TRANSACTION__H

#include <chrono>
using namespace std;

struct Transaction
{
   unsigned int buyerId;
   unsigned int sellerId;
   unsigned int itemId;
   int quantity;
   double totalPrice;
   chrono::system_clock::time_point transactionDate;

   Transaction(unsigned int bId, unsigned int sId, unsigned int iId, int qty, double price)
       : buyerId(bId), sellerId(sId), itemId(iId), quantity(qty), totalPrice(price),
         transactionDate(chrono::system_clock::now()) {}
};

#endif