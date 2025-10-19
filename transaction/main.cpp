#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <map>
#include <algorithm>
#include <cctype>

#include "repo/bank.h"
#include "repo/buyer.h"
#include "repo/seller.h"
#include "repo/items.h"
#include "repo/transaction.h"
#include "utils.cpp"
#include "database.cpp"
#include "serialization.cpp"

using namespace std;
using namespace chrono;

// =======================================================
// Handler Purchase
// =======================================================

void handlePurchaseItem()
{
   clearScreen();
   printHeader("Beli Barang");

   if (Database::sellers.empty())
   {
      Database::globalMessage = "Maaf, belum ada toko yang buka saat ini.";
      return;
   }

   cout << "Daftar Barang yang Tersedia:\n";
   cout << "--------------------------------------------------------\n";
   for (auto &seller : Database::sellers)
   {
      if (!seller.getStoreItems()->getItems().empty())
      {
         cout << "Toko: " << seller.getStoreName() << "\n";
         seller.getStoreItems()->showAllItems();
         cout << "\n";
      }
   }
   cout << "--------------------------------------------------------\n";

   unsigned int itemId;
   int quantity;

   cout << "Masukkan ID Item yang ingin dibeli (ketik 0 untuk batal): ";
   cin >> itemId;
   if (itemId == 0)
   {
      Database::globalMessage = "Pembelian dibatalkan.";
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      return;
   }
   cout << "Masukkan Jumlah: ";
   cin >> quantity;

   Seller *targetSeller = nullptr;
   Items *targetItems = nullptr;

   for (auto &seller : Database::sellers)
   {
      Item *item = seller.getStoreItems()->findItemById(itemId);
      if (item)
      {
         targetSeller = &seller;
         targetItems = seller.getStoreItems();
         break;
      }
   }

   if (!targetSeller)
   {
      Database::globalMessage = "Item dengan ID " + to_string(itemId) + " tidak ditemukan di toko mana pun.";
      return;
   }

   try
   {
      unsigned int newTransactionId = Database::nextTransactionId++;
      Database::loggedInBuyer->buyItem(newTransactionId, targetSeller, *targetItems, itemId, quantity, Database::transactionLog);

      Database::globalMessage = "Pembelian berhasil! ID Transaksi Anda: " + to_string(newTransactionId);
   }
   catch (const runtime_error &e)
   {
      Database::globalMessage = "Error: " + string(e.what());
   }
}

// =======================================================
// Handler action
// =======================================================

void handleUpgradeToSeller()
{
   if (!Database::loggedInBuyer)
   {
      Database::globalMessage = "Error: Tidak ada user yang login.";
      return;
   }
   if (Database::loggedInSeller)
   {
      Database::globalMessage = "Akun Anda sudah menjadi Seller!";
      return;
   }

   clearScreen();
   printHeader("Upgrade Akun ke Seller");

   string storeName, storeAddress, storeEmail;
   cout << "Data Buyer: " << Database::loggedInBuyer->getName() << "\n";
   cout << "Silakan lengkapi informasi toko Anda.\n\n";

   cout << "Nama Toko (ketik 0 untuk batal): ";
   getline(cin, storeName);
   if (storeName == "0")
   {
      Database::globalMessage = "Registrasi dibatalkan.";
      return;
   }

   cout << "Alamat Toko    : ";
   getline(cin, storeAddress);

   cout << "Email Toko     : ";
   getline(cin, storeEmail);

   Database::sellers.emplace_back(Database::loggedInBuyer, storeName, storeAddress, storeEmail);
   Database::loggedInSeller = &Database::sellers.back();

   Database::globalMessage = "Upgrade berhasil! Anda sekarang adalah Seller.";
}

void handleCheckStatus()
{
   clearScreen();
   printHeader("Status Akun");

   Database::loggedInBuyer->displayBasicInfo();
   cout << "Status   : ";
   if (Database::loggedInSeller)
   {
      cout << "Seller & Buyer\n";
      cout << "Nama Toko: " << Database::loggedInSeller->getStoreName() << "\n";
   }
   else
   {
      cout << "Buyer\n";
   }

   cout << "\nTekan [Enter] untuk kembali...";
   cin.get();
}

