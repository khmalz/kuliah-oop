#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <map>
#include <algorithm>
#include <ctime>

#include "repo/buyer.h"
#include "repo/seller.h"
#include "repo/items.h"
#include "repo/transaction.h"

using namespace std;

// =======================================================
// "Database" & State Management Global
// =======================================================
vector<Buyer> buyers;
vector<Seller> sellers;

Buyer *loggedInBuyer = nullptr;
Seller *loggedInSeller = nullptr;
vector<Transaction> transactionLog;

string globalMessage = "";
uint nextItemId = 1;

// =======================================================
// Fungsi Utilitas Tampilan
// =======================================================
void clearScreen()
{
#if defined(__unix__) || defined(__APPLE__)
   cout << "\033[2J\033[1;1H";
#elif defined(_WIN32)
   system("cls");
#else
   for (int i = 0; i < 50; ++i)
      cout << endl;
#endif
}

void printHeader(const string &title)
{
   cout << "========================================\n";
   cout << "     " << title << "\n";
   cout << "========================================\n\n";
}

string statusToString(OrderStatus status)
{
   switch (status)
   {
   case PAID:
      return "Paid";
   case COMPLETED:
      return "Completed";
   case CANCELED:
      return "Canceled";
   default:
      return "Unknown";
   }
}

// FUNGSI BARU: Untuk menampilkan dan menghapus pesan global
void displayGlobalMessage()
{
   if (!globalMessage.empty())
   {
      cout << "----------------------------------------\n";
      cout << ">> " << globalMessage << "\n";
      cout << "----------------------------------------\n\n";
      globalMessage = ""; // Hapus pesan setelah ditampilkan
   }
}

// =======================================================
// Handler untuk Aksi-Aksi Toko (BARU)
// =======================================================

void handleRegisterNewItem()
{
   clearScreen();
   printHeader("Daftarkan Item Baru");

   string name;
   double price;
   int quantity;

   cout << "Nama Item      : ";
   getline(cin, name);
   cout << "Harga per Item : Rp ";
   cin >> price;
   cout << "Jumlah Stok    : ";
   cin >> quantity;

   // Ambil inventaris milik seller yang login
   Items *store = loggedInSeller->getStoreItems();

   // Buat item baru dengan ID unik
   Item newItem(nextItemId++, name, price, quantity);
   store->addItem(newItem);

   globalMessage = "Item '" + name + "' berhasil didaftarkan!";
}

void handleUpdateExistingItem()
{
   clearScreen();
   printHeader("Update Item");

   Items *store = loggedInSeller->getStoreItems();
   if (store->getItems().empty())
   {
      globalMessage = "Anda belum memiliki item untuk diupdate.";
      return;
   }

   cout << "Item yang Anda miliki:\n";
   store->showAllItems();
   cout << "----------------------------------------\n";

   uint itemId;
   cout << "Masukkan ID item yang ingin diupdate: ";
   cin >> itemId;
   cin.ignore(numeric_limits<streamsize>::max(), '\n');

   Item *itemToUpdate = store->findItemById(itemId);
   if (!itemToUpdate)
   {
      globalMessage = "Item dengan ID " + to_string(itemId) + " tidak ditemukan.";
      return;
   }

   // Sub-menu untuk update
   clearScreen();
   printHeader("Mengupdate: " + itemToUpdate->getName());
   cout << "1. Tambah Stok (Replenish)\n";
   cout << "2. Ubah Harga\n";
   cout << "3. Buang Item (Discard)\n";
   cout << "Pilihan Anda: ";
   int choice;
   cin >> choice;
   cin.ignore(numeric_limits<streamsize>::max(), '\n');

   switch (choice)
   {
   case 1:
   {
      int amount;
      cout << "Jumlah stok yang ingin ditambahkan: ";
      cin >> amount;
      itemToUpdate->increaseQuantity(amount);
      globalMessage = "Stok berhasil ditambahkan.";
      break;
   }
   case 2:
   {
      double newPrice;
      cout << "Harga baru: Rp ";
      cin >> newPrice;
      itemToUpdate->setPrice(newPrice);
      globalMessage = "Harga berhasil diubah.";
      break;
   }
   case 3:
   {
      store->removeItem(itemId);
      globalMessage = "Item berhasil dibuang.";
      break;
   }
   default:
      globalMessage = "Pilihan update tidak valid.";
      break;
   }
}

pair<int, int> getMonthYear(const chrono::system_clock::time_point &timepoint)
{
   time_t time = chrono::system_clock::to_time_t(timepoint);
   tm *localTime = localtime(&time);
   return {localTime->tm_mon + 1, localTime->tm_year + 1900};
}

