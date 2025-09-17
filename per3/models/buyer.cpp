#include "buyer.h"

Buyer::Buyer(int id, string name, int bankAccountId)
    : id(id), name(name), bankAccountId(bankAccountId) {}

int Buyer::getId() const { return id; }
string Buyer::getName() const { return name; }
int Buyer::getBankAccountId() const { return bankAccountId; }
