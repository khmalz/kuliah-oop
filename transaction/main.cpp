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

using namespace std;
using namespace chrono;

// =======================================================
// Handler untuk Buyer
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

   uint itemId;
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
      uint newTransactionId = Database::nextTransactionId++;
      Database::loggedInBuyer->buyItem(newTransactionId, targetSeller, *targetItems, itemId, quantity, Database::transactionLog);

      Database::globalMessage = "Pembelian berhasil! ID Transaksi Anda: " + to_string(newTransactionId);
   }
   catch (const runtime_error &e)
   {
      Database::globalMessage = "Error: " + string(e.what());
   }
}

void handleListOrders()
{
   clearScreen();
   printHeader("Riwayat Pesanan Anda");

   cout << "Menampilkan semua pesanan Anda:\n\n";

   bool hasOrders = false;
   for (const auto &record : Database::transactionLog)
   {
      if (record.buyerId == Database::loggedInBuyer->getId())
      {
         cout << "Toko     : " << record.sellerStoreName << "\n";
         cout << "Item     : " << record.itemName << " (x" << record.quantity << ")\n";
         cout << "Total    : Rp " << record.totalPrice << "\n";
         cout << "Status   : " << statusToString(record.status) << "\n";
         cout << "----------------------------------------\n";
         hasOrders = true;
      }
   }

   if (!hasOrders)
   {
      cout << "Anda belum memiliki riwayat pesanan.\n";
   }

   cout << "\nTekan [Enter] untuk kembali...";
   cin.get();
}

void handleCheckSpending()
{
   clearScreen();
   printHeader("Analisis Pengeluaran");

   int k_days;
   cout << "Cek total pengeluaran dalam (k) hari terakhir.\n";
   cout << "Masukkan jumlah hari (k) (ketik 0 untuk batal): ";
   cin >> k_days;
   if (k_days == 0)
   {
      Database::globalMessage = "Pencarian dibatalkan.";
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      return;
   }

   double totalSpending = 0;
   auto now = system_clock::now();
   auto k_days_ago = now - hours(24 * k_days);

   for (const auto &record : Database::transactionLog)
   {
      if (record.buyerId == Database::loggedInBuyer->getId() && record.transactionDate >= k_days_ago)
      {
         totalSpending += record.totalPrice;
      }
   }

   cout << "\nTotal pengeluaran Anda dalam " << k_days << " hari terakhir adalah: Rp " << totalSpending << "\n";

   cout << "\nTekan [Enter] untuk kembali...";
   cin.ignore(numeric_limits<streamsize>::max(), '\n');
   cin.get();
}

void handleConfirmReceipt()
{
   clearScreen();
   printHeader("Konfirmasi Penerimaan Barang");

   bool hasPaidOrders = false;
   cout << "Pesanan Anda yang Menunggu Konfirmasi (Status PAID):\n";
   cout << "--------------------------------------------------------\n";
   for (const auto &record : Database::transactionLog)
   {
      if (record.buyerId == Database::loggedInBuyer->getId() && record.status == PAID)
      {
         cout << "ID Transaksi: " << record.transactionId << "\n";
         cout << "Item        : " << record.itemName << " (x" << record.quantity << ")\n";
         cout << "Dari Toko   : " << record.sellerStoreName << "\n\n";
         hasPaidOrders = true;
      }
   }

   if (!hasPaidOrders)
   {
      cout << "Anda tidak memiliki pesanan yang menunggu konfirmasi.\n";
      cout << "\nTekan [Enter] untuk kembali...";
      cin.get();
      return;
   }
   cout << "--------------------------------------------------------\n";

   uint idToConfirm;
   cout << "Masukkan ID Transaksi yang ingin dikonfirmasi (ketik 0 untuk batal): ";
   cin >> idToConfirm;
   cin.ignore(numeric_limits<streamsize>::max(), '\n');

   if (idToConfirm == 0)
   {
      Database::globalMessage = "Aksi dibatalkan.";
      return;
   }

   for (auto &record : Database::transactionLog)
   {
      if (record.transactionId == idToConfirm && record.buyerId == Database::loggedInBuyer->getId() && record.status == PAID)
      {
         record.status = COMPLETED;
         Database::globalMessage = "Terima kasih! Transaksi #" + to_string(idToConfirm) + " telah ditandai selesai.";
         return;
      }
   }

   Database::globalMessage = "Error: ID Transaksi tidak ditemukan atau sudah dikonfirmasi.";
}