void handleTopKItems()
{
   clearScreen();
   printHeader("Item Terpopuler per Bulan");

   int month, year, k;
   cout << "Masukkan Bulan (1-12): ";
   cin >> month;
   cout << "Masukkan Tahun (cth: 2025): ";
   cin >> year;
   cout << "Berapa item teratas (K): ";
   cin >> k;

   map<uint, int> itemPopularity;
   for (const auto &record : transactionLog)
   {
      if (record.sellerId == loggedInSeller->getBuyer()->getId())
      {
         pair<int, int> recordDate = getMonthYear(record.transactionDate);
         if (recordDate.first == month && recordDate.second == year)
         {
            itemPopularity[record.itemId] += record.quantity;
         }
      }
   }

   if (itemPopularity.empty())
   {
      globalMessage = "Tidak ada data penjualan untuk periode tersebut.";
      return;
   }

   vector<pair<uint, int>> sortedItems(itemPopularity.begin(), itemPopularity.end());
   sort(sortedItems.begin(), sortedItems.end(), [](const auto &a, const auto &b)
        { return a.second > b.second; });

   clearScreen();
   printHeader("Top " + to_string(k) + " Item Populer");
   cout << "Periode: " << month << "/" << year << endl
        << endl;
   int count = 0;
   for (const auto &pair : sortedItems)
   {
      if (count++ >= k)
         break;
      Item *item = loggedInSeller->getStoreItems()->findItemById(pair.first);
      if (item)
      {
         cout << count << ". " << item->getName()
              << " (ID: " << pair.first << ") - Terjual: "
              << pair.second << " unit\n";
      }
   }

   cout << "\nTekan [Enter] untuk kembali...";
   cin.ignore(numeric_limits<streamsize>::max(), '\n');
   cin.get();
}

void handleLoyalCustomers()
{
   clearScreen();
   printHeader("Pelanggan Loyal per Bulan");

   int month, year;
   cout << "Masukkan Bulan (1-12): ";
   cin >> month;
   cout << "Masukkan Tahun (cth: 2025): ";
   cin >> year;

   map<uint, int> customerFrequency;
   for (const auto &record : transactionLog)
   {
      if (record.sellerId == loggedInSeller->getBuyer()->getId())
      {
         pair<int, int> recordDate = getMonthYear(record.transactionDate);
         if (recordDate.first == month && recordDate.second == year)
         {
            customerFrequency[record.buyerId]++;
         }
      }
   }

   if (customerFrequency.empty())
   {
      globalMessage = "Tidak ada data pelanggan untuk periode tersebut.";
      return;
   }

   vector<pair<uint, int>> sortedCustomers(customerFrequency.begin(), customerFrequency.end());
   sort(sortedCustomers.begin(), sortedCustomers.end(), [](const auto &a, const auto &b)
        { return a.second > b.second; });

   clearScreen();
   printHeader("Top 3 Pelanggan Loyal");
   cout << "Periode: " << month << "/" << year << endl
        << endl;
   int count = 0;
   for (const auto &pair : sortedCustomers)
   {
      if (count++ >= 3)
         break;
      Buyer *buyer = nullptr;
      for (auto &b : buyers)
      {
         if (b.getId() == pair.first)
            buyer = &b;
      }

      if (buyer)
      {
         cout << count << ". " << buyer->getName()
              << " (ID: " << pair.first << ") - Jumlah Transaksi: "
              << pair.second << "\n";
      }
   }

   cout << "\nTekan [Enter] untuk kembali...";
   cin.ignore(numeric_limits<streamsize>::max(), '\n');
   cin.get();
}

// =======================================================
// Menu Baru Khusus Seller
// =======================================================
void showManageStoreMenu()
{
   int choice = 0;
   while (true)
   {
      clearScreen();
      printHeader("Manajemen Toko: " + loggedInSeller->getStoreName());
      displayGlobalMessage();

      cout << "1. Daftarkan Item Baru\n";
      cout << "2. Update Item (Stok/Harga/Buang)\n";
      cout << "3. Lihat Semua Item Toko\n";
      cout << "--- Analisis Toko ---\n";
      cout << "4. Lihat Item Terpopuler\n"; // <-- BARU
      cout << "5. Lihat Pelanggan Loyal\n"; // <-- BARU
      cout << "6. Kembali\n";               // <-- Nomor disesuaikan
      cout << "----------------------------------------\n";
      cout << "Pilihan Anda: ";
      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         globalMessage = "Input tidak valid.";
         choice = 0;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      switch (choice)
      {
      case 1:
         handleRegisterNewItem();
         break;
      case 2:
         handleUpdateExistingItem();
         break;
      case 3:
         clearScreen();
         printHeader("Daftar Item di " + loggedInSeller->getStoreName());
         loggedInSeller->getStoreItems()->showAllItems();
         cout << "\nTekan [Enter] untuk kembali...";
         cin.get();
         break;
      case 4:
         handleTopKItems();
         break;
      case 5:
         handleLoyalCustomers();
         break;
      case 6:
         return;
      default:
         if (choice != 0)
            globalMessage = "Pilihan tidak valid.";
         break;
      }
   }
}

// =======================================================
// Handler untuk Aksi-Aksi Buyer
// =======================================================

void handlePurchaseItem()
{
   clearScreen();
   printHeader("Beli Barang");

   if (sellers.empty())
   {
      globalMessage = "Maaf, belum ada toko yang buka saat ini.";
      return;
   }

   cout << "Daftar Barang yang Tersedia:\n";
   cout << "--------------------------------------------------------\n";
   for (auto &seller : sellers)
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

   cout << "Masukkan ID Item yang ingin dibeli: ";
   cin >> itemId;
   cout << "Masukkan Jumlah: ";
   cin >> quantity;

   Seller *targetSeller = nullptr;
   Items *targetItems = nullptr;

   for (auto &seller : sellers)
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
      globalMessage = "Item dengan ID " + to_string(itemId) + " tidak ditemukan di toko mana pun.";
      return;
   }

   try
   {
      loggedInBuyer->buyItem(targetSeller, *targetItems, itemId, quantity, transactionLog);
      globalMessage = "Pembelian berhasil!";
   }
   catch (const runtime_error &e)
   {
      globalMessage = "Error: " + string(e.what());
   }
}

void handleListOrders()
{
   clearScreen();
   printHeader("Riwayat Pesanan Anda");

   cout << "Menampilkan semua pesanan Anda:\n\n";

   bool hasOrders = false;
   for (const auto &record : transactionLog)
   {
      if (record.buyerId == loggedInBuyer->getId())
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
   cout << "Masukkan jumlah hari (k): ";
   cin >> k_days;

   double totalSpending = 0;
   auto now = chrono::system_clock::now();
   auto k_days_ago = now - chrono::hours(24 * k_days);

   for (const auto &record : transactionLog)
   {
      if (record.buyerId == loggedInBuyer->getId() && record.transactionDate >= k_days_ago)
      {
         totalSpending += record.totalPrice;
      }
   }

   cout << "\nTotal pengeluaran Anda dalam " << k_days << " hari terakhir adalah: Rp " << totalSpending << "\n";

   cout << "\nTekan [Enter] untuk kembali...";
   cin.ignore(numeric_limits<streamsize>::max(), '\n');
   cin.get();
}

// =======================================================
// Handler untuk Aksi-Aksi (sekarang menggunakan globalMessage)
// =======================================================

void handleRegisterBuyer()
{
   clearScreen();
   printHeader("Registrasi Akun Buyer Baru");

   string name, email;
   double initialDeposit = 0;

   cout << "Masukkan Nama Lengkap  : ";
   getline(cin, name);

   cout << "Masukkan Email         : ";
   getline(cin, email);

   cout << "Masukkan Deposit Awal  : Rp ";
   while (!(cin >> initialDeposit) || initialDeposit < 0)
   {
      cout << "Input tidak valid. Masukkan angka positif: Rp ";
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
   }

   uint newId = buyers.size() + 1;
   buyers.emplace_back(newId, name, email, initialDeposit);

   // Simpan pesan ke "papan pesan" global
   globalMessage = "Registrasi berhasil! ID Buyer Anda adalah " + to_string(newId);
}

void handleUpgradeToSeller()
{
   if (!loggedInBuyer)
   {
      globalMessage = "Error: Tidak ada user yang login.";
      return;
   }
   if (loggedInSeller)
   {
      globalMessage = "Akun Anda sudah menjadi Seller!";
      return;
   }

   clearScreen();
   printHeader("Upgrade Akun ke Seller");

   string storeName, storeAddress, storeEmail;
   cout << "Data Buyer: " << loggedInBuyer->getName() << "\n";
   cout << "Silakan lengkapi informasi toko Anda.\n\n";

   cout << "Nama Toko      : ";
   getline(cin, storeName);

   cout << "Alamat Toko    : ";
   getline(cin, storeAddress);

   cout << "Email Toko     : ";
   getline(cin, storeEmail);

   sellers.emplace_back(loggedInBuyer, storeName, storeAddress, storeEmail);
   loggedInSeller = &sellers.back();

   globalMessage = "Upgrade berhasil! Anda sekarang adalah Seller.";
}

void handleLogin()
{
   if (buyers.empty())
   {
      globalMessage = "Belum ada buyer terdaftar. Silakan registrasi dahulu.";
      return;
   }

   clearScreen();
   printHeader("Login");

   uint id;
   cout << "Masukkan ID Buyer Anda: ";
   cin >> id;

   for (auto &buyer : buyers)
   {
      if (buyer.getId() == id)
      {
         loggedInBuyer = &buyer;
         loggedInSeller = nullptr;
         for (auto &seller : sellers)
         {
            if (seller.getBuyer()->getId() == id)
            {
               loggedInSeller = &seller;
               break;
            }
         }
         globalMessage = "Login berhasil! Selamat datang, " + loggedInBuyer->getName() + ".";
         return;
      }
   }
   globalMessage = "Login gagal. ID Buyer tidak ditemukan.";
}

void handleLogout()
{
   globalMessage = "Anda telah logout. Sampai jumpa, " + loggedInBuyer->getName() + "!";
   loggedInBuyer = nullptr;
   loggedInSeller = nullptr;
}

void handleCheckStatus()
{
   clearScreen();
   printHeader("Status Akun");
   BankCustomer *customer = loggedInBuyer->getCustomer();

   cout << "ID       : " << loggedInBuyer->getId() << "\n";
   cout << "Nama     : " << loggedInBuyer->getName() << "\n";
   cout << "Email    : " << loggedInBuyer->getEmail() << "\n";
   cout << "Saldo    : Rp " << customer->getBalance() << "\n";
   cout << "Status   : ";
   if (loggedInSeller)
   {
      cout << "Seller & Buyer\n";
      cout << "Nama Toko: " << loggedInSeller->getStoreName() << "\n";
   }
   else
   {
      cout << "Buyer\n";
   }

   cout << "\nTekan [Enter] untuk kembali...";
   cin.get();
}

// =======================================================
// Fungsi untuk Menampilkan Menu-Menu
// =======================================================

void showBuyerMenu()
{
   int choice = 0;
   while (true)
   {
      clearScreen();
      printHeader("Menu Pembeli");
      displayGlobalMessage();

      cout << "1. Beli Barang\n";
      cout << "2. Lihat Riwayat Pesanan\n";
      cout << "3. Cek Pengeluaran Terakhir\n";
      cout << "4. Kembali ke Menu Utama\n";
      cout << "----------------------------------------\n";
      cout << "Pilihan Anda: ";
      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         globalMessage = "Input tidak valid.";
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
         return;
      default:
         if (choice != 0)
            globalMessage = "Pilihan tidak valid.";
         break;
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
      displayGlobalMessage();

      cout << "1. Buat Akun Buyer\n";
      cout << "2. Kembali ke Menu Utama\n";
      cout << "----------------------------------------\n";
      cout << "Pilihan Anda: ";
      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         globalMessage = "Input tidak valid. Harap masukkan angka.";
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
            globalMessage = "Pilihan tidak valid.";
         break;
      }
   }
}

void showLoggedInMenu()
{
   int choice = 0;

   while (loggedInBuyer)
   {
      clearScreen();
      printHeader("Selamat Datang, " + loggedInBuyer->getName());
      displayGlobalMessage();

      cout << "1. Cek Status Akun\n";
      cout << "2. Menu Pembeli\n";
      cout << "3. Upgrade Akun ke Seller\n";

      int sellerMenuOption = 0;
      int logoutOption = 4;
      int exitOption = 5;

      if (loggedInSeller)
      {
         sellerMenuOption = 4;
         cout << sellerMenuOption << ". Kelola Toko\n";
         logoutOption = 5;
         exitOption = 6;
      }

      cout << logoutOption << ". Logout\n";
      cout << exitOption << ". Keluar dari Program\n";
      cout << "----------------------------------------\n";
      cout << "Pilihan Anda: ";
      cin >> choice;

      if (cin.fail())
      {
         cin.clear();
         globalMessage = "Input tidak valid.";
         choice = 0;
      }
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      if (choice == 2)
      {
         showBuyerMenu();
      }
      else if (loggedInSeller && choice == sellerMenuOption)
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
         case 3:
            handleUpgradeToSeller();
            break;
         default:
            if (choice != 0)
               globalMessage = "Pilihan tidak valid.";
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
      if (loggedInBuyer)
      {
         showLoggedInMenu();
      }
      else
      {
         clearScreen();
         printHeader("E-COMMERCE MINI");
         displayGlobalMessage();

         cout << "1. Registrasi\n";
         cout << "2. Login\n";
         cout << "3. Keluar\n";
         cout << "----------------------------------------\n";
         cout << "Pilihan Anda: ";
         cin >> choice;

         if (cin.fail())
         {
            cin.clear();
            globalMessage = "Input tidak valid. Harap masukkan angka.";
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
               globalMessage = "Pilihan tidak valid.";
            break;
         }
      }
   }
   return 0;
}