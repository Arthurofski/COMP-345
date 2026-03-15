#include "Player.h"
#include "Cards.h"
#include "Orders.h"

// Constructor
Player::Player(std::string n) {
    name = new std::string(n);
    territories = new std::vector<Territory*>();
    hand = new Hand();
    orders = new OrdersList();
    reinforcementPool = new int(0);
}

// Copy constructor (deep copy)
Player::Player(const Player& other) {
    name = new std::string(*(other.name));
    territories = new std::vector<Territory*>(*(other.territories));
    hand = new Hand(*(other.hand));
    orders = new OrdersList(*(other.orders));
    reinforcementPool = new int(*other.reinforcementPool);
}

// Assignment operator
Player& Player::operator=(const Player& other) {
    if (this != &other) {
        delete name;
        delete territories;
        delete hand;
        delete orders;
        delete reinforcementPool;
        
        name = new std::string(*(other.name));
        territories = new std::vector<Territory*>(*(other.territories));
        hand = new Hand(*(other.hand));
        orders = new OrdersList(*(other.orders));
        reinforcementPool = new int(*other.reinforcementPool);
    }
    return *this;
}

// Destructor
Player::~Player() {
    delete name;
    delete territories;
    delete hand;         
    delete orders;       
    delete reinforcementPool;
    
    // Set to nullptr to prevent dangling pointers
    name = nullptr;
    territories = nullptr;
    hand = nullptr;
    orders = nullptr;
    reinforcementPool = nullptr;
    
}

// Stream insertion
std::ostream& operator<<(std::ostream& out, const Player& p) {
    out << "Player: " << *(p.name) << std::endl;
    out << "  Territories: " << p.territories->size() << std::endl;
    out << "  Reinforcement Pool: " << *p.reinforcementPool << std::endl;
    out << "  Hand: " << *(p.hand) << std::endl;
    out << "  Orders: " << *p.orders;
    return out;
}

// Getters
std::string* Player::getName() const { return name; }
std::vector<Territory*>* Player::getTerritories() const { return territories; }
Hand* Player::getHand() const { return hand; }
OrdersList* Player::getOrders() const { return orders; }
int Player::getReinforcementPool() const { return *reinforcementPool; }
void Player::setReinforcementPool(int pool) { *reinforcementPool = pool; }

// Add territory and mark the territory's owner as this player
void Player::addTerritory(Territory* t) {
    territories->push_back(t);
    t->owner = this;
}

// Next methods are arbitrary implementations 
// Defend method
// Arbitrary implementation - returns all owned territories to defend
std::vector<Territory*>* Player::toDefend() {
    std::vector<Territory*>* result = new std::vector<Territory*>();
    
    // Return all owned territories to defend
    for (Territory* t : *territories) {
        result->push_back(t);
    }
    
    return result;
}

// Attack method
std::vector<Territory*>* Player::toAttack() {
    std::vector<Territory*>* result = new std::vector<Territory*>();
    //attack implementation 
    for (Territory* owned : *territories) {
        for (Territory* neighbour : *owned->neighbours) {
            // Only include territories not owned by this player
            if (neighbour->owner != this) {
                // Avoid duplicates
                bool already = false;
                for (Territory* r : *result) {
                    if (r == neighbour) { already = true; break; }
                }
                if (!already) result->push_back(neighbour);
            }
        }
    }
    
    return result;
}

/**
 * issueOrder: creates a Deploy order targeting the player's first territory
 * and adds it to the player's OrdersList.
 */
void Player::issueOrder() {
    Territory* target = territories->empty() ? nullptr : territories->front();
    Order* order = new Deploy(1, this, target);
    orders->add(order);
    std::cout << "Order issued: Deploy to "
              << (target ? target->getName() : "NoTerritory") << std::endl;
}