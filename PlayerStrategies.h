#ifndef PLAYER_STRATEGIES_H
#define PLAYER_STRATEGIES_H

#include <string>
#include <vector>
#include "Map.h"

// Forward declarations
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

    // Issues one or more orders for this turn
    virtual void issueOrder(Player* player, Deck* deck) = 0;

    // Returns the list of territories this strategy prioritizes attacking
    virtual std::vector<Territory*>* toAttack(Player* player) = 0;

    // Returns the list of territories this strategy prioritizes defending
    virtual std::vector<Territory*>* toDefend(Player* player) = 0;

    // Returns the name of the strategy for display
    virtual std::string strategyName() const = 0;
};

/**
 * HumanPlayerStrategy: Requires user interaction via cin to make decisions.
 */
class HumanPlayerStrategy : public PlayerStrategy {
public:
    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*>* toAttack(Player* player) override;
    std::vector<Territory*>* toDefend(Player* player) override;
    std::string strategyName() const override { return "Human"; }
};

/**
 * AggressivePlayerStrategy: Deploys all armies on its strongest territory,
 * then advances to attack every adjacent enemy until it cannot do so anymore.
 */
class AggressivePlayerStrategy : public PlayerStrategy {
public:
    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*>* toAttack(Player* player) override;
    std::vector<Territory*>* toDefend(Player* player) override;
    std::string strategyName() const override { return "Aggressive"; }

private:
    Territory* findStrongestTerritory(Player* player);
};

/**
 * BenevolentPlayerStrategy: Deploys all armies on its weakest territory,
 * advances armies to reinforce weak territories. Never attacks enemies.
 */
class BenevolentPlayerStrategy : public PlayerStrategy {
public:
    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*>* toAttack(Player* player) override;  // always empty
    std::vector<Territory*>* toDefend(Player* player) override;
    std::string strategyName() const override { return "Benevolent"; }

private:
    Territory* findWeakestTerritory(Player* player);
};

/**
 * NeutralPlayerStrategy: Never issues any orders.
 * If attacked, the game engine switches this player to AggressivePlayerStrategy.
 */
class NeutralPlayerStrategy : public PlayerStrategy {
public:
    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*>* toAttack(Player* player) override;  // always empty
    std::vector<Territory*>* toDefend(Player* player) override;
    std::string strategyName() const override { return "Neutral"; }
};

/**
 * CheaterPlayerStrategy: Automatically conquers all territories adjacent
 * to its own territories — once per turn, no battle needed.
 */
class CheaterPlayerStrategy : public PlayerStrategy {
public:
    void issueOrder(Player* player, Deck* deck) override;
    std::vector<Territory*>* toAttack(Player* player) override;
    std::vector<Territory*>* toDefend(Player* player) override;
    std::string strategyName() const override { return "Cheater"; }
};

#endif // PLAYER_STRATEGIES_H