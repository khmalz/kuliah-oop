#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <chrono>
#include <string>

using namespace std;
using namespace chrono;

enum OrderStatus
{
  PAID,
  COMPLETED,
  CANCELED
};

struct Transaction
{
  unsigned int transactionId;
  unsigned int buyerId;
  unsigned int sellerId;
  unsigned int itemId;

  string itemName;
  string sellerStoreName;

  int quantity;
  double totalPrice;

  system_clock::time_point transactionDate;
  OrderStatus status;

  Transaction(unsigned int tId, unsigned int bId, unsigned int sId, unsigned int iId, const string &iName, const string &sName, int qty, double price, system_clock::time_point timestamp = system_clock::now(), OrderStatus tStatus = PAID)
      : transactionId(tId), buyerId(bId), sellerId(sId), itemId(iId),
        itemName(iName), sellerStoreName(sName),
        quantity(qty), totalPrice(price),
        transactionDate(timestamp),
        status(tStatus)
  {
  }
};

#endif