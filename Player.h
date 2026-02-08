#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <iostream>

// Forward declarations 
class Territory;
class Hand;
class OrderList;  
class Order;      

class Player {
public:
    Player(std::string name);
    Player(const Player& other);
    Player& operator=(const Player& other);
    
    friend std::ostream& operator<<(std::ostream& out, const Player& p);
    
    ~Player();

    std::string* getName() const;
    std::vector<Territory*>* getTerritories() const;
    Hand* getHand() const;
    OrderList* getOrders() const;

    void addTerritory(Territory* t);
    
    // Required methods
    std::vector<Territory*>* toDefend();
    std::vector<Territory*>* toAttack();
    void issueOrder();

private:
    std::string* name;
    std::vector<Territory*>* territories;
    Hand* hand;
    OrderList* orders;
};

#endif
