#ifndef ORDERS_H
#define ORDERS_H

#include "LoggingObserver.h"
#include <vector>
#include <string>
#include <ostream>



class Order : public Subject, public ILoggable {
private:
    std::string* _description;

public:
    Order();
    Order(const std::string description);
    Order(const Order& other);
    Order& operator=(const Order& other);
    virtual ~Order();

    std::string getDescription() const;
    void setDescription(std::string description);
    std::string stringToLog() const override;

    // Polymorphic cloning (virtual constructor) for deep copying via base class pointer.
    virtual Order* clone() const = 0;

    // Validation and execution are separate steps; execute should only do something if validate() returns true.
    virtual bool validate() const = 0;
    virtual void execute() = 0;
};
/**
 * Stream insertion for any Order (polymorphic).
 */
std::ostream& operator<<(std::ostream& os, const Order& order);

// Forward declarations so Deploy can reference Player and Territory without
// a heavyweight include chain inside the header.
class Player;
class Territory;

// Deploy armies from issuer reinforcement pool to one owned territory.
class Deploy : public Order {
private:
    int* _armies;
    Player*    _player;    // non-owning: the player who issued this order
    Territory* _territory; // non-owning: the target territory
public:
    Deploy();
    Deploy(int armies, Player* player, Territory* territory);
    Deploy(const Deploy& other);
    Deploy& operator=(const Deploy& other);
    ~Deploy() override;

    int        getArmies()    const;
    Player*    getPlayer()    const;
    Territory* getTerritory() const;
    void setArmies(int armies);

    Order* clone()        const override;
    bool   validate()     const override;
    void   execute()            override;
};

// Move armies between adjacent territories; can trigger combat and conquest.
class Advance : public Order {
private:
    int* _armies;
    Player* _player;      // non-owning issuer
    Territory* _source;   // non-owning source territory
    Territory* _target;   // non-owning target territory

public:
    Advance();
    Advance(int armies, Player* player, Territory* source, Territory* target);
    Advance(const Advance& other);
    Advance& operator=(const Advance& other);
    ~Advance() override;

    Order* clone() const override;
    bool validate() const override;
    void execute() override;


};

// Halve armies on an adjacent enemy territory.
class Bomb : public Order {
private:
    Player* _player;      // non-owning issuer
    Territory* _target;   // non-owning target territory
public:
    Bomb();
    Bomb(Player* player, Territory* targetTerritory);
    Bomb(const Bomb& other);
    Bomb& operator=(const Bomb& other);
    ~Bomb() override;

    Order* clone() const override;
    bool validate() const override;
    void execute() override;


};

// Double armies on one owned territory and transfer it to Neutral.
class Blockade : public Order {
private:
    Player* _player;      // non-owning issuer
    Territory* _target;   // non-owning target territory
public:
    Blockade();
    Blockade(Player* player, Territory* targetTerritory);
    Blockade(const Blockade& other);
    Blockade& operator=(const Blockade& other);
    ~Blockade() override;

    Order* clone() const override;
    bool validate() const override;
    void execute() override;


};

// Move armies between two owned territories without adjacency requirement.
class Airlift : public Order {
private:
    int* _armies;
    Player* _player;      // non-owning issuer
    Territory* _source;   // non-owning source territory
    Territory* _target;   // non-owning target territory
public:
    Airlift();
    Airlift(int armies, Player* player, Territory* sourceTerritory, Territory* targetTerritory);
    Airlift(const Airlift& other);
    Airlift& operator=(const Airlift& other);
    ~Airlift() override;

    Order* clone() const override;
    bool validate() const override;
    void execute() override;


};

// Prevent attacks between issuer and target player for the current turn.
class Negotiate : public Order {
private:
    Player* _player;      // non-owning issuer
    Player* _targetPlayer; // non-owning target player

public:
    Negotiate();
    Negotiate(Player* player, Player* targetPlayer);
    Negotiate(const Negotiate& other);
    Negotiate& operator=(const Negotiate& other);
    ~Negotiate() override;

    Order* clone() const override;
    bool validate() const override;
    void execute() override;


};

class OrdersList : public Subject, public ILoggable {
private:
    void clear();
    void copyFrom(const OrdersList& other);

    std::vector<Order*>* _orders;  // user-defined type => pointer
public:
    OrdersList();
    OrdersList(const OrdersList& other);
    OrdersList& operator=(const OrdersList& other);
    ~OrdersList();

    void add(Order* order);        // takes ownership
    bool remove(int index);        // deletes removed order
    bool move(int from, int to);

    int size() const;
    const Order* get(int index) const;
    std::string stringToLog() const override;

    friend std::ostream& operator<<(std::ostream& os, const OrdersList& list);

};

#endif