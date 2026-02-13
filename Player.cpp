#include "Player.h"

// Constructor
Player::Player(std::string n) {
    name = new std::string(n);
    territories = new std::vector<Territory*>();
    hand = new Hand();
    orders = new OrderList();
}

// Copy constructor
Player::Player(const Player& other) {
    name = new std::string(*(other.name));
    territories = new std::vector<Territory*>(*(other.territories));
    hand = new Hand(*(other.hand));
    orders = new OrderList(*(other.orders));
}

// Assignment operator
Player& Player::operator=(const Player& other) {
    if (this != &other) {
        delete name;
        delete territories;
        delete hand;
        delete orders;
        
        name = new std::string(*(other.name));
        territories = new std::vector<Territory*>(*(other.territories));
        hand = new Hand(*(other.hand));
        orders = new OrderList(*(other.orders));
    }
    return *this;
}

// Destructor
Player::~Player() {
    delete name;
    delete territories;
    delete hand;
    delete orders;

    // Avoid dangling pointers
    name = nullptr;
    territories = nullptr;
    hand = nullptr;
    orders = nullptr;
}

// Stream insertion
std::ostream& operator<<(std::ostream& out, const Player& p) {
    out << "Player: " << *(p.name) << std::endl;
    out << "  Territories: " << p.territories->size() << std::endl;
    out << "  Hand: " << *(p.hand) << std::endl;
    out << "  Orders: " << *(p.orders);
    return out;
}

// Getters
std::string* Player::getName() const { return name; }
std::vector<Territory*>* Player::getTerritories() const { return territories; }
Hand* Player::getHand() const { return hand; }
OrderList* Player::getOrders() const { return orders; }

// Add territory
void Player::addTerritory(Territory* t) {
    territories->push_back(t);
}

// Next methods are arbirtrary implementations 
// Defend method
std::vector<Territory*>* Player::toDefend() {
    std::vector<Territory*>* result = new std::vector<Territory*>();
    
    // Return first territory if exists
    if (!territories->empty()) {
        result->push_back((*territories)[0]);
    }
    
    return result;
}

// Attack method
std::vector<Territory*>* Player::toAttack() {
    std::vector<Territory*>* result = new std::vector<Territory*>();
    
    // Create a dummy territory to attack
    Territory* dummy = new Territory("DummyEnemyTerritory");
    result->push_back(dummy);
    
    return result;
}

// Issue Order method 
void Player::issueOrder() {
    Order* order = new Order();
    orders->add(order);
    std::cout << "Order issued." << std::endl;
}
