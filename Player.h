#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <iostream>

// For Assignment 1, using placeholder classes
// Unfinished classes for compilation only

class Territory {
public:
    Territory(std::string name) : name(name) {}
    std::string getName() const { return name; }
private:
    std::string name;
};

class Hand {
public:
    Hand() {}
    friend std::ostream& operator<<(std::ostream& out, const Hand& h) {
        out << "[Hand placeholder]";
        return out;
    }
};

class OrderList {
public:
    OrderList() {}
    void add(class Order* order) { /* placeholder */ }
    friend std::ostream& operator<<(std::ostream& out, const OrderList& ol) {
        out << "[OrderList placeholder]";
        return out;
    }
};

class Order {
public:
    Order() {}
};


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
    void issueOrder();

    std::string* getName() const;
    std::vector<Territory*>* getTerritories() const;
    Hand* getHand() const;
    OrderList* getOrders() const;

    void addTerritory(Territory* t);

private:
    std::string* name;
    std::vector<Territory*>* territories;
    Hand* hand;
    OrderList* orders;
};

#endif
