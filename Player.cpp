#include "Player.h"
#include "Cards.h"
#include "Orders.h"
#include "PlayerStrategies.h"

// Constructor
Player::Player(std::string n) {
    name = new std::string(n);
    territories = new std::vector<Territory*>();
    hand = new Hand();
    orders = new OrdersList();
    reinforcementPool = new int(0);
    negotiatedPlayers = new std::vector<Player*>();
    conqueredTerritoryThisTurn = new bool(false);
    receivedCardThisTurn = new bool(false);
    strategy = new HumanPlayerStrategy();  // Default to Human strategy
}

// Copy constructor (deep copy)
Player::Player(const Player& other) {
    name = new std::string(*(other.name));
    territories = new std::vector<Territory*>(*(other.territories));
    hand = new Hand(*(other.hand));
    orders = new OrdersList(*(other.orders));
    reinforcementPool = new int(*other.reinforcementPool);
    negotiatedPlayers = new std::vector<Player*>(*other.negotiatedPlayers);
    conqueredTerritoryThisTurn = new bool(*other.conqueredTerritoryThisTurn);
    receivedCardThisTurn = new bool(*other.receivedCardThisTurn);
    
    // Copy the strategy - note: this is a shallow copy, strategy is not owned by Player
    strategy = other.strategy;
}

// Assignment operator
Player& Player::operator=(const Player& other) {
    if (this != &other) {
        delete name;
        delete territories;
        delete hand;
        delete orders;
        delete reinforcementPool;
        delete negotiatedPlayers;
        delete conqueredTerritoryThisTurn;
        delete receivedCardThisTurn;
        
        name = new std::string(*(other.name));
        territories = new std::vector<Territory*>(*(other.territories));
        hand = new Hand(*(other.hand));
        orders = new OrdersList(*(other.orders));
        reinforcementPool = new int(*other.reinforcementPool);
        negotiatedPlayers = new std::vector<Player*>(*other.negotiatedPlayers);
        conqueredTerritoryThisTurn = new bool(*other.conqueredTerritoryThisTurn);
        receivedCardThisTurn = new bool(*other.receivedCardThisTurn);
        
        // Copy the strategy
        strategy = other.strategy;
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
    delete negotiatedPlayers;
    delete conqueredTerritoryThisTurn;
    delete receivedCardThisTurn;
    delete strategy;
    // Set to nullptr to prevent dangling pointers
    name = nullptr;
    territories = nullptr;
    hand = nullptr;
    orders = nullptr;
    reinforcementPool = nullptr;
    negotiatedPlayers = nullptr;
    conqueredTerritoryThisTurn = nullptr;
    receivedCardThisTurn = nullptr;
    strategy = nullptr;
    
}

// Stream insertion
std::ostream& operator<<(std::ostream& out, const Player& p) {
    out << "Player: " << *(p.name) << std::endl;
    out << "  Territories: " << p.territories->size() << std::endl;
     for (Territory* t : *p.territories)
        out << t->getName() << "(" << *t->armies << ") ";
    out << "\n";
    out << "  Hand: " << *(p.hand) << std::endl;
    out << "Reinforcement pool: " << *p.reinforcementPool << std::endl;
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
void  Player::addReinforcementPool(int n){*reinforcementPool+=n;}

// Add territory and mark the territory's owner as this player
void Player::addTerritory(Territory* t) {
    t->owner = this;
    territories->push_back(t);
    t->owner = this;
}
void Player::removeTerritory(Territory* t){
    territories->erase(
        std::remove(territories->begin(),territories->end(),t),territories->end());
}

bool Player::ownsTerritory(const Territory* t) const {
    if (!t) return false;
    for (Territory* terr : *territories) {
        if (terr == t) return true;
    }
    return false;
}

void Player::addNegotiatedPlayer(Player* other) {
    if (!other || other == this) return;
    for (Player* p : *negotiatedPlayers) {
        if (p == other) return;
    }
    negotiatedPlayers->push_back(other);
}

bool Player::isNegotiatingWith(const Player* other) const {
    if (!other) return false;
    for (Player* p : *negotiatedPlayers) {
        if (p == other) return true;
    }
    return false;
}

void Player::clearNegotiations() {
    negotiatedPlayers->clear();
}

void Player::markConqueredTerritory() {
    *conqueredTerritoryThisTurn = true;
}

bool Player::awardCardIfEligible(Deck* deck) {
    if (!deck) return false;
    if (*conqueredTerritoryThisTurn && !*receivedCardThisTurn) {
        deck->draw(hand);
        *receivedCardThisTurn = true;
        return true;
    }
    return false;
}

void Player::resetTurnFlags() {
    *conqueredTerritoryThisTurn = false;
    *receivedCardThisTurn = false;
}
// Next methods are arbitrary implementations 
// Defend method
// Delegates to the strategy
std::vector<Territory*>* Player::toDefend() {
    if (strategy) {
        return strategy->toDefend(this);
    }
    
    else{
        std::cout << "No strategy assigned to player " << *name << ". Cannot defend.\n";
        return nullptr;
    }
    
}

// Attack method
// Delegates to the strategy
std::vector<Territory*>* Player::toAttack() {
    if (strategy) {
        return strategy->toAttack(this);
    }
    else{
        std::cout << "No strategy assigned to player " << *name << ". Cannot attack.\n";
        return nullptr;
    }
}

/**
 * issueOrder: Delegates to the strategy's issueOrder method
 */
void Player::issueOrder(Deck* deck) {
    if (strategy) {
        strategy->issueOrder(this, deck);
    }
}

// Set the player's strategy
void Player::setStrategy(PlayerStrategy* newStrategy) {
    if (newStrategy != nullptr) {
        delete strategy;
        strategy = newStrategy;
    }
}

// Get the player's current strategy
PlayerStrategy* Player::getStrategy() const {
    return strategy;
}