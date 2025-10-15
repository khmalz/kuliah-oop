#include <iostream>
#include <limits>
#include <vector>

#include "user.h"

using namespace std;

User *loggedInUser = nullptr;

vector<User> users;
vector<User> contacts;
string globalMessage = "";

// Helper Functions

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

void displayGlobalMessage()
{
   if (!globalMessage.empty())
   {
      cout << "----------------------------------------\n";
      cout << ">> " << globalMessage << "\n";
      cout << "----------------------------------------\n\n";

      globalMessage = "";
   }
}

// Main Code
void loginUser()
{
   clearScreen();

   cout << "----------------------------------------\n";
   cout << ">> " << "Login User" << "\n";
   cout << "----------------------------------------\n\n";

   string phone_number;

   cout << "Masukkan Nomor Telepon (ketik 0 untuk batal): ";
   getline(cin, phone_number);
   if (phone_number == "0")
   {
      globalMessage = "Login dibatalkan.";
      return;
   }

   for (auto &user : users)
   {
      if (user.getPhoneNumber() == phone_number)
      {
         loggedInUser = &user;
         globalMessage = "Login berhasil!";
         return;
      }
   }

   globalMessage = "Login gagal. Nomor telepon tidak ditemukan.";
}

void registerUser()
{
   clearScreen();

   cout << "----------------------------------------\n";
   cout << ">> " << "Register New User" << "\n";
   cout << "----------------------------------------\n\n";

   string name, phone_number;

   cout << "Masukkan Nama  (ketik 0 untuk batal): ";
   getline(cin, name);
   if (name == "0")
   {
      globalMessage = "Registrasi dibatalkan.";
      return;
   }

   cout << "Masukkan Nomor Telepon: ";
   getline(cin, phone_number);

   bool isDuplicate = false;
   for (auto &user : users)
   {
      if (user.getPhoneNumber() == phone_number)
      {
         isDuplicate = true;
         break;
      }
   }

   if (isDuplicate)
   {
      globalMessage = "Registrasi gagal. Nomor telepon sudah terdaftar.";
      return;
   }

   uint newId = users.size() + 1;
   users.emplace_back(newId, name, phone_number);

   globalMessage = "Akun '" + name + "' berhasil didaftarkan!";
}

void handleCheckStatus()
{
   clearScreen();
   cout << "----------------------------------------\n";
   cout << ">> " << "Status Akun" << "\n";
   cout << "----------------------------------------\n\n";

   cout << "ID              : " << loggedInUser->getId() << "\n";
   cout << "Nama            : " << loggedInUser->getName() << "\n";
   cout << "Phone Number    : " << loggedInUser->getPhoneNumber() << "\n";

   cout << "\nTekan [Enter] untuk kembali...";
   cin.get();
}

void handleAddContact()
{
   clearScreen();
   cout << "----------------------------------------\n";
   cout << ">> " << "Tambah Kontak" << "\n";
   cout << "----------------------------------------\n\n";

   string name, phone_number;

   cout << "Masukkan Nama Kontak (ketik 0 untuk batal): ";
   getline(cin, name);
   if (name == "0")
   {
      globalMessage = "Pendaftaran kontak dibatalkan.";
      return;
   }

   cout << "Masukkan Nomor Telepon Kontak: ";
   getline(cin, phone_number);

   for (auto &user : users)
   {
      if (user.getPhoneNumber() == phone_number)
      {
         contacts.push_back(user);
         globalMessage = "Kontak '" + name + "' berhasil ditambahkan!";
         return;
      }
   }

   globalMessage = "Kontak gagal ditambahkan. Nomor telepon tidak ditemukan.";
}

void handleListContacts()
{
   clearScreen();

   cout << "----------------------------------------\n";
   cout << ">> " << "Daftar Kontak" << "\n";
   cout << "----------------------------------------\n\n";

   if (contacts.empty())
   {
      cout << "Belum ada kontak.\n";
      cout << "\nTekan [Enter] untuk kembali...";
      cin.get();

      return;
   }

   for (auto &contact : contacts)
   {
      cout << "ID              : " << contact.getId() << "\n";
      cout << "Nama            : " << contact.getName() << "\n";
      cout << "Phone Number    : " << contact.getPhoneNumber() << "\n";
      cout << "----------------------------------------\n\n";
   }

   cout << "\nTekan [Enter] untuk kembali...";
   cin.get();
}

void handleLogout()
{
   clearScreen();
   cout << "----------------------------------------\n";
   cout << ">> " << "Logout" << "\n";
   cout << "----------------------------------------\n\n";

   char choice;
   while (true)
   {
      cout << "Apakah Anda yakin ingin logout? (y/n): ";
      cin >> choice;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      choice = tolower(choice);

      if (choice == 'y')
      {
         string name = loggedInUser->getName();

         loggedInUser = nullptr;
         contacts.clear();

         globalMessage = "Anda telah logout. Sampai jumpa, " + name + "!";
         return;
      }
      else if (choice == 'n')
      {
         globalMessage = "Logout dibatalkan.";
         return;
      }
      else
      {
         cout << "Input tidak valid. Harap masukkan 'y' atau 'n'.\n";
      }
   }
}

void showLoggedInMenu()
{
   int choice = 0;

   while (loggedInUser)
   {
      clearScreen();

      cout << "----------------------------------------\n";
      cout << ">> " << "Selamat Datang, " + loggedInUser->getName() << "\n";
      cout << "----------------------------------------\n\n";

      displayGlobalMessage();

      cout << "1. Cek Status Akun\n";
      cout << "2. Tambahkan Kontak\n";
      cout << "3. List Kontak\n";

      int logoutOption = 4;
      int exitOption = 5;

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

      if (choice == logoutOption)
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
         case 2:
            handleAddContact();
            break;
         case 3:
            handleListContacts();
            break;
         default:
            if (choice != 0)
               globalMessage = "Pilihan tidak valid.";
            break;
         }
      }
   }
}

int main()
{
   int choice = 0;

   while (true)
   {
      if (loggedInUser)
      {
         showLoggedInMenu();
      }
      else
      {

         clearScreen();

         cout << "=======================================\n";
         cout << "Simple Messaging Application" << "\n";
         cout << "=======================================\n\n";

         displayGlobalMessage();

         cout << "1. Login\n";
         cout << "2. Register\n";
         cout << "3. Exit\n";
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
         {
            loginUser();

            break;
         }
         case 2:
         {
            registerUser();

            break;
         }
         case 3:
            return 0;
         default:
            globalMessage = "Pilihan tidak valid.";
            break;
         }
      }
   }

   return 0;
}
