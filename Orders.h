#ifndef ORDERS_H
#define ORDERS_H

#include <vector>
#include <string>


class Order {
private:
    int* _amount;
    std::string* _destination;

public:
    Order();
    Order(int amount, const std::string destination);
    Order(const Order& other);
    ~Order();
    void validate(Order* order);
    void execute();
};

class DeployOrder : Order {
public:
    DeployOrder();
    DeployOrder(int amount, const std::string destination);
    DeployOrder(const DeployOrder& other);
    ~DeployOrder();
};
class AttackOrder : Order {
private: 
    std::string* _origin;
public:
    AttackOrder();
    AttackOrder(int amount, const std::string origin, const std::string destination);
    AttackOrder(const AttackOrder& other);
    ~AttackOrder();
};

class OrderList{
    private:
        std::vector<Order*>* orderList;
    public:
        OrderList();
        OrderList(const OrderList& other);
        ~OrderList();
        void add(Order* order);
        void remove(Order* order);
        void move(Order* order, int newPosition);
};
#endif // ORDERS_H