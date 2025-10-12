#include "repo/transaction.h"

#include <iostream>

using namespace std;

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
