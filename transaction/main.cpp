#include <iostream>
#include <vector>
#include <string>
#include <limits>

#include "repo/buyer.h"
#include "repo/seller.h"
#include "repo/items.h"

using namespace std;

// =======================================================
// "Database" & State Management Global
// =======================================================
vector<Buyer> buyers;
vector<Seller> sellers;
Items allItems;

Buyer *loggedInBuyer = nullptr;
Seller *loggedInSeller = nullptr;

// VARIABEL BARU: "Papan Pesan" untuk menampilkan pesan di layar berikutnya
string globalMessage = "";

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
      cout << "2. Upgrade Akun ke Seller\n";
      cout << "3. Logout\n";
      cout << "4. Keluar dari Program\n";
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
         handleCheckStatus();
         break;
      case 2:
         handleUpgradeToSeller();
         break;
      case 3:
         handleLogout();
         break;
      case 4:
         cout << "Terima kasih!\n";
         exit(0);
      default:
         if (choice != 0)
            globalMessage = "Pilihan tidak valid.";
         break;
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