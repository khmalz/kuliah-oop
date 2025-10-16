#include <string>

using namespace std;

class User{
private:
    
    string name;
    string phone;
    int loginState; //0 = out, 1= login
    static inline int id;

public:
    User(){
        id = 0;
    }

    User(int id, string name, string phone): name(name), phone(phone) {
        User::id = id;
        loginState = 0;
    }

    User(string name, string phone): name(name), phone(phone) {
        id++;
        loginState = 0;
    }

    string getName(){return name;};
    string getPhone(){return phone;};
    int getLoginState(){return loginState;};
    void setLoginState(int state){ loginState = state;};
    static int getId(){return id;};
    static void resetId(){
        id = 0;
    }
};