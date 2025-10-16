#ifndef DATABASE_CPP
#define DATABASE_CPP

#include <iostream>
#include <string>
#include <vector>

#include "repo/buyer.h"
#include "repo/seller.h"
#include "repo/bank.h"
#include "repo/transaction.h"

using namespace std;

class Database
{
public:
   inline static vector<Buyer> buyers;
   inline static vector<Seller> sellers;
   inline static Bank mainBank;
   inline static vector<Transaction> transactionLog;

   inline static string globalMessage = "";
   inline static Buyer *loggedInBuyer = nullptr;
   inline static Seller *loggedInSeller = nullptr;
   inline static uint nextItemId = 1;
   inline static uint nextTransactionId = 1001;

   inline static void displayGlobalMessage()
   {
      if (!globalMessage.empty())
      {
         cout << "----------------------------------------\n";
         cout << ">> " << globalMessage << "\n";
         cout << "----------------------------------------\n\n";
         globalMessage = "";
      }
   }
};

#endif