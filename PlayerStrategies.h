#ifndef PLAYER_STRATEGIES_H
#define PLAYER_STRATEGIES_H

#include <string>
#include <vector>
#include <iostream>
#include "Map.h"

class Player;
class Deck;

/**
 * PlayerStrategy: Abstract base class defining the strategy interface.
 * Each ConcreteStrategy subclass implements different player behaviors
 * for issueOrder(), toAttack(), and toDefend().
 * PlayerStrategy is NOT a subclass of Player.
 */
class PlayerStrategy {
public:
    virtual ~PlayerStrategy() = default;

    virtual void issueOrder(Player* player, Deck* deck) = 0;
    virtual std::vector<Territory*>* toAttack(Player* player) = 0;
    virtual std::vector<Territory*>* toDefend(Player* player) = 0;
    virtual std::string strategyName() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const PlayerStrategy& ps);
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * HumanPlayerStrategy: Requires user interaction via cin to make decisions.
 */
class HumanPlayerStrategy : public PlayerStrategy {
public:
    HumanPlayerStrategy() = default;
    HumanPlayerStrategy(const HumanPlayerStrategy& other);
    HumanPlayerStrategy& operator=(const HumanPlayerStrategy& other);
    friend std::ostream& operator<<(std::ostream& os, const HumanPlayerStrategy& ps);

    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*>* toAttack(Player* player) override;
    std::vector<Territory*>* toDefend(Player* player) override;
    std::string strategyName() const override { return "Human"; }
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * AggressivePlayerStrategy: Deploys all armies on its strongest territory
 * (that has an adjacent enemy), then advances from every owned territory
 * to attack all adjacent enemies.
 */
class AggressivePlayerStrategy : public PlayerStrategy {
public:
    AggressivePlayerStrategy() = default;
    AggressivePlayerStrategy(const AggressivePlayerStrategy& other);
    AggressivePlayerStrategy& operator=(const AggressivePlayerStrategy& other);
    friend std::ostream& operator<<(std::ostream& os, const AggressivePlayerStrategy& ps);

    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*>* toAttack(Player* player) override;
    std::vector<Territory*>* toDefend(Player* player) override;
    std::string strategyName() const override { return "Aggressive"; }

private:
    Territory* findStrongestTerritory(Player* player);
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * BenevolentPlayerStrategy: Deploys all armies on its weakest territory,
 * advances armies to reinforce weak territories. Never attacks enemies.
 */
class BenevolentPlayerStrategy : public PlayerStrategy {
public:
    BenevolentPlayerStrategy() = default;
    BenevolentPlayerStrategy(const BenevolentPlayerStrategy& other);
    BenevolentPlayerStrategy& operator=(const BenevolentPlayerStrategy& other);
    friend std::ostream& operator<<(std::ostream& os, const BenevolentPlayerStrategy& ps);

    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*>* toAttack(Player* player) override;
    std::vector<Territory*>* toDefend(Player* player) override;
    std::string strategyName() const override { return "Benevolent"; }

private:
    Territory* findWeakestTerritory(Player* player);
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * NeutralPlayerStrategy: Never issues any orders.
 * Switched to AggressivePlayerStrategy by the attacker's issueOrder()
 * when an Advance order is queued against one of its territories.
 */
class NeutralPlayerStrategy : public PlayerStrategy {
public:
    NeutralPlayerStrategy() = default;
    NeutralPlayerStrategy(const NeutralPlayerStrategy& other);
    NeutralPlayerStrategy& operator=(const NeutralPlayerStrategy& other);
    friend std::ostream& operator<<(std::ostream& os, const NeutralPlayerStrategy& ps);

    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*>* toAttack(Player* player) override;
    std::vector<Territory*>* toDefend(Player* player) override;
    std::string strategyName() const override { return "Neutral"; }
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * CheaterPlayerStrategy: Automatically conquers all territories adjacent
 * to its own territories — once per turn, no battle needed.
 */
class CheaterPlayerStrategy : public PlayerStrategy {
public:
    CheaterPlayerStrategy() = default;
    CheaterPlayerStrategy(const CheaterPlayerStrategy& other);
    CheaterPlayerStrategy& operator=(const CheaterPlayerStrategy& other);
    friend std::ostream& operator<<(std::ostream& os, const CheaterPlayerStrategy& ps);

    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*>* toAttack(Player* player) override;
    std::vector<Territory*>* toDefend(Player* player) override;
    std::string strategyName() const override { return "Cheater"; }
};

#endif // PLAYER_STRATEGIES_H