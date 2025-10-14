#include "repo/transaction.h"

#include <iostream>

using namespace std;

inline void clearScreen()
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

inline void printHeader(const string &title)
{
   cout << "========================================\n";
   cout << "     " << title << "\n";
   cout << "========================================\n\n";
}

inline string statusToString(OrderStatus status)
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

inline string maskBankId(uint id)
{
   string idStr = to_string(id);
   if (idStr.length() > 2)
   {
      return idStr.substr(0, 2) + string(idStr.length() - 2, 'x');
   }
   return idStr;
}