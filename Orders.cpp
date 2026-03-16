#include "Orders.h"
#include "Player.h"  // provides full Player and Territory definitions for validate/execute
#include <iostream>
#include <cstdlib>
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

// Method to convert an order's description to a string for logging purposes
string Order::stringToLog() const {
    return "Order executed: " + *_description;
}

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
    notify(this); // Notify observers that a new order has been added
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

// method to convert an orders list to a string for logging purposes
string OrdersList::stringToLog() const {
    return "Order Issued and added to list";
}

Deploy::Deploy()
    : Order("Deploy"),
      _armies(new int(0)),
      _player(nullptr),
      _territory(nullptr) {}

Deploy::Deploy(int armies, Player* player, Territory* territory)
    : Order("Deploy"),
      _armies(new int(armies)),
      _player(player),
      _territory(territory) {}

Deploy::Deploy(const Deploy& other)
    : Order(other),
      _armies(new int(*other._armies)),
      _player(other._player),       // shallow copy — non-owning
      _territory(other._territory)  // shallow copy — non-owning
      {}

Deploy& Deploy::operator=(const Deploy& other) {
    if (this == &other) return *this;
    Order::operator=(other);
    *_armies   = *other._armies;
    _player    = other._player;    // shallow — non-owning
    _territory = other._territory; // shallow — non-owning
    return *this;
}

Deploy::~Deploy() {
    delete _armies;
    // _player and _territory are non-owning; do not delete
}

int        Deploy::getArmies()    const { return *_armies; }
Player*    Deploy::getPlayer()    const { return _player; }
Territory* Deploy::getTerritory() const { return _territory; }
void       Deploy::setArmies(int armies) { *_armies = armies; }

Order* Deploy::clone() const { return new Deploy(*this); }

// Valid when: player and territory are set, player owns the territory,
// armies > 0, and the player's reinforcement pool can cover the cost.
bool Deploy::validate() const {
    if (!_player || !_territory)           return false;
    if (_territory->owner != _player)      return false;
    if (*_armies <= 0)                     return false;
    if (_player->getReinforcementPool() < *_armies) return false;
    return true;
}

void Deploy::execute() {
    notify(this);  // Log execution attempt (valid or invalid)
    if (!validate()) {
        cout << "Invalid Deploy order." << endl;
        return;
    }
    *_territory->armies += *_armies;
    _player->setReinforcementPool(_player->getReinforcementPool() - *_armies);
    cout << "Deployed " << *_armies << " armies to " << _territory->getName()
         << ". Armies there: " << *_territory->armies
         << ". Remaining pool: " << _player->getReinforcementPool() << endl;
}


namespace {
bool areAdjacent(const Territory* a, const Territory* b) {
    if (!a || !b || !a->neighbours) return false;
    for (Territory* n : *a->neighbours) {
        if (n == b) return true;
    }
    return false;
}

bool isAdjacentToAnyOwned(const Player* player, const Territory* target) {
    if (!player || !target) return false;
    std::vector<Territory*>* owned = player->getTerritories();
    if (!owned) return false;
    for (Territory* t : *owned) {
        if (areAdjacent(t, target)) return true;
    }
    return false;
}
}

Advance::Advance()
    : Order("Advance"),
      _armies(new int(0)),
      _player(nullptr),
      _source(nullptr),
      _target(nullptr) {}

Advance::Advance(int armies, Player* player, Territory* source, Territory* target)
    : Order("Advance"),
      _armies(new int(armies)),
      _player(player),
      _source(source),
      _target(target) {}

Advance::Advance(const Advance& other)
    : Order(other),
      _armies(new int(*other._armies)),
      _player(other._player),
      _source(other._source),
      _target(other._target) {}

Advance& Advance::operator=(const Advance& other) {
    if (this == &other) return *this;
    Order::operator=(other);
    *_armies = *other._armies;
    _player = other._player;
    _source = other._source;
    _target = other._target;
    return *this;
}

Advance::~Advance() {
    delete _armies;
}

Order* Advance::clone() const { return new Advance(*this); }

bool Advance::validate() const {
    if (!_player || !_source || !_target) return false;
    if (*_armies <= 0) return false;
    if (_source->owner != _player) return false;
    if (*_source->armies < *_armies) return false;
    if (!areAdjacent(_source, _target)) return false;
    return true;
}

void Advance::execute() {
    notify(this);
    if (!validate()) {
        cout << "Invalid Advance order." << endl;
        return;
    }

    if (_target->owner && _player->isNegotiatingWith(_target->owner)) {
        cout << "Advance blocked by negotiate effect." << endl;
        return;
    }

    *_source->armies -= *_armies;

    if (_target->owner == _player) {
        *_target->armies += *_armies;
        cout << "Advanced " << *_armies << " armies from " << _source->getName()
             << " to " << _target->getName() << endl;
        return;
    }

    int attackers = *_armies;
    int defenders = *_target->armies;

    while (attackers > 0 && defenders > 0) {
        int attackersAtStart = attackers;
        int defendersAtStart = defenders;
        int killsByAttackers = 0;
        int killsByDefenders = 0;

        for (int i = 0; i < attackersAtStart; ++i) {
            if ((rand() % 100) < 60) ++killsByAttackers;
        }
        for (int i = 0; i < defendersAtStart; ++i) {
            if ((rand() % 100) < 70) ++killsByDefenders;
        }

        defenders -= (killsByAttackers < defenders ? killsByAttackers : defenders);
        attackers -= (killsByDefenders < attackers ? killsByDefenders : attackers);
    }

    if (defenders <= 0 && attackers > 0) {
        Player* defender = _target->owner;
        if (defender) defender->removeTerritory(_target);
        _player->addTerritory(_target);
        *_target->armies = attackers;
        _player->markConqueredTerritory();
        cout << "Advance conquered " << _target->getName() << " with " << attackers
             << " surviving armies." << endl;
    } else {
        *_target->armies = defenders;
        cout << "Advance attack failed on " << _target->getName()
             << ". Defenders left: " << defenders << endl;
    }
}

Bomb::Bomb()
    : Order("Bomb"),
      _player(nullptr),
      _target(nullptr) {}

Bomb::Bomb(Player* player, Territory* targetTerritory)
    : Order("Bomb"),
      _player(player),
      _target(targetTerritory) {}

Bomb::Bomb(const Bomb& other)
    : Order(other),
      _player(other._player),
      _target(other._target) {}

Bomb& Bomb::operator=(const Bomb& other) {
    if (this == &other) return *this;
    Order::operator=(other);
    _player = other._player;
    _target = other._target;
    return *this;
}

Bomb::~Bomb() {}

Order* Bomb::clone() const { return new Bomb(*this); }

bool Bomb::validate() const {
    if (!_player || !_target) return false;
    if (_target->owner == _player) return false;
    if (!isAdjacentToAnyOwned(_player, _target)) return false;
    return true;
}

void Bomb::execute() {
    notify(this);
    if (!validate()) {
        cout << "Invalid Bomb order." << endl;
        return;
    }

    if (_target->owner && _player->isNegotiatingWith(_target->owner)) {
        cout << "Bomb blocked by negotiate effect." << endl;
        return;
    }

    *_target->armies /= 2;
    cout << "Bombed " << _target->getName() << ". Armies now: " << *_target->armies << endl;
}

Blockade::Blockade()
    : Order("Blockade"),
      _player(nullptr),
      _target(nullptr) {}

Blockade::Blockade(Player* player, Territory* targetTerritory)
    : Order("Blockade"),
      _player(player),
      _target(targetTerritory) {}

Blockade::Blockade(const Blockade& other)
    : Order(other),
      _player(other._player),
      _target(other._target) {}

Blockade& Blockade::operator=(const Blockade& other) {
    if (this == &other) return *this;
    Order::operator=(other);
    _player = other._player;
    _target = other._target;
    return *this;
}

Blockade::~Blockade() {}

Order* Blockade::clone() const { return new Blockade(*this); }

bool Blockade::validate() const {
    if (!_player || !_target) return false;
    return _target->owner == _player;
}

void Blockade::execute() {
    notify(this);
    if (!validate()) {
        cout << "Invalid Blockade order." << endl;
        return;
    }

    static Player neutralPlayer("Neutral");

    *_target->armies *= 2;
    _player->removeTerritory(_target);
    neutralPlayer.addTerritory(_target);

    cout << "Blockade on " << _target->getName()
         << ": armies doubled and ownership transferred to Neutral." << endl;
}

Airlift::Airlift()
    : Order("Airlift"),
      _armies(new int(0)),
      _player(nullptr),
      _source(nullptr),
      _target(nullptr) {}

Airlift::Airlift(int armies, Player* player, Territory* sourceTerritory, Territory* targetTerritory)
    : Order("Airlift"),
      _armies(new int(armies)),
      _player(player),
      _source(sourceTerritory),
      _target(targetTerritory) {}

Airlift::Airlift(const Airlift& other)
    : Order(other),
      _armies(new int(*other._armies)),
      _player(other._player),
      _source(other._source),
      _target(other._target) {}

Airlift& Airlift::operator=(const Airlift& other) {
    if (this == &other) return *this;
    Order::operator=(other);
    *_armies = *other._armies;
    _player = other._player;
    _source = other._source;
    _target = other._target;
    return *this;
}

Airlift::~Airlift() {
    delete _armies;
}

Order* Airlift::clone() const { return new Airlift(*this); }

bool Airlift::validate() const {
    if (!_player || !_source || !_target) return false;
    if (*_armies <= 0) return false;
    if (_source->owner != _player || _target->owner != _player) return false;
    if (*_source->armies < *_armies) return false;
    return true;
}

void Airlift::execute() {
    notify(this);
    if (!validate()) {
        cout << "Invalid Airlift order." << endl;
        return;
    }

    *_source->armies -= *_armies;
    *_target->armies += *_armies;
    cout << "Airlifted " << *_armies << " armies from " << _source->getName()
         << " to " << _target->getName() << endl;
}

Negotiate::Negotiate()
    : Order("Negotiate"),
      _player(nullptr),
      _targetPlayer(nullptr) {}

Negotiate::Negotiate(Player* player, Player* targetPlayer)
    : Order("Negotiate"),
      _player(player),
      _targetPlayer(targetPlayer) {}

Negotiate::Negotiate(const Negotiate& other)
    : Order(other),
      _player(other._player),
      _targetPlayer(other._targetPlayer) {}

Negotiate& Negotiate::operator=(const Negotiate& other) {
    if (this == &other) return *this;
    Order::operator=(other);
    _player = other._player;
    _targetPlayer = other._targetPlayer;
    return *this;
}

Negotiate::~Negotiate() {}

Order* Negotiate::clone() const { return new Negotiate(*this); }

bool Negotiate::validate() const {
    if (!_player || !_targetPlayer) return false;
    return _player != _targetPlayer;
}

void Negotiate::execute() {
    notify(this);
    if (!validate()) {
        cout << "Invalid Negotiate order." << endl;
        return;
    }

    _player->addNegotiatedPlayer(_targetPlayer);
    _targetPlayer->addNegotiatedPlayer(_player);
    cout << "Negotiate set between " << *_player->getName()
         << " and " << *_targetPlayer->getName() << endl;
}


