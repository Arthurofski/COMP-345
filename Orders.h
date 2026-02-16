#ifndef ORDERS_H
#define ORDERS_H

#include <vector>
#include <string>


class Order {
private:
    std::string* _description;

public:
    Order();
    Order(const std::string description);
    Order(const Order& other);
    Order& operator=(const Order& other);
    virtual ~Order();

    std::string getDescription() const { return *_description; }
    void setDescription(std::string description) { *_description = description; }

    // Polymorphic copy
    virtual Order* clone() const = 0;

    // Core behavior
    virtual bool validate() const = 0;
    virtual void execute() = 0;

    // Accessors
    std::string getDescription() const;
};
/**
 * Stream insertion for any Order (polymorphic).
 */
std::ostream& operator<<(std::ostream& os, const Order& order);

class Deploy : public Order {
    private:
    int* _armies;                       
    std::string* _targetTerritoryName;  
public:
    Deploy();
    Deploy(int armies, const std::string& targetTerritoryName);
    Deploy(const Deploy& other);
    Deploy& operator=(const Deploy& other);
    ~Deploy() override;
    
    int getArmies() const { return *_armies; }
    std::string getTargetTerritoryName() const { return *_targetTerritoryName; }
    void setArmies(int armies) { *_armies = armies; }
    void setTargetTerritoryName(std::string targetTerritoryName) { *_targetTerritoryName = targetTerritoryName; }

    Order* clone() const override;
    bool validate() const override;
    void execute() override;


};
class Advance : public Order {
private:
    int* _armies;
    std::string* _sourceTerritoryName;
    std::string* _targetTerritoryName;

public:
    Advance();
    Advance(int armies, const std::string& sourceTerritoryName, const std::string& targetTerritoryName);
    Advance(const Advance& other);
    Advance& operator=(const Advance& other);
    ~Advance() override;

    Order* clone() const override;
    bool validate() const override;
    void execute() override;


};
class Bomb : public Order {
private:
    std::string* _targetTerritoryName;
public:
    Bomb();
    Bomb(const std::string& targetTerritoryName);
    Bomb(const Bomb& other);
    Bomb& operator=(const Bomb& other);
    ~Bomb() override;

    Order* clone() const override;
    bool validate() const override;
    void execute() override;


};

class Blockade : public Order {
    private:
    std::string* _targetTerritoryName;
public:
    Blockade();
    Blockade(const std::string& targetTerritoryName);
    Blockade(const Blockade& other);
    Blockade& operator=(const Blockade& other);
    ~Blockade() override;

    Order* clone() const override;
    bool validate() const override;
    void execute() override;


};

class Airlift : public Order {
private:
    int* _armies;
    std::string* _sourceTerritoryName;
    std::string* _targetTerritoryName;
public:
    Airlift();
    Airlift(int armies, const std::string& sourceTerritoryName, const std::string& targetTerritoryName);
    Airlift(const Airlift& other);
    Airlift& operator=(const Airlift& other);
    ~Airlift() override;

    Order* clone() const override;
    bool validate() const override;
    void execute() override;


};

class Negotiate : public Order {
private:
    std::string* _targetPlayerName;

public:
    Negotiate();
    Negotiate(const std::string& targetPlayerName);
    Negotiate(const Negotiate& other);
    Negotiate& operator=(const Negotiate& other);
    ~Negotiate() override;

    Order* clone() const override;
    bool validate() const override;
    void execute() override;


};

class OrdersList {
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


};

std::ostream& operator<<(std::ostream& os, const OrdersList& list);
#endif // ORDERS_H