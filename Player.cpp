#include "Player.h"
#include "Cards.h"
#include "Orders.h"

// Constructor
Player::Player(std::string n) {
    name = new std::string(n);
    territories = new std::vector<Territory*>();
    hand = new Hand();
    orders = new OrdersList();
}

// Copy constructor (deep copy)
Player::Player(const Player& other) {
    name = new std::string(*(other.name));
    territories = new std::vector<Territory*>(*(other.territories));
    hand = new Hand(*(other.hand));
    orders = new OrdersList(*(other.orders));
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
        orders = new OrdersList(*(other.orders));
    }
    return *this;
}

// Destructor
Player::~Player() {
    delete name;
    delete territories;
    delete hand;         
    delete orders;       
    
    // Set to nullptr to prevent dangling pointers
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
    out << "  Orders: " << *p.orders;
    return out;
}

// Getters
std::string* Player::getName() const { return name; }
std::vector<Territory*>* Player::getTerritories() const { return territories; }
Hand* Player::getHand() const { return hand; }
OrdersList* Player::getOrders() const { return orders; }

// Add territory
void Player::addTerritory(Territory* t) {
    territories->push_back(t);
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
    std::string target = territories->empty() ? "NoTerritory" : territories->front()->getName();
    Order* order = new Deploy(1, target);
    orders->add(order);
    std::cout << "Order issued: Deploy to " << target << std::endl;
}