void handleListRecentBankTransactions()
{
   printHeader("Tampilkan Transaksi Terbaru");

   int k_days;
   cout << "Tampilkan transaksi dalam (k) hari terakhir.\n";
   cout << "Masukkan jumlah hari (k) (ketik 0 untuk batal): ";
   cin >> k_days;
   cin.ignore(numeric_limits<streamsize>::max(), '\n');

   if (k_days == 0)
   {
      Database::globalMessage = "Operasi dibatalkan.";
      return;
   }
   if (k_days < 0)
   {
      Database::globalMessage = "Jumlah hari tidak boleh negatif.";
      return;
   }

   clearScreen();
   Database::mainBank.listRecentTransactions(Database::transactionLog, k_days);
}

void handleListActiveBuyers()
{
   clearScreen();
   printHeader("Top Pembeli Aktif Hari Ini");

   int n;
   cout << "Berapa pembeli teratas yang ingin ditampilkan? (ketik 0 untuk batal): ";
   cin >> n;
   cin.ignore(numeric_limits<streamsize>::max(), '\n');
   if (n == 0)
   {
      Database::globalMessage = "Operasi dibatalkan.";
      return;
   }

   Database::mainBank.listMostActiveBuyersToday(Database::transactionLog, n);
}

void handleListActiveSellers()
{
   clearScreen();
   printHeader("Top Penjual Aktif Hari Ini");

   int n;
   cout << "Berapa penjual teratas yang ingin ditampilkan? (ketik 0 untuk batal): ";
   cin >> n;
   cin.ignore(numeric_limits<streamsize>::max(), '\n');
   if (n == 0)
   {
      Database::globalMessage = "Operasi dibatalkan.";
      return;
   }

   Database::mainBank.listMostActiveSellersToday(Database::transactionLog, n);
}

// =======================================================
// Handler authentication
// =======================================================

void handleRegisterBuyer()
{
   clearScreen();
   printHeader("Registrasi Akun Buyer Baru");

   string name, email;
   double initialDeposit = 0;

   cout << "Masukkan Nama Lengkap  (ketik 0 untuk batal): ";
   getline(cin, name);
   if (name == "0")
   {
      Database::globalMessage = "Registrasi dibatalkan.";
      return;
   }

   cout << "Masukkan Email         : ";
   getline(cin, email);

   cout << "Masukkan Deposit Awal  : Rp ";
   while (!(cin >> initialDeposit) || initialDeposit < 0)
   {
      cout << "Input tidak valid. Masukkan angka positif: Rp ";
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
   }

   unsigned int newId = Database::buyers.size() + 1;
   Database::buyers.emplace_back(newId, name, email, initialDeposit);

   Database::mainBank.addCustomer(*(Database::buyers.back().getCustomer()));

   Database::globalMessage = "Registrasi berhasil! ID Buyer Anda adalah " + to_string(newId);
}

void handleLogin()
{
   if (Database::buyers.empty())
   {
      Database::globalMessage = "Belum ada buyer terdaftar. Silakan registrasi dahulu.";
      return;
   }

   clearScreen();
   printHeader("Login");

   unsigned int id;
   cout << "Masukkan ID Buyer Anda (ketik 0 untuk batal): ";
   cin >> id;

   if (id == 0)
   {
      Database::globalMessage = "Login dibatalkan.";
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      return;
   }

   for (auto &buyer : Database::buyers)
   {
      if (buyer.getId() == id)
      {
         Database::loggedInBuyer = &buyer;
         Database::loggedInSeller = nullptr;
         for (auto &seller : Database::sellers)
         {
            if (seller.getBuyer()->getId() == id)
            {
               Database::loggedInSeller = &seller;
               break;
            }
         }
         Database::globalMessage = "Login berhasil! Selamat datang, " + Database::loggedInBuyer->getName() + ".";
         return;
      }
   }
   Database::globalMessage = "Login gagal. ID Buyer tidak ditemukan.";
}

void handleLogout()
{
   clearScreen();
   printHeader("Logout");

   char choice;
   while (true)
   {
      cout << "Apakah Anda yakin ingin logout? (y/n): ";
      cin >> choice;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      choice = tolower(choice);

      if (choice == 'y')
      {
         string name = Database::loggedInBuyer->getName();

         Database::loggedInBuyer = nullptr;
         Database::loggedInSeller = nullptr;

         Database::globalMessage = "Anda telah logout. Sampai jumpa, " + name + "!";
         return;
      }
      else if (choice == 'n')
      {
         Database::globalMessage = "Logout dibatalkan.";
         return;
      }
      else
      {
         cout << "Input tidak valid. Harap masukkan 'y' atau 'n'.\n";
      }
   }
}

// =======================================================
// Handler untuk Menampilkan Menu-Menu
// =======================================================

void showBankMenu()
{
   int choice = 0;
   while (true)
   {
      clearScreen();
      printHeader("Menu Laporan Bank");
      Database::displayGlobalMessage();

      cout << "1. Tampilkan Semua Nasabah\n";
      cout << "2. Tampilkan Transaksi Beberapa Hari Terakhir\n";
      cout << "3. Tampilkan Akun Dormant (>30 Hari)\n";
      cout << "4. Tampilkan Top Pengguna Hari Ini\n";
      cout << "5. Tampilkan Top Pembeli Hari Ini\n";
      cout << "6. Tampilkan Top Penjual Hari Ini\n";
      cout << "7. Kembali ke Menu Utama\n";
      cout << "----------------------------------------\n";
      cout << "Pilihan Anda: ";
      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         Database::globalMessage = "Input tidak valid.";
         choice = 0;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      if (choice == 7)
      {
         return;
      }

      if (choice >= 1 && choice <= 6)
      {
         clearScreen();

         switch (choice)
         {
         case 1:
            Database::mainBank.showAllCustomers();
            break;
         case 2:
            handleListRecentBankTransactions();
            break;
         case 3:
            Database::mainBank.listDormantAccounts(Database::transactionLog);
            break;
         case 4:
            printHeader("Top Pengguna Aktif Hari Ini");
            int n;
            cout << "Berapa pengguna teratas yang ingin ditampilkan? (ketik 0 untuk batal): ";
            cin >> n;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (n == 0)
            {
               Database::globalMessage = "Operasi dibatalkan.";
               return;
            }

            Database::mainBank.listTopUsersToday(Database::transactionLog, n);
            break;
         case 5:
            handleListActiveBuyers();
            break;
         case 6:
            handleListActiveSellers();
            break;
         }

         cout << "\nTekan [Enter] untuk kembali...";
         cin.get();
      }
      else
      {
         if (choice != 0)
            Database::globalMessage = "Pilihan tidak valid.";
      }
   }
}

void showBuyerMenu()
{
   int choice = 0;
   while (true)
   {
      clearScreen();
      printHeader("Menu Pembeli");
      Database::displayGlobalMessage();

      cout << "1. Beli Barang\n";
      cout << "2. Lihat Riwayat Pesanan\n";
      cout << "3. Cek Pengeluaran Pada Beberapa Hari Terakhir\n";
      cout << "4. Lihat Cash Flow\n";
      cout << "5. Konfirmasi Penerimaan Barang\n";
      cout << "6. Batalkan Pesanan\n";
      cout << "7. Kembali ke Menu Utama\n";
      cout << "----------------------------------------\n";
      cout << "Pilihan Anda: ";
      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         Database::globalMessage = "Input tidak valid.";
         choice = 0;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      if (choice == 7)
      {
         return;
      }

      if (choice >= 1 && choice <= 6)
      {
         clearScreen();

         switch (choice)
         {
         case 1:
            handlePurchaseItem();
            break;
         case 2:
            Database::loggedInBuyer->listOrders(Database::transactionLog);
            break;
         case 3:
            Database::loggedInBuyer->checkSpending();
            break;
         case 4:
            Database::loggedInBuyer->showCashFlow();
            break;
         case 5:
            Database::loggedInBuyer->confirmReceipt(Database::transactionLog);
            break;
         case 6:
            Database::loggedInBuyer->cancelOrder(Database::transactionLog, Database::sellers);
            break;
         }
      }
      else
      {
         if (choice != 0)
            Database::globalMessage = "Pilihan tidak valid.";
      }
   }
}

void showManageStoreMenu()
{
   int choice = 0;
   while (true)
   {
      clearScreen();

      printHeader("Manajemen Toko: " + Database::loggedInSeller->getStoreName());
      Database::displayGlobalMessage();

      cout << "1. Daftarkan Item Baru\n";
      cout << "2. Update Item (Stok/Harga/Buang)\n";
      cout << "3. Lihat Semua Item Toko\n";
      cout << "4. Lihat Pesanan Menunggu Penyelesaian\n";
      cout << "--- Analisis Toko ---\n";
      cout << "5. Lihat Item Terpopuler\n";
      cout << "6. Lihat Pelanggan Loyal\n";
      cout << "7. Kembali\n";
      cout << "----------------------------------------\n";
      cout << "Pilihan Anda: ";
      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         Database::globalMessage = "Input tidak valid.";
         choice = 0;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      if (choice == 7)
      {
         return;
      }

      if (choice >= 1 && choice <= 6)
      {
         clearScreen();

         switch (choice)
         {
         case 1:
            Database::loggedInSeller->registerNewItem();
            break;
         case 2:
            Database::loggedInSeller->updateExistingItem();
            break;
         case 3:
            printHeader("Daftar Item di " + Database::loggedInSeller->getStoreName());
            Database::loggedInSeller->getStoreItems()->showAllItems();
            cout << "\nTekan [Enter] untuk kembali...";
            cin.get();
            break;
         case 4:
            Database::loggedInSeller->listPendingOrders(Database::transactionLog);
            cout << "\nTekan [Enter] untuk kembali...";
            cin.get();
            break;
         case 5:
            Database::loggedInSeller->showTopKItems(Database::transactionLog);
            break;
         case 6:
            Database::loggedInSeller->showLoyalCustomers(Database::transactionLog);
            break;
         }
      }
      else
      {
         if (choice != 0)
         {
            Database::globalMessage = "Pilihan tidak valid.";
         }
      }
   }
}

