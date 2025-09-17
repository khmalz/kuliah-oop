#ifndef BUYER_SERVICE_H
#define BUYER_SERVICE_H

#include "buyer.h"
#include <vector>
using namespace std;

class BuyerService
{
private:
   vector<Buyer> buyers;

public:
   void addBuyer(const Buyer &buyer);
   Buyer *findBuyerById(int id);
   Buyer findBuyerByIdValue(int id);
   Buyer &findBuyerByIdRef(int id);
   vector<Buyer> getAllBuyers();
};

#endif
