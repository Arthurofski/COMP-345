#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "Map.h"
// For Assignment 1, using placeholder classes
// Unfinished classes for compilation only
class Hand;
class Deck;
class OrdersList;
class Order;
class Territory;
class PlayerStrategy;



// actual Player class
class Player {
public:
    Player(std::string name);
    Player(const Player& other);
    Player& operator=(const Player& other);
    friend std::ostream& operator<<(std::ostream& out, const Player& p);
    ~Player();

    // Defend/Attack and issue order methods
    //delegate to the PlayerStrategy
    std::vector<Territory*>* toDefend();
    std::vector<Territory*>* toAttack();
    void issueOrder(Deck* deck);
    
    // Strategy getter/setter
    void setStrategy(PlayerStrategy* newStrategy);
    PlayerStrategy* getStrategy() const;
    //setters and getters
    std::string* getName() const;
    std::vector<Territory*>* getTerritories() const;
    Hand* getHand() const;
    OrdersList* getOrders() const;
    int getReinforcementPool() const;
    void setReinforcementPool(int pool);
    void addReinforcementPool(int n);
    void addTerritory(Territory* t);
    void removeTerritory(Territory *t);
    bool ownsTerritory(const Territory* t) const;

    void addNegotiatedPlayer(Player* other);
    bool isNegotiatingWith(const Player* other) const;
    void clearNegotiations();

    void markConqueredTerritory();
    bool awardCardIfEligible(Deck* deck);
    void resetTurnFlags();

private:
    std::string* name;
    std::vector<Territory*>* territories;
    Hand* hand;
    OrdersList* orders;
    int* reinforcementPool;
    std::vector<Player*>* negotiatedPlayers;
    bool* conqueredTerritoryThisTurn;
    bool* receivedCardThisTurn;
    PlayerStrategy* strategy;  // Strategy for issuing orders and making decisions
};

#endif
