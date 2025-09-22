#include "./repo/bank.h"
#include "./repo/bankCustomer.h"
#include "./repo/items.h"
#include "./repo/buyer.h"

#include <iostream>
using namespace std;

int main()
{
   Bank bank;
   Items items;

   BankCustomer customer1(1, "John Doe", 1001, 100000);
   BankCustomer customer2(2, "Jane Angel", 1002, 250000);

   bank.addCustomer(customer1);
   bank.addCustomer(customer2);

   bank.showAllCustomers();

   items.addItem(Item(1, "Kopi", 1000, 10));
   items.addItem(Item(2, "Susu", 2000, 20));

   items.showAllItems();

   items.updateItem(1, Item(1, "Kopi Coklat", 1500, 15));

   items.showAllItems();

   // =======================================================

   Buyer buyer(&customer1);

   buyer.buyItem(items, 1, 2);

   cout << customer1.getName() << " new balance: " << customer1.getBalance() << endl;

   items.showAllItems();

   return 0;
}
