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
class OrdersList;
class Order;
class Territory;



// actual Player class
class Player {
public:
    Player(std::string name);
    Player(const Player& other);
    Player& operator=(const Player& other);
    friend std::ostream& operator<<(std::ostream& out, const Player& p);
    ~Player();

    // Defend/Attack and issue order methods
    std::vector<Territory*>* toDefend();
    std::vector<Territory*>* toAttack();
    void issueOrder(Deck* deck);
    //setters and getters
    std::string* getName() const;
    std::vector<Territory*>* getTerritories() const;
    Hand* getHand() const;
    OrdersList* getOrders() const;
    int getReinforcementPool() const;
    void setReinforcementPool(int pool);

    void setReinforcementPool(int n);
    void addReinforcementPool(int n);
    void addTerritory(Territory* t);
    void removeTerritory(Territory *t);

private:
    std::string* name;
    std::vector<Territory*>* territories;
    Hand* hand;
    OrdersList* orders;
    int* reinforcementPool;
};

#endif
