#ifndef ITEMS_H
#define ITEMS_H

#include "item.h"
#include <iostream>
#include <vector>
using namespace std;

class Items
{
private:
   vector<Item> items;

public:
   void addItem(const Item &item);
   vector<Item> &getItems();
   Item *findItemById(unsigned int itemId);
   void showAllItems();
   void removeItem(unsigned int itemId);
   void updateItem(unsigned int itemId, const Item &newItem);
};

#endif