void handleCancelOrder()
{
   clearScreen();
   printHeader("Batalkan Pesanan");

   bool hasPaidOrders = false;
   cout << "Pesanan Anda yang bisa dibatalkan (Status PAID):\n";
   cout << "--------------------------------------------------------\n";
   for (const auto &record : Database::transactionLog)
   {
      if (record.buyerId == Database::loggedInBuyer->getId() && record.status == PAID)
      {
         cout << "ID Transaksi: " << record.transactionId << "\n";
         cout << "Item        : " << record.itemName << " (x" << record.quantity << ")\n";
         cout << "Dari Toko   : " << record.sellerStoreName << "\n\n";
         hasPaidOrders = true;
      }
   }

   if (!hasPaidOrders)
   {
      cout << "Anda tidak memiliki pesanan yang bisa dibatalkan.\n";
      cout << "\nTekan [Enter] untuk kembali...";
      cin.get();
      return;
   }
   cout << "--------------------------------------------------------\n";

   uint idToCancel;
   cout << "Masukkan ID Transaksi yang ingin dibatalkan (ketik 0 untuk batal): ";
   cin >> idToCancel;
   cin.ignore(numeric_limits<streamsize>::max(), '\n');

   if (idToCancel == 0)
   {
      Database::globalMessage = "Aksi dibatalkan.";
      return;
   }

   for (auto &record : Database::transactionLog)
   {
      if (record.transactionId == idToCancel && record.buyerId == Database::loggedInBuyer->getId() && record.status == PAID)
      {
         Seller *sellerOfItem = nullptr;
         for (auto &s : Database::sellers)
         {
            if (s.getBuyer()->getId() == record.sellerId)
            {
               sellerOfItem = &s;
               break;
            }
         }

         if (sellerOfItem)
         {
            Item *itemToRestock = sellerOfItem->getStoreItems()->findItemById(record.itemId);
            if (itemToRestock)
            {
               itemToRestock->increaseQuantity(record.quantity);
            }
         }

         BankCustomer *buyerAccount = Database::loggedInBuyer->getCustomer();
         if (sellerOfItem)
         {
            BankCustomer *sellerAccount = sellerOfItem->getCustomerAccount();
            sellerAccount->withdraw(record.totalPrice);
            buyerAccount->deposit(record.totalPrice);
         }

         record.status = CANCELED;
         Database::globalMessage = "Transaksi #" + to_string(idToCancel) + " berhasil dibatalkan dan dana telah dikembalikan.";
         return;
      }
   }

   Database::globalMessage = "Error: ID Transaksi tidak ditemukan atau statusnya bukan PAID.";
}

// =======================================================
// Handler untuk Action
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

   uint newId = Database::buyers.size() + 1;
   Database::buyers.emplace_back(newId, name, email, initialDeposit);

   Database::mainBank.addCustomer(*(Database::buyers.back().getCustomer()));

   Database::globalMessage = "Registrasi berhasil! ID Buyer Anda adalah " + to_string(newId);
}

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

void handleLogin()
{
   if (Database::buyers.empty())
   {
      Database::globalMessage = "Belum ada buyer terdaftar. Silakan registrasi dahulu.";
      return;
   }

   clearScreen();
   printHeader("Login");

   uint id;
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

void handleCheckStatus()
{
   clearScreen();
   printHeader("Status Akun");
   BankCustomer *customer = Database::loggedInBuyer->getCustomer();

   cout << "ID       : " << Database::loggedInBuyer->getId() << "\n";
   cout << "Nama     : " << Database::loggedInBuyer->getName() << "\n";
   cout << "Email    : " << Database::loggedInBuyer->getEmail() << "\n";
   cout << "Saldo    : Rp " << customer->getBalance() << "\n";
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

// =======================================================
// Handler untuk Bank
// =======================================================

void handleListAllBankCustomers()
{
   clearScreen();
   printHeader("Daftar Semua Nasabah Bank");
   Database::mainBank.showAllCustomers();
   cout << "\nTekan [Enter] untuk kembali...";
   cin.get();
}

void handleListRecentBankTransactions()
{
   clearScreen();
   Database::mainBank.listRecentTransactions(Database::transactionLog);
   cout << "\nTekan [Enter] untuk kembali...";
   cin.get();
}

void handleListDormantAccounts()
{
   clearScreen();
   Database::mainBank.listDormantAccounts(Database::transactionLog);
   cout << "\nTekan [Enter] untuk kembali...";
   cin.get();
}

void handleListTopUsers()
{
   clearScreen();
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
   cout << "\nTekan [Enter] untuk kembali...";
   cin.get();
}

// =======================================================
// Hadnler untuk Menampilkan Menu-Menu
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
      cout << "2. Tampilkan Transaksi (1 Minggu Terakhir)\n";
      cout << "3. Tampilkan Akun Dormant (>30 Hari)\n";
      cout << "4. Tampilkan Top Pengguna Hari Ini\n";
      cout << "5. Kembali ke Menu Utama\n";
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

      switch (choice)
      {
      case 1:
         handleListAllBankCustomers();
         break;
      case 2:
         handleListRecentBankTransactions();
         break;
      case 3:
         handleListDormantAccounts();
         break;
      case 4:
         handleListTopUsers();
         break;
      case 5:
         return;
      default:
         if (choice != 0)
            Database::globalMessage = "Pilihan tidak valid.";
         break;
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
      cout << "3. Cek Pengeluaran Terakhir\n";
      cout << "4. Konfirmasi Penerimaan Barang\n";
      cout << "5. Batalkan Pesanan\n";
      cout << "6. Kembali ke Menu Utama\n";
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

      switch (choice)
      {
      case 1:
         handlePurchaseItem();
         break;
      case 2:
         handleListOrders();
         break;
      case 3:
         handleCheckSpending();
         break;
      case 4:
         handleConfirmReceipt();
         break;
      case 5:
         handleCancelOrder();
         break;
      case 6:
         return;
      default:
         if (choice != 0)
            Database::globalMessage = "Pilihan tidak valid.";
         break;
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
      cout << "--- Analisis Toko ---\n";
      cout << "4. Lihat Item Terpopuler\n";
      cout << "5. Lihat Pelanggan Loyal\n";
      cout << "6. Kembali\n";
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

      if (choice == 6)
      {
         return;
      }

      if (choice >= 1 && choice <= 5)
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
            Database::loggedInSeller->showTopKItems(Database::transactionLog);
            break;
         case 5:
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