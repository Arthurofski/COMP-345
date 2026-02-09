#include "Orders.h"
#include <iostream>
using namespace std;

Order::Order() {
    cout << "Order created." << endl;
}
Order::Order(const Order& other) {
    cout << "Order copied." << endl;
}

void Order::validate(Order* order) {
    cout << "Validating order..." << endl;
}
void Order::execute() {
    cout << "Executing order..." << endl;
}

Deploy::Deploy(){
    this->_amount = new int(0);
    this->_destination = new std::string("");
}

Deploy::Deploy(int amount, const std::string destination) {
    this->_amount = new int(amount);
    this->_destination = new std::string(destination);
}
Deploy::Deploy(const Deploy& other) {
    this->_amount = new int(*(other._amount));
    this->_destination = new std::string(*(other._destination));
}
Deploy& Deploy::operator=(const Deploy& otherDeploy){
    this->_amount = new int(*(otherDeploy._amount));
    this->_destination = new std::string(*(otherDeploy._destination));
    return *this;
}

int Deploy::getAmount() const { return *this->_amount; }
std::string Deploy::getDestination() const { return *this->_destination; }
void Deploy::setAmount(int amount) { *this->_amount = amount; }
void Deploy::setDestination(std::string destination) { *this->_destination = destination; }

std::ostream& operator<<(std::ostream& os, const Deploy& Deploy){
    os << "Deploy Amount: " << Deploy.getAmount() << ", Destination: " << Deploy.getDestination();
    return os;
}
Deploy::~Deploy() {
    delete this->_amount;
    delete this->_destination;
}


Advance::Advance(int amount, const std::string origin, const std::string destination) : Order(amount, destination) {
    this->_origin = new std::string(origin);
}
Advance::Advance(const Advance& other) : Order(other) {
    this->_origin = new std::string(*(other._origin));
}



DeployOrder::~DeployOrder() {
    cout << "DeployOrder destroyed." << endl;
}
AttackOrder::~AttackOrder() {
    delete this->_origin;
    cout << "AttackOrder destroyed." << endl;
}


