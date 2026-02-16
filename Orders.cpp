#include "Orders.h"
#include <iostream>
using namespace std;

Order::Order(){
    _description = new std::string("");
}
    

Order::Order(const std::string description)
    : _description(new std::string(description)){}
     

Order::Order(const Order& other)
    : _description(new std::string(*other._description)){}

Order& Order::operator=(const Order& other) {
    if (this == &other) return *this;
    *_description = *other._description;
    return *this;
}
Order::~Order() {
    delete _description;
}
std::string Order::getDescription() const { return *_description; }
void Order::setDescription(std::string description) { *_description = description; }


std::ostream& operator<<(std::ostream& os, const Order& order) {
    os << order.getDescription();
    return os;
}

OrdersList::OrdersList()
    : _orders(new std::vector<Order*>()) {}

OrdersList::OrdersList(const OrdersList& other)
    : _orders(new std::vector<Order*>()) {
    copyFrom(other);
}

OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this == &other) return *this;
    clear();
    copyFrom(other);
    return *this;
}

OrdersList::~OrdersList() {
    clear();
    delete _orders;
}

void OrdersList::add(Order* order) {
    if (!order) return;
    _orders->push_back(order);
}

bool OrdersList::remove(int index) {
    if (index < 0 || index >= static_cast<int>(_orders->size())) return false;
    delete (*_orders)[index];
    _orders->erase(_orders->begin() + index);
    return true;
}

bool OrdersList::move(int from, int to) {
    if (from < 0 || from >= static_cast<int>(_orders->size())) return false;
    if (to < 0 || to >= static_cast<int>(_orders->size())) return false;
    if (from == to) return true;

    Order* temp = (*_orders)[from];
    _orders->erase(_orders->begin() + from);
    _orders->insert(_orders->begin() + to, temp);
    return true;
}

int OrdersList::size() const { return static_cast<int>(_orders->size()); }

const Order* OrdersList::get(int index) const {
    if (index < 0 || index >= static_cast<int>(_orders->size())) return nullptr;
    return (*_orders)[index];
}

void OrdersList::clear() {
    for (Order* o : *_orders) delete o;
    _orders->clear();
}

void OrdersList::copyFrom(const OrdersList& other) {
    for (Order* o : *other._orders) {
        _orders->push_back(o ? o->clone() : nullptr);
    }
}

std::ostream& operator<<(std::ostream& os, const OrdersList& list) {
    os << "OrdersList (" << list.size() << " orders)\n";
    for (int i = 0; i < list.size(); ++i) {
        const Order* o = list.get(i);
        os << "  [" << i << "] ";
        if (o) os << *o;
        else os << "(null)";
        os << "\n";
    }
    return os;
}



Deploy::Deploy()
    : Order("Deploy"),
      _armies(new int(0)),
      _targetTerritoryName(new std::string("")) {}

Deploy::Deploy(int armies, const std::string& targetTerritoryName)
    : Order("Deploy"),
      _armies(new int(armies)),
      _targetTerritoryName(new std::string(targetTerritoryName)) {}

Deploy::Deploy(const Deploy& other)
    : Order(other),
      _armies(new int(*other._armies)),
      _targetTerritoryName(new std::string(*other._targetTerritoryName)) {}

Deploy& Deploy::operator=(const Deploy& other) {
    if (this == &other) return *this;
    Order::operator=(other);
    *_armies = *other._armies;
    *_targetTerritoryName = *other._targetTerritoryName;
    return *this;
}

Deploy::~Deploy() {
    delete _armies;
    delete _targetTerritoryName;
}

int Deploy::getArmies() const { return *_armies; }
std::string Deploy::getTargetTerritoryName() const { return *_targetTerritoryName; }
void Deploy::setArmies(int armies) { *_armies = armies; }
void Deploy::setTargetTerritoryName(std::string targetTerritoryName) { *_targetTerritoryName = targetTerritoryName; }


Order* Deploy::clone() const { return new Deploy(*this); }

bool Deploy::validate() const {
    // Placeholder validation; later: check player owns target, has reinforcements, armies > 0, etc.
    return (*_armies >= 0);
}

void Deploy::execute() {
    if (!validate()) {
        cout << "Invalid Deploy order: " << *this << endl;
        return;
    }
    cout << "Executing Deploy: " << *_armies << " armies to " << *_targetTerritoryName << endl;
}


Advance::Advance()
    : Order("Advance"),
      _armies(new int(0)),
      _sourceTerritoryName(new std::string("")),
      _targetTerritoryName(new std::string("")) {}

Advance::Advance(int armies, const std::string& sourceTerritoryName, const std::string& targetTerritoryName)
    : Order("Advance"),
      _armies(new int(armies)),
      _sourceTerritoryName(new std::string(sourceTerritoryName)),
      _targetTerritoryName(new std::string(targetTerritoryName)) {}

Advance::Advance(const Advance& other)
    : Order(other),
      _armies(new int(*other._armies)),
      _sourceTerritoryName(new std::string(*other._sourceTerritoryName)),
      _targetTerritoryName(new std::string(*other._targetTerritoryName)) {}

Advance& Advance::operator=(const Advance& other) {
    if (this == &other) return *this;
    Order::operator=(other);
    *_armies = *other._armies;
    *_sourceTerritoryName = *other._sourceTerritoryName;
    *_targetTerritoryName = *other._targetTerritoryName;
    return *this;
}

