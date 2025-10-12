#include "../repo/bank.h"
#include "../repo/bankCustomer.h"
#include "../utils.cpp"

#include <iostream>
#include <chrono>
#include <map>
#include <algorithm>
#include <vector>

using namespace std;
using namespace chrono;

void Bank::addCustomer(const BankCustomer &customer)
{
   customers.push_back(customer);
}

BankCustomer *Bank::findCustomerById(uint id)
{
   for (auto &c : customers)
   {
      if (c.getId() == id)
         return &c;
   }
   return nullptr;
}

const vector<BankCustomer> &Bank::getAllCustomers() const
{
   return customers;
}

void Bank::showAllCustomers()
{
   for (const auto &c : customers)
   {
      cout << "Customer ID: " << c.getId()
           << ", Nama: " << c.getName()
           << ", BankID: " << c.getBankAccountId()
           << ", Saldo: Rp " << c.getBalance()
           << endl;
   }
}

void Bank::transfer(uint from, uint to, double amount)
{
   findCustomerById(from)->withdraw(amount);
   findCustomerById(to)->deposit(amount);
}

bool isToday(const system_clock::time_point &timepoint)
{
   auto now = system_clock::now();
   time_t time_now = system_clock::to_time_t(now);
   time_t time_record = system_clock::to_time_t(timepoint);

   tm tm_now{};
   tm tm_record{};

#ifdef _WIN32
   localtime_s(&tm_now, &time_now);
   localtime_s(&tm_record, &time_record);
#else
   localtime_r(&time_now, &tm_now);
   localtime_r(&time_record, &tm_record);
#endif

   return (tm_now.tm_year == tm_record.tm_year &&
           tm_now.tm_mon == tm_record.tm_mon &&
           tm_now.tm_mday == tm_record.tm_mday);
}

void Bank::listRecentTransactions(const vector<Transaction> &log) const
{
   printHeader("Transaksi Dalam 1 Minggu Terakhir");

   auto now = system_clock::now();
   auto one_week_ago = now - hours(24 * 7);
   bool found = false;

   for (const auto &record : log)
   {
      if (record.transactionDate >= one_week_ago)
      {
         cout << "Toko   : " << record.sellerStoreName << "\n";
         cout << "Item   : " << record.itemName << " (x" << record.quantity << ")\n";
         cout << "Total  : Rp " << record.totalPrice << "\n";
         cout << "Status : " << statusToString(record.status) << "\n";
         cout << "----------------------------------------\n";
         found = true;
      }
   }

   if (!found)
   {
      cout << "Tidak ada transaksi dalam seminggu terakhir.\n";
   }
}

void Bank::listDormantAccounts(const vector<Transaction> &log) const
{
   printHeader("Akun Dormant (Tidak Aktif > 30 Hari)");

   map<uint, system_clock::time_point> lastActivity;

   for (const auto &record : log)
   {
      if (lastActivity.find(record.buyerId) == lastActivity.end() || record.transactionDate > lastActivity[record.buyerId])
      {
         lastActivity[record.buyerId] = record.transactionDate;
      }
      if (lastActivity.find(record.sellerId) == lastActivity.end() || record.transactionDate > lastActivity[record.sellerId])
      {
         lastActivity[record.sellerId] = record.transactionDate;
      }
   }

   auto now = system_clock::now();
   auto one_month_ago = now - hours(24 * 30);
   bool found = false;

   for (const auto &customer : customers)
   {
      bool isDormant = false;
      if (lastActivity.find(customer.getId()) == lastActivity.end())
      {
         isDormant = true;
      }
      else
      {
         if (lastActivity.at(customer.getId()) < one_month_ago)
         {
            isDormant = true;
         }
      }

      if (isDormant)
      {
         cout << "ID: " << customer.getId() << ", Nama: " << customer.getName() << ", Email: " << customer.getEmail() << "\n";
         found = true;
      }
   }

   if (!found)
   {
      cout << "Tidak ada akun yang dormant saat ini.\n";
   }
}

void Bank::listTopUsersToday(const vector<Transaction> &log, int n) const
{
   printHeader("Top " + to_string(n) + " Pengguna Aktif Hari Ini");

   map<uint, int> userTransactionCount;

   for (const auto &record : log)
   {
      if (isToday(record.transactionDate))
      {
         userTransactionCount[record.buyerId]++;
         userTransactionCount[record.sellerId]++;
      }
   }

   if (userTransactionCount.empty())
   {
      cout << "Tidak ada aktivitas transaksi hari ini.\n";
      return;
   }

   vector<pair<uint, int>> sortedUsers(userTransactionCount.begin(), userTransactionCount.end());
   sort(sortedUsers.begin(), sortedUsers.end(), [](const auto &a, const auto &b)
        { return a.second > b.second; });

   int count = 0;
   for (const auto &pair : sortedUsers)
   {
      if (count++ >= n)
         break;
      BankCustomer *customer = const_cast<Bank *>(this)->findCustomerById(pair.first);
      if (customer)
      {
         cout << count << ". " << customer->getName() << " (ID: " << customer->getId()
              << ") - Transaksi: " << pair.second << "\n";
      }
   }
}