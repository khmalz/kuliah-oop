#pragma once
#include "buyer.h"
#include "item.h"
#include "order.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Seller : public Buyer
{
private:
    int sellerId;
    string sellerName, storeName, storeAddress, storePhone, storeEmail;
    vector<Item> items;
    vector<Order> orders;
    bool idDisplayed(int itemId) const
    {
        // Example implementation, can be customized
        return itemId > 0; // Assuming valid IDs are positive integers
    }

public:
    Seller() = default;

    Seller(Buyer buyer, int sellerId,
           const string &sellerName,
           const string &storeName,
           const string &storeAddress,
           const string &storePhone,
           const string &storeEmail)
        : Buyer(buyer.getId(),
                buyer.getName(),
                buyer.getAddress(),
                buyer.getPhone(),
                buyer.getEmail()),
          sellerId(sellerId), sellerName(sellerName), storeName(storeName),
          storeAddress(storeAddress), storePhone(storePhone), storeEmail(storeEmail)
    {
        Buyer::setId(buyer.getId());
    }

    virtual ~Seller() = default;

    void addOrder(const Order &order)
    {
        orders.push_back(order);
    }

    void addNewItem(int newId, const string &newName, int newQuantity, double newPrice)
    {
        Item newItem(newId, newName, newQuantity, newPrice);
        newItem.setDisplay(true);
        items.push_back(newItem);
        cout << "Item '" << newName << "' added to inventory successfully.\n";
    }

    bool removeItem(int itemId)
    {
        for (auto it = items.begin(); it != items.end(); ++it)
        {
            if (it->getId() == itemId)
            {
                cout << "Item '" << it->getName() << "' removed from inventory.\n";
                items.erase(it);
                return true;
            }
        }
        cout << "Item not found.\n";
        return false;
    }

    void checkInventory() const
    {
        cout << "\n=== Inventory for " << storeName << " ===\n";
        if (items.empty())
        {
            cout << "No items in inventory.\n";
            return;
        }

        for (const auto &item : items)
        {
            cout << "ID: " << item.getId()
                 << " | Name: " << item.getName()
                 << " | Quantity: " << item.getQuantity()
                 << " | Price: $" << item.getPrice()
                 << " | is Displayed: " << (item.isDisplayed() ? "Yes" : "No") << endl;
        }
    }

    void viewOrders() const
    {
        cout << "\n=== Orders for Seller: " << storeName << " ===\n";
        bool hasPaidOrders = false;

        for (const auto &order : orders)
        {
            if (order.getStatus() == "PAID")
            {
                hasPaidOrders = true;
                order.printSummary();
                cout << "Items:\n";
                for (const auto &item : order.getItems())
                {
                    cout << "  - " << item.getName()
                         << " x" << item.getQuantity()
                         << " ($" << item.getPrice() << ")\n";
                }
                cout << "-----------------------------------\n";
            }
        }

        if (!hasPaidOrders)
            cout << "No paid orders found.\n";
    }

    int getSellerId() const { return sellerId; }
    string getStoreName() const { return storeName; }

    void makeItemVisibleToCustomer(int itemId)
    {
        for (auto &item : items)
        {
            if (item.getId() == itemId)
            {
                item.setDisplay(true);
                break;
            }
        }
    }

    void makeItemInvisibleToCustomer(int itemId)
    {
        for (auto &item : items)
        {
            if (item.getId() == itemId)
            {
                item.setDisplay(false);
                cout << "Item '" << item.getName() << "' is now hidden from customers.\n";
                return;
            }
        }
        cout << "Item not found.\n";
    }

    Item *findItemById(int itemId)
    {
        for (auto &item : items)
        {
            if (item.getId() == itemId && item.isDisplayed())
            {
                return &item;
            }
        }
        return nullptr;
    }

    void printInfo() const
    {
        cout << "=== Seller Info ===\n";
        cout << "Seller ID: " << sellerId << endl;
        cout << "Store Name: " << storeName << endl;
        cout << "Store Address: " << storeAddress << endl;
        cout << "Store Phone: " << storePhone << endl;
        cout << "Store Email: " << storeEmail << endl;
    }
};