Advance::~Advance() {
    delete _armies;
    delete _sourceTerritoryName;
    delete _targetTerritoryName;
}

Order* Advance::clone() const { return new Advance(*this); }

bool Advance::validate() const {
    // Placeholder; later: check adjacency, source owned, enough armies, etc.
    return (*_armies >= 0);
}

void Advance::execute() {
    if (!validate()) {
        cout << "Invalid Advance order: " << *this << endl;
        return;
    }
    cout << "Executing Advance: " << *_armies << " armies from " << *_sourceTerritoryName
         << " to " << *_targetTerritoryName << endl;
}

Bomb::Bomb()
    : Order("Bomb"),
      _targetTerritoryName(new std::string("")) {}

Bomb::Bomb(const std::string& targetTerritoryName)
    : Order("Bomb"),
      _targetTerritoryName(new std::string(targetTerritoryName)) {}

Bomb::Bomb(const Bomb& other)
    : Order(other),
      _targetTerritoryName(new std::string(*other._targetTerritoryName)) {}

Bomb& Bomb::operator=(const Bomb& other) {
    if (this == &other) return *this;
    Order::operator=(other);
    *_targetTerritoryName = *other._targetTerritoryName;
    return *this;
}

Bomb::~Bomb() {
    delete _targetTerritoryName;
}

Order* Bomb::clone() const { return new Bomb(*this); }

bool Bomb::validate() const {
    return !_targetTerritoryName->empty();
}

void Bomb::execute() {
    if (!validate()) {
        cout << "Invalid Bomb order: " << *this << endl;
        return;
    }
    cout << "Executing Bomb on " << *_targetTerritoryName << endl;
}

Blockade::Blockade()
    : Order("Blockade"),
      _targetTerritoryName(new std::string("")) {}

Blockade::Blockade(const std::string& targetTerritoryName)
    : Order("Blockade"),
      _targetTerritoryName(new std::string(targetTerritoryName)) {}

Blockade::Blockade(const Blockade& other)
    : Order(other),
      _targetTerritoryName(new std::string(*other._targetTerritoryName)) {}

Blockade& Blockade::operator=(const Blockade& other) {
    if (this == &other) return *this;
    Order::operator=(other);
    *_targetTerritoryName = *other._targetTerritoryName;
    return *this;
}

Blockade::~Blockade() {
    delete _targetTerritoryName;
}

Order* Blockade::clone() const { return new Blockade(*this); }

bool Blockade::validate() const {
    return !_targetTerritoryName->empty();
}
void Blockade::execute() {
    if (!validate()) {
        cout << "Invalid Blockade order: " << *this << endl;
        return;
    }
    cout << "Executing Blockade on " << *_targetTerritoryName << endl;
}

Airlift::Airlift()
    : Order("Airlift"),
      _armies(new int(0)),
      _sourceTerritoryName(new std::string("")),
      _targetTerritoryName(new std::string("")) {}

Airlift::Airlift(int armies, const std::string& sourceTerritoryName, const std::string& targetTerritoryName)
    : Order("Airlift"),
      _armies(new int(armies)),
      _sourceTerritoryName(new std::string(sourceTerritoryName)),
      _targetTerritoryName(new std::string(targetTerritoryName)) {}

Airlift::Airlift(const Airlift& other)
    : Order(other),
      _armies(new int(*other._armies)),
      _sourceTerritoryName(new std::string(*other._sourceTerritoryName)),
      _targetTerritoryName(new std::string(*other._targetTerritoryName)) {}

Airlift& Airlift::operator=(const Airlift& other) {
    if (this == &other) return *this;
    Order::operator=(other);
    *_armies = *other._armies;
    *_sourceTerritoryName = *other._sourceTerritoryName;
    *_targetTerritoryName = *other._targetTerritoryName;
    return *this;
}

Airlift::~Airlift() {
    delete _armies;
    delete _sourceTerritoryName;
    delete _targetTerritoryName;
}

Order* Airlift::clone() const { return new Airlift(*this); }

bool Airlift::validate() const {
    return (*_armies >= 0) && !_sourceTerritoryName->empty() && !_targetTerritoryName->empty();
}

void Airlift::execute() {
    if (!validate()) {
        cout << "Invalid Airlift order: " << *this << endl;
        return;
    }
    cout << "Executing Airlift: " << *_armies << " armies from " << *_sourceTerritoryName
         << " to " << *_targetTerritoryName << endl;
}

Negotiate::Negotiate()
    : Order("Negotiate"),
      _targetPlayerName(new std::string("")) {}

Negotiate::Negotiate(const std::string& targetPlayerName)
    : Order("Negotiate"),
      _targetPlayerName(new std::string(targetPlayerName)) {}

Negotiate::Negotiate(const Negotiate& other)
    : Order(other),
      _targetPlayerName(new std::string(*other._targetPlayerName)) {}

Negotiate& Negotiate::operator=(const Negotiate& other) {
    if (this == &other) return *this;
    Order::operator=(other);
    *_targetPlayerName = *other._targetPlayerName;
    return *this;
}

Negotiate::~Negotiate() {
    delete _targetPlayerName;
}

Order* Negotiate::clone() const { return new Negotiate(*this); }

bool Negotiate::validate() const {
    return !_targetPlayerName->empty();
}

void Negotiate::execute() {
    if (!validate()) {
        cout << "Invalid Negotiate order: " << *this << endl;
        return;
    }
    cout << "Executing Negotiate with player " << *_targetPlayerName << endl;
}


