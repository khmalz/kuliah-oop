#include <iostream>
#include "user.h"

using namespace std;

enum PrimaryPrompt
{
    LOGIN,
    REGISTER,
    EXIT,
    MAIN_PROMPT
};
enum subLoginPrompt
{
    USER_VERIFICATION,
    LOGIN_MENU
};
enum loginMenuPrompt
{
    LIST_CONTACT,
    ADD_CONTACT,
    LOGOUT,
    LOGIN_MENU_PROMPT
};

int main()
{
    PrimaryPrompt prompt = MAIN_PROMPT;
    subLoginPrompt sub_login_prompt = USER_VERIFICATION;
    loginMenuPrompt login_menu_prompt = LOGIN_MENU_PROMPT;

    // reset static variables to 0
    User::resetId();

    User user;
    // test case
    user = User("test", "123");
    while (prompt != EXIT)
    {
        switch (prompt)
        {
        case MAIN_PROMPT:
            cout << "Select an option: " << endl;
            cout << "1. Login" << endl;
            cout << "2. Register" << endl;
            cout << "3. Exit" << endl;
            int choice;
            cin >> choice;
            prompt = static_cast<PrimaryPrompt>(choice - 1);
            break;
        case LOGIN:
        {
            switch (sub_login_prompt)
            {
            case USER_VERIFICATION:
            {
                string name, phone;
                cout << "Please enter name and phone number for Login." << endl;
                cout << "Name: ";
                cin >> name;
                cout << "Phone: ";
                cin >> phone;
                if ((user.getName() == name) & (phone == user.getPhone()))
                {
                    sub_login_prompt = LOGIN_MENU;

                    login_menu_prompt = LOGIN_MENU_PROMPT;

                    user.setLoginState(1);
                }
                else
                {
                    cout << "Invalid credentials. Returning to main menu." << endl;
                    prompt = MAIN_PROMPT;
                }
                break;
            }
            case LOGIN_MENU:
            {
                switch (login_menu_prompt)
                {

                case LOGIN_MENU_PROMPT:
                {
                    if (user.getLoginState() == 0)
                    {
                        cout << "GA LOGIN" << endl;

                        prompt = MAIN_PROMPT;
                        sub_login_prompt = USER_VERIFICATION;
                        break;
                    }

                    cout << "Please select the following" << endl;
                    cout << "1. List contact" << endl;
                    cout << "2. Add contact" << endl;
                    cout << "3. Logout" << endl;
                    cin >> choice;
                    login_menu_prompt = static_cast<loginMenuPrompt>(choice - 1);

                    break;
                }
                case LIST_CONTACT:
                {
                    cout << "List contact selected." << endl;

                    login_menu_prompt = LOGIN_MENU_PROMPT;

                    break;
                }
                case ADD_CONTACT:
                {
                    cout << "Add contact selected." << endl;

                    login_menu_prompt = LOGIN_MENU_PROMPT;

                    break;
                }
                case LOGOUT:
                {
                    cout << "Logout selected." << endl;

                    user.setLoginState(0);

                    prompt = MAIN_PROMPT;
                    login_menu_prompt = LOGIN_MENU_PROMPT;
                    sub_login_prompt = USER_VERIFICATION;

                    cout << "udah logout? " << user.getLoginState() << endl;

                    break;
                }
                }
            }
            }
            break;
        }
        case REGISTER:
        {
            string name, phone;
            cout << "Please enter name and phone number for registration." << endl;
            cout << "Name: ";
            cin >> name;
            cout << "Phone: ";
            cin >> phone;
            user = User(name, phone);
            prompt = LOGIN;
            sub_login_prompt = USER_VERIFICATION;
            break;
        }
        case EXIT:
            return 1;
        }
    }

    return 0;
}