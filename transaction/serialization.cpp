#ifndef SERIALIZATION_CPP
#define SERIALIZATION_CPP

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <functional>
#include <stdexcept>
#include <limits>
#include <algorithm>

#include "database.cpp"
#include "utils.cpp"

using namespace std;

struct TempSellerData
{
   unsigned int buyerId;
   string storeName;
   string storeAddress;
   string storeEmail;
};

class Serialization
{
private:
   static bool loadBuyersFromFile(const string &filename = "data/buyers.txt")
   {
      cout << "Attempting to load buyers from: " << filename << endl;
      Database::buyers.clear();
      Database::mainBank = Bank();

      const function<bool(const vector<string> &)> processLine = [&](const vector<string> &data)
      {
         if (data.size() == 5)
         {
            try
            {
               unsigned int id = stoul(data[0]);
               string name = restoreSpaces(data[1]);
               string email = data[2];
               double balance = stod(data[4]);

               Database::buyers.emplace_back(id, name, email, balance);
               Database::mainBank.addCustomer(*(Database::buyers.back().getCustomer()));
               return true;
            }
            catch (const exception &e)
            {
               cerr << " -> Failed parsing buyer data: (" << e.what() << ")";
               return false;
            }
         }
         cerr << " -> Skipping invalid buyer data line (expected 5 columns, got " << data.size() << ")";
         return false;
      };

      bool success = loadDataFromFile(filename, processLine);
      const char *message;

      if (success)
         message = "Buyers loaded successfully.";
      else
         message = "Failed to load buyers completely.";

      cout << message << endl;
      return success;
   }

   static bool loadSellersFromFile(const string &filename = "data/sellers.txt")
   {
      cout << "Attempting to load sellers from: " << filename << endl;
      Database::sellers.clear();
      vector<TempSellerData> tempData;

      const function<bool(const vector<string> &)> processLine = [&](const vector<string> &data)
      {
         if (data.size() == 4)
         {
            try
            {
               unsigned int buyerId = stoul(data[0]);
               string storeName = restoreSpaces(data[1]);
               string storeAddress = restoreSpaces(data[2]);
               string storeEmail = data[3];
               tempData.push_back({buyerId, storeName, storeAddress, storeEmail});
               return true;
            }
            catch (const exception &e)
            {
               cerr << " -> Failed parsing seller data: (" << e.what() << ")";
               return false;
            }
         }
         cerr << " -> Skipping invalid seller data line (expected 4 columns, got " << data.size() << ")";
         return false;
      };

      bool readSuccess = loadDataFromFile(filename, processLine);

      if (!readSuccess)
      {
         cout << "Finished reading sellers file with errors. Pointer fixing might be incomplete." << endl;
      }

      bool allPointersFixed = true;
      for (const auto &temp : tempData)
      {
         Buyer *foundBuyer = nullptr;
         for (auto &buyer : Database::buyers)
         {
            if (buyer.getId() == temp.buyerId)
            {
               foundBuyer = &buyer;
               break;
            }
         }

         if (foundBuyer)
         {
            Database::sellers.emplace_back(foundBuyer, temp.storeName, temp.storeAddress, temp.storeEmail);
         }
         else
         {
            cerr << "Warning: Could not find Buyer ID " << temp.buyerId << " for seller '" << temp.storeName << "'. Skipping seller." << endl;
            allPointersFixed = false;
         }
      }

      if (allPointersFixed)
         cout << "Seller pointers fixed successfully." << endl;
      else
         cout << "Finished fixing seller pointers with some errors (missing buyers)." << endl;

      return readSuccess && allPointersFixed;
   }

   static bool loadTransactionsFromFile(const string &filename = "data/transactions.txt")
   {
      cout << "Attempting to load transactions from: " << filename << endl;
      Database::transactionLog.clear();
      unsigned int maxTransactionId = 0;

      const function<bool(const vector<string> &)> processLine = [&](const vector<string> &data)
      {
         if (data.size() == 10)
         {
            try
            {
               unsigned int transactionId = stoul(data[0]);
               unsigned int buyerId = stoul(data[1]);
               unsigned int sellerId = stoul(data[2]);
               unsigned int itemId = stoul(data[3]);
               string itemName = restoreSpaces(data[4]);
               string sellerStoreName = restoreSpaces(data[5]);
               int quantity = stoi(data[6]);
               double totalPrice = stod(data[7]);
               time_t timestamp_ll = stoll(data[8]);
               int statusInt = stoi(data[9]);

               system_clock::time_point transactionTimestamp = system_clock::from_time_t(timestamp_ll);

               OrderStatus status = static_cast<OrderStatus>(statusInt);
               if (status < PAID || status > CANCELED)
               {
                  throw runtime_error("Invalid status value");
               }

               Database::transactionLog.emplace_back(
                   transactionId, buyerId, sellerId, itemId,
                   itemName, sellerStoreName, quantity, totalPrice, transactionTimestamp, status);

               if (transactionId > maxTransactionId)
               {
                  maxTransactionId = transactionId;
               }

               return true;
            }
            catch (const exception &e)
            {
               cerr << " -> Failed parsing transactions data: (" << e.what() << ")";
               return false;
            }
         }
         cerr << " -> Skipping invalid transactions data line (expected 10 columns, got " << data.size() << ")";
         return false;
      };

      bool success = loadDataFromFile(filename, processLine);

      Database::nextTransactionId = (maxTransactionId > 0) ? (maxTransactionId + 1) : 1001;

      if (success)
         cout << "Transactions loaded successfully." << endl;
      else
         cout << "Finished loading transactions with some errors." << endl;
      return success;
   }

   static bool saveBuyersToFile(const string &filename = "data/buyers.txt")
   {
      cout << "Attempting to save buyers to: " << filename << endl;

      function<string(const Buyer &)> formatBuyer =
          [](const Buyer &buyer) -> string
      {
         string line = "";
         BankCustomer *customer = nullptr;

         customer = buyer.getCustomer();

         if (customer)
         {
            line = to_string(buyer.getId()) + ";" +
                   replaceSpaces(buyer.getName()) + ";" +
                   buyer.getEmail() + ";" +
                   to_string(customer->getBankAccountId()) + ";" +
                   to_string(customer->getBalance());
         }
         else
         {
            cerr << " -> Warning: Skipping buyer ID " << buyer.getId() << " (no customer data)." << endl;
         }
         return line;
      };

      bool success = saveDataToFile<vector<Buyer>>(filename, Database::buyers, formatBuyer);

      if (success)
         cout << "Buyers saved successfully." << endl;
      else
         cerr << " -> Failed to save buyers completely to " << filename << endl;
      return success;
   }

   static bool saveSellersToFile(const string &filename = "data/sellers.txt")
   {
      cout << "Attempting to save sellers to: " << filename << endl;

      function<string(const Seller &)> formatSeller =
          [](const Seller &seller) -> string
      {
         string line = "";
         if (seller.getBuyer())
         {
            line = to_string(seller.getBuyer()->getId()) + ";" +
                   replaceSpaces(seller.getStoreName()) + ";" +
                   replaceSpaces(seller.getStoreAddress()) + ";" +
                   seller.getStoreEmail();
         }
         else
         {
            cerr << " -> Warning: Skipping seller '" << seller.getStoreName() << "' (null buyer pointer)." << endl;
         }
         return line;
      };

      bool success = saveDataToFile<vector<Seller>>(filename, Database::sellers, formatSeller);

      if (success)
         cout << "Sellers saved successfully." << endl;
      else
         cerr << " -> Failed to save sellers completely to " << filename << endl;
      return success;
   }

   static bool saveTransactionsToFile(const string &filename = "data/transactions.txt")
   {
      cout << "Attempting to save transactions to: " << filename << endl;

      function<string(const Transaction &)> formatTransaction =
          [](const Transaction &record) -> string
      {
         string line = "";
         try
         {
            auto timeSinceEpoch = record.transactionDate.time_since_epoch();
            long long timestamp_s = duration_cast<seconds>(timeSinceEpoch).count();

            int statusInt = static_cast<int>(record.status);

            line = to_string(record.transactionId) + ";" +
                   to_string(record.buyerId) + ";" +
                   to_string(record.sellerId) + ";" +
                   to_string(record.itemId) + ";" +
                   replaceSpaces(record.itemName) + ";" +
                   replaceSpaces(record.sellerStoreName) + ";" +
                   to_string(record.quantity) + ";" +
                   to_string(record.totalPrice) + ";" +
                   to_string(timestamp_s) + ";" +
                   to_string(statusInt);
         }
         catch (const exception &e)
         {
            cerr << " -> Error formatting transaction ID " << record.transactionId << ": (" << e.what() << ")" << endl;
            line = "";
         }
         return line;
      };

      bool success = saveDataToFile<vector<Transaction>>(filename, Database::transactionLog, formatTransaction);

      if (success)
         cout << "Transactions saved successfully." << endl;
      else
         cerr << " -> Failed to save transactions completely to " << filename << endl;
      return success;
   }

public:
   inline static void loadAllData()
   {
      loadBuyersFromFile();
      loadSellersFromFile();
      loadTransactionsFromFile();
      cout << "----------------------------------------" << endl;
      cout << "All data loading process finished." << endl;
      cout << "----------------------------------------" << endl;
   }

   inline static void saveAllData()
   {
      saveBuyersToFile();
      saveSellersToFile();
      saveTransactionsToFile();
      cout << "----------------------------------------" << endl;
      cout << "Data saving process finished." << endl;
      cout << "----------------------------------------" << endl;
   }
};

#endif