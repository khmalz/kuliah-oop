#pragma once
#include "item.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;

class Order
{
private:
   int id;
   string buyerName;
   string status;
   vector<Item> items;
   double totalAmount;

public:
   Order(int id, const string &buyerName, const string &status,
         const vector<Item> &items, double totalAmount)
       : id(id), buyerName(buyerName), status(status),
         items(items), totalAmount(totalAmount) {}

   int getId() const { return id; }
   string getBuyerName() const { return buyerName; }
   string getStatus() const { return status; }
   double getTotalAmount() const { return totalAmount; }
   vector<Item> getItems() const { return items; }

   void printSummary() const
   {
      cout << "Order ID: " << id << " | Buyer: " << buyerName
           << " | Total: $" << totalAmount
           << " | Status: " << status << endl;
   }
};