void showRegisterMenu()
{
   int choice = 0;
   while (true)
   {
      clearScreen();
      printHeader("Menu Registrasi");
      Database::displayGlobalMessage();

      cout << "1. Buat Akun Buyer\n";
      cout << "2. Kembali ke Menu Utama\n";
      cout << "----------------------------------------\n";
      cout << "Pilihan Anda: ";
      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         Database::globalMessage = "Input tidak valid. Harap masukkan angka.";
         choice = 0;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      switch (choice)
      {
      case 1:
         handleRegisterBuyer();
         break;
      case 2:
         return;
      default:
         if (choice != 0)
            Database::globalMessage = "Pilihan tidak valid.";
         break;
      }
   }
}

void showLoggedInMenu()
{
   int choice = 0;

   while (Database::loggedInBuyer)
   {
      clearScreen();
      printHeader("Selamat Datang, " + Database::loggedInBuyer->getName());
      Database::displayGlobalMessage();

      cout << "1. Cek Status Akun\n";
      cout << "2. Menu Pembeli\n";
      cout << "3. Menu Bank\n";
      cout << "4. Upgrade Akun ke Seller\n";

      int sellerMenuOption = 0;
      int logoutOption = 5;
      int exitOption = 6;

      if (Database::loggedInSeller)
      {
         sellerMenuOption = 5;
         cout << sellerMenuOption << ". Kelola Toko\n";
         logoutOption = 6;
         exitOption = 7;
      }

      cout << logoutOption << ". Logout\n";
      cout << exitOption << ". Keluar dari Program\n";
      cout << "----------------------------------------\n";
      cout << "Pilihan Anda: ";
      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         Database::globalMessage = "Input tidak valid.";
         choice = 0;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      if (choice == 2)
      {
         showBuyerMenu();
      }
      else if (choice == 3)
      {
         showBankMenu();
      }
      else if (Database::loggedInSeller && choice == sellerMenuOption)
      {
         showManageStoreMenu();
      }
      else if (choice == logoutOption)
      {
         handleLogout();
      }
      else if (choice == exitOption)
      {
         Serialization::saveAllData();
         cout << "Terima kasih!\n";
         exit(0);
      }
      else
      {
         switch (choice)
         {
         case 1:
            handleCheckStatus();
            break;
         case 4:
            handleUpgradeToSeller();
            break;
         default:
            if (choice != 0)
               Database::globalMessage = "Pilihan tidak valid.";
            break;
         }
      }
   }
}

// =======================================================
// Fungsi Utama Program
// =======================================================

int main()
{
   Serialization::loadAllData();

   if (Database::buyers.empty())
   {
      Database::seedDatabase();
   }

   int choice = 0;
   while (true)
   {
      if (Database::loggedInBuyer)
      {
         showLoggedInMenu();
      }
      else
      {
         clearScreen();
         printHeader("E-COMMERCE MINI");
         Database::displayGlobalMessage();

         cout << "1. Registrasi\n";
         cout << "2. Login\n";
         cout << "3. Keluar\n";
         cout << "----------------------------------------\n";
         cout << "Pilihan Anda: ";
         cin >> choice;

         if (cin.fail())
         {
            cin.clear();
            Database::globalMessage = "Input tidak valid. Harap masukkan angka.";
            choice = 0;
         }
         cin.ignore(numeric_limits<streamsize>::max(), '\n');

         switch (choice)
         {
         case 1:
            showRegisterMenu();
            break;
         case 2:
            handleLogin();
            break;
         case 3:
            Serialization::saveAllData();
            cout << "Terima kasih!\n";
            return 0;
         default:
            if (choice != 0)
               Database::globalMessage = "Pilihan tidak valid.";
            break;
         }
      }
   }
   return 0;
}