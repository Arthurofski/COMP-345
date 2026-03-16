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
    negotiatedPlayers = new std::vector<Player*>();
    conqueredTerritoryThisTurn = new bool(false);
    receivedCardThisTurn = new bool(false);
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
    // Set to nullptr to prevent dangling pointers
    name = nullptr;
    territories = nullptr;
    hand = nullptr;
    orders = nullptr;
    reinforcementPool = nullptr;
    negotiatedPlayers = nullptr;
    conqueredTerritoryThisTurn = nullptr;
    receivedCardThisTurn = nullptr;
    
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
 * Reinforcement pool >0 then we issue deploy order, none other order can go through
 * once pool empty, issue an advance order
 * if card in hand, can  play card 
 */
void Player::issueOrder(Deck* deck) {
    // --- Phase 1: Deploy all reinforcements first ---
    if (*reinforcementPool > 0) {
        std::vector<Territory*>* defend = toDefend();
 
        if (!defend->empty()) {
            // Distribute one army per call to the territory with the fewest armies
            Territory* weakest = defend->front();
            for (Territory* t : *defend)
                if (*t->armies < *weakest->armies) weakest = t;
 
            int toDeploy = 1;  // deploy one army per call (game engine calls per army)
            orders->add(new Deploy(toDeploy,this, weakest));
            *reinforcementPool -= toDeploy;
            std::cout << "  [" << *name << "] Deploy " << toDeploy
                      << " to " << weakest->getName()
                      << " (pool remaining: " << *reinforcementPool << ")\n";
        }
        delete defend;
        return;   // no other orders while pool > 0
    }
 
    // --- Phase 2: Advance (attack or defend) ---
    std::vector<Territory*>* attack = toAttack();
    std::vector<Territory*>* defend = toDefend();
 
    bool issuedAdvance = false;
 
    if (!attack->empty() && !defend->empty()) {
        // Find the owned territory with the most armies to attack from
        Territory* src = defend->front();
        for (Territory* t : *defend)
            if (*t->armies > *src->armies) src = t;
 
        Territory* tgt = attack->front();  // attack the first reachable enemy
 
        if (*src->armies > 1) {
            int attacking = *src->armies / 2;
            orders->add(new Advance(attacking, this, src, tgt));   
            std::cout << "  [" << *name << "] Advance " << attacking
                      << " from " << src->getName() << " -> " << tgt->getName() << "\n";
            issuedAdvance = true;
        }
    } else if (defend->size() >= 2) {
        // No enemies reachable — move armies to bolster a weaker own territory
        Territory* strongest = defend->front();
        Territory* weakest   = defend->front();
        for (Territory* t : *defend) {
            if (*t->armies > *strongest->armies) strongest = t;
            if (*t->armies < *weakest->armies)   weakest   = t;
        }
        if (strongest != weakest && *strongest->armies > 1) {
            int moving = *strongest->armies / 2;
            orders->add(new Advance(moving, this, strongest, weakest));
            std::cout << "  [" << *name << "] Reinforce " << moving
                      << " from " << strongest->getName()
                      << " -> " << weakest->getName() << "\n";
            issuedAdvance = true;
        }
    }
 
    // --- Phase 3: Play a card if one is available ---
    if (hand->getNumCards() > 0 && deck) {
        Cards* card = hand->getCards()->front();
        card->play(hand, deck, this);
    }
 
    delete attack;
    delete defend;
 
    if (!issuedAdvance && hand->getNumCards() == 0) {
        std::cout << "  [" << *name << "] No orders to issue this round.\n";
    }
}