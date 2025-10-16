#include <iostream>
#include "bank_customer.h"
#include "buyer.h"
#include "seller.h"

enum PrimaryPrompt
{
    LOGIN,
    REGISTER,
    EXIT
};

enum LoginPrompt
{
    BACK_LOGIN = -1
};

enum BuyerPrompt
{
    CHECK_STATUS = 0,
    UPGRADE_TO_SELLER,
    CREATE_BANK_ACCOUNT,
    BROWSE_STORE,
    VIEW_CART,
    BUYER_BACK
};

enum SellerPrompt
{
    CHECK_INVENTORY,
    ADD_ITEM,
    REMOVE_ITEM,
    VIEW_ORDERS,
    HIDE_ITEM,
    SHOW_ITEM,
    SELLER_BACK
};

enum RegisterPrompt
{
    CREATE_BUYER,
    CREATE_SELLER,
    BACK_REGISTER
};

using namespace std;

int main()
{
    Buyer *loggedInBuyer = nullptr;
    Seller *loggedInSeller = nullptr;
    vector<Seller *> allSellers;

    // create a loop prompt
    PrimaryPrompt prompt = LOGIN;
    LoginPrompt loginPrompt = BACK_LOGIN;
    RegisterPrompt regPrompt = CREATE_BUYER;

    while (prompt != EXIT)
    {
        cout << "Select an option: " << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Exit" << endl;
        int choice;
        cin >> choice;
        prompt = static_cast<PrimaryPrompt>(choice - 1);

        switch (prompt)
        {
        case LOGIN:
        {
            if (!loggedInBuyer)
            {
                cout << "No account registered yet. Please register first.\n";
                break;
            }

            loginPrompt = static_cast<LoginPrompt>(0);

            while (loginPrompt != BACK_LOGIN)
            {
                cout << "\n=== Login Menu ===\n";

                if (loggedInSeller)
                {
                    cout << "1. Check Inventory\n";
                    cout << "2. Add Item to Inventory\n";
                    cout << "3. Remove Item from Inventory\n";
                    cout << "4. View Orders\n";
                    cout << "5. Hide Item (make it invisible to customers)\n";
                    cout << "6. Show Item (make it visible again)\n";
                    cout << "7. Back\n";

                    int sellerChoice;
                    cin >> sellerChoice;
                    SellerPrompt sellerPrompt = static_cast<SellerPrompt>(sellerChoice - 1);

                    switch (sellerPrompt)
                    {
                    case CHECK_INVENTORY:
                        loggedInSeller->checkInventory();
                        break;

                    case ADD_ITEM:
                    {
                        int id, qty;
                        string name;
                        double price;

                        cin.ignore();
                        cout << "Enter Item Name: ";
                        getline(cin, name);
                        cout << "Enter Item Quantity: ";
                        cin >> qty;
                        cout << "Enter Item Price: ";
                        cin >> price;
                        id = static_cast<int>(rand());

                        loggedInSeller->addNewItem(id, name, qty, price);
                        break;
                    }

                    case REMOVE_ITEM:
                    {
                        int id;
                        cout << "Enter Item ID to remove: ";
                        cin >> id;
                        loggedInSeller->removeItem(id);
                        break;
                    }

                    case VIEW_ORDERS:
                        cout << "=== View Orders ===\n";
                        loggedInSeller->viewOrders();
                        break;

                    case HIDE_ITEM:
                    {
                        int id;
                        cout << "Enter Item ID to hide: ";
                        cin >> id;
                        loggedInSeller->makeItemInvisibleToCustomer(id);
                        break;
                    }

                    case SHOW_ITEM:
                    {
                        int id;
                        cout << "Enter Item ID to show: ";
                        cin >> id;
                        loggedInSeller->makeItemVisibleToCustomer(id);
                        break;
                    }

                    case SELLER_BACK:
                        cout << "Back to main menu.\n";
                        loginPrompt = BACK_LOGIN;
                        break;

                    default:
                        cout << "Invalid option.\n";
                        break;
                    }
                }
                else
                {
                    cout << "1. Check Account Status\n";
                    cout << "2. Upgrade Account to Seller\n";
                    cout << "3. Create Banking Account\n";
                    cout << "4. Browse Store\n";
                    cout << "5. View Cart / Checkout\n";
                    cout << "6. Back\n";

                    int buyerChoice;
                    cin >> buyerChoice;
                    BuyerPrompt buyerPrompt = static_cast<BuyerPrompt>(buyerChoice - 1);

                    switch (buyerPrompt)
                    {
                    case CHECK_STATUS:
                        cout << "=== Account Status ===\n";
                        loggedInBuyer->printInfo();
                        if (loggedInSeller)
                            loggedInSeller->printInfo();
                        break;

                    case UPGRADE_TO_SELLER:
                    {
                        cout << "=== Upgrade to Seller ===\n";
                        if (loggedInSeller)
                        {
                            cout << "You already have a Seller account.\n";
                            break;
                        }

                        if (!loggedInBuyer->hasAccount())
                        {
                            cout << "Upgrade failed. You must have a linked bank account.\n";
                            cout << "Please create a banking account first.\n";
                            break;
                        }

                        int sellerId = static_cast<int>(rand());
                        string storeName, storeAddress, storePhone, storeEmail;

                        cin.ignore();
                        cout << "Enter Store Name: ";
                        getline(cin, storeName);
                        cout << "Enter Store Address: ";
                        getline(cin, storeAddress);
                        cout << "Enter Store Phone Number: ";
                        getline(cin, storePhone);
                        cout << "Enter Store Email: ";
                        getline(cin, storeEmail);

                        static Seller newSeller(
                            *loggedInBuyer,
                            sellerId,
                            loggedInBuyer->getName(),
                            storeName,
                            storeAddress,
                            storePhone,
                            storeEmail);

                        loggedInSeller = &newSeller;
                        allSellers.push_back(loggedInSeller);
                        cout << "Upgrade successful! You are now also a Seller.\n";
                        break;
                    }

                    case CREATE_BANK_ACCOUNT:
                    {
                        cout << "=== Create Banking Account ===\n";
                        if (loggedInBuyer->getAccount() != nullptr)
                        {
                            cout << "You already have a banking account linked.\n";
                            break;
                        }

                        int accountId = static_cast<int>(rand());
                        string address, phone, email;
                        double initialDeposit;

                        cin.ignore();
                        cout << "Enter Address: ";
                        getline(cin, address);
                        cout << "Enter Phone Number: ";
                        getline(cin, phone);
                        cout << "Enter Email: ";
                        getline(cin, email);
                        cout << "Enter Initial Deposit: ";
                        cin >> initialDeposit;

                        BankCustomer *newAccount = new BankCustomer(accountId, loggedInBuyer->getName(), initialDeposit);
                        loggedInBuyer->setAccount(newAccount);

                        cout << "Bank account created and linked successfully!\n";
                        break;
                    }

                    case BROWSE_STORE:
                    {
                        cout << "=== Browse Stores ===\n";
                        if (allSellers.empty())
                        {
                            cout << "No stores available yet.\n";
                            break;
                        }

                        for (size_t i = 0; i < allSellers.size(); ++i)
                            cout << i + 1 << ". " << allSellers[i]->getStoreName() << "\n";

                        size_t storeChoice;
                        cout << "Select a store to browse: ";
                        cin >> storeChoice;

                        if (storeChoice < 1 || static_cast<size_t>(storeChoice) > allSellers.size())
                        {
                            cout << "Invalid store selection.\n";
                            break;
                        }

                        Seller *selectedStore = allSellers[storeChoice - 1];
                        cout << "=== " << selectedStore->getStoreName() << " Inventory ===\n";
                        selectedStore->checkInventory();

                        int itemId;
                        cout << "Enter Item ID to view details (0 to cancel): ";
                        cin >> itemId;
                        if (itemId == 0)
                            break;

                        Item *selectedItem = selectedStore->findItemById(itemId);
                        if (!selectedItem)
                        {
                            cout << "Item not found.\n";
                            break;
                        }

                        cout << "Item Details:\n";
                        cout << "Name: " << selectedItem->getName() << "\n";
                        cout << "Price: " << selectedItem->getPrice() << "\n";
                        cout << "Quantity Available: " << selectedItem->getQuantity() << "\n";

                        char addToCart;
                        cout << "Add to cart? (y/n): ";
                        cin >> addToCart;

                        if (addToCart == 'y' || addToCart == 'Y')
                        {
                            int qty;
                            cout << "Enter quantity: ";
                            cin >> qty;

                            if (qty > selectedItem->getQuantity())
                            {
                                cout << "Not enough stock.\n";
                                break;
                            }

                            Item cartItem = *selectedItem;
                            cartItem.setQuantity(qty);
                            loggedInBuyer->addToCart(cartItem);
                        }
                        break;
                    }

                    case VIEW_CART:
                    {
                        cout << "=== Your Cart ===\n";
                        loggedInBuyer->viewCart();

                        cout << "1. Remove Item\n";
                        cout << "2. Checkout\n";
                        cout << "3. Back\n";
                        int cartChoice;
                        cin >> cartChoice;

                        if (cartChoice == 1)
                        {
                            int itemId;
                            cout << "Enter Item ID to remove: ";
                            cin >> itemId;
                            loggedInBuyer->removeFromCart(itemId);
                        }
                        else if (cartChoice == 2)
                        {
                            if (allSellers.empty())
                            {
                                cout << "No sellers available to fulfill order.\n";
                                break;
                            }

                            cout << "=== Select Seller to Checkout ===\n";
                            for (size_t i = 0; i < allSellers.size(); ++i)
                                cout << i + 1 << ". " << allSellers[i]->getStoreName() << "\n";

                            int sellerChoice;
                            cout << "Select seller: ";
                            cin >> sellerChoice;

                            if (sellerChoice < 1 || static_cast<size_t>(sellerChoice) > allSellers.size())
                            {
                                cout << "Invalid selection.\n";
                                break;
                            }

                            Seller *targetSeller = allSellers[sellerChoice - 1];
                            loggedInBuyer->checkout(*targetSeller);
                        }
                        break;
                    }

                    case BUYER_BACK:
                        cout << "Back to main menu.\n";
                        loginPrompt = BACK_LOGIN;
                        break;

                    default:
                        cout << "Invalid option.\n";
                        break;
                    }
                }
            }

            break;
        }
        case REGISTER:
        {
            regPrompt = CREATE_BUYER;

            while (regPrompt != BACK_REGISTER)
            {
                cout << "Register selected.\n";
                cout << "Select an option: " << endl;
                cout << "1. Create Buyer Account" << endl;
                cout << "2. Create Seller Account" << endl;
                cout << "3. Back" << endl;

                int regChoice;
                cin >> regChoice;

                regPrompt = static_cast<RegisterPrompt>(regChoice - 1);

                switch (regPrompt)
                {
                case CREATE_BUYER:
                {
                    if (loggedInBuyer)
                    {
                        cout << "You already have a Buyer account. Please login instead.\n";
                        break;
                    }

                    int id = static_cast<int>(rand());
                    string name, address, phone, email;

                    cout << "=== Create Buyer Account ===\n";

                    cin.ignore();
                    cout << "Enter Name: ";
                    getline(cin, name);
                    cout << "Enter Home Address: ";
                    getline(cin, address);
                    cout << "Enter Phone Number: ";
                    getline(cin, phone);
                    cout << "Enter Email: ";
                    getline(cin, email);

                    static Buyer newBuyer(id, name, address, phone, email);

                    loggedInBuyer = &newBuyer;
                    loggedInSeller = nullptr;
                    cout << "Buyer registration successful! You are now logged in as Buyer.\n";
                    break;
                }
                case CREATE_SELLER:
                {
                    if (loggedInSeller)
                    {
                        cout << "You already have a Seller account. Please login instead.\n";
                        break;
                    }

                    if (!loggedInBuyer)
                    {
                        cout << "Please create a Buyer account first.\n";
                        break;
                    }

                    cout << "=== Create Seller Account ===\n";

                    int sellerId = static_cast<int>(rand());
                    string sellerName, storeName, storeAddress, storePhone, storeEmail;
                    double initialDeposit;
                    string homeAddress, phone, email;

                    cin.ignore();
                    cout << "\n=== Store Details ===\n";
                    cout << "Enter Store Name: ";
                    getline(cin, storeName);
                    cout << "Enter Store Address: ";
                    getline(cin, storeAddress);
                    cout << "Enter Store Phone Number: ";
                    getline(cin, storePhone);
                    cout << "Enter Store Email: ";
                    getline(cin, storeEmail);

                    cout << "\n=== Banking Details ===\n";
                    cout << "Enter Initial Deposit Amount: ";
                    cin >> initialDeposit;

                    int accountId = static_cast<int>(rand());
                    BankCustomer *newAccount = new BankCustomer(accountId, sellerName, initialDeposit);
                    loggedInBuyer->setAccount(newAccount);

                    static Seller newSeller(
                        *loggedInBuyer,
                        sellerId,
                        sellerName,
                        storeName,
                        storeAddress,
                        storePhone,
                        storeEmail);

                    loggedInSeller = &newSeller;

                    cout << "\nSeller registration successful!\n";
                    cout << "A new banking account has been created and linked automatically.\n";
                    cout << "You are now logged in as both Buyer and Seller.\n";
                    break;
                }
                case BACK_REGISTER:
                {
                    cout << "Back selected.\n";
                    regPrompt = BACK_REGISTER;
                    break;
                }
                default:
                    cout << "Invalid option.\n";
                    break;
                }
            }
            break;
        }
        case EXIT:
            cout << "Exiting.\n";
            break;
        default:
            cout << "Invalid option.\n";
            break;
        }

        cout << endl;
    }

    return 0;
}
