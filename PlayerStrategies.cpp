#include "PlayerStrategies.h"
#include "Player.h"
#include "Orders.h"
#include "Cards.h"
#include <iostream>
#include <algorithm>
#include <limits>
#include <vector>

using namespace std;

// ─── input helpers (used only by Human) ──────────────────────────────────────

static void flushCin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static int readInt(const string& prompt, int lo, int hi) {
    int v;
    while (true) {
        cout << prompt;
        if (cin >> v && v >= lo && v <= hi) { flushCin(); return v; }
        cout << "  Invalid - enter a number between " << lo << " and " << hi << ".\n";
        flushCin();
    }
}

// =============================================================================
// HumanPlayerStrategy
// =============================================================================

/**
 * Issues orders interactively:
 *   1. Deploy all reinforcements to a territory chosen by the user.
 *   2. Issue Advance orders in a loop until the user types 'd' (done).
 *   3. Optionally play a card.
 */
void HumanPlayerStrategy::issueOrder(Player* player, Deck* deck) {
    if (!player) return;

    // ── Step 1: Deploy ────────────────────────────────────────────────────────
    if (player->getReinforcementPool() > 0) {
        vector<Territory*>* defend = toDefend(player);

        cout << "\nDEPLOY (" << *player->getName() << ")\n";
        cout << "Reinforcement pool: " << player->getReinforcementPool() << " armies\n";
        cout << "Your territories:\n";
        for (int i = 0; i < (int)defend->size(); ++i)
            cout << " [" << i << "] " << defend->at(i)->getName()
                 << "  (" << *defend->at(i)->armies << " armies)\n";

        int choice = readInt("Deploy all armies to territory: ",
                             0, (int)defend->size() - 1);

        Territory* target = defend->at(choice);
        int pool = player->getReinforcementPool();
        player->getOrders()->add(new Deploy(pool, player, target));
        player->setReinforcementPool(0);
        cout << "Deploy queued: " << pool << " ==> " << target->getName() << "\n";
        delete defend;
    }

    // ── Step 2: Advance orders (loop until 'd') ───────────────────────────────
    while (true) {
        cout << "\nADVANCE (" << *player->getName() << ")\n";
        cout << "[y] issue Advance   [d] done\n";
        char ans;
        cin >> ans; flushCin();

        if (ans == 'd' || ans == 'D') break;
        if (ans != 'y' && ans != 'Y') continue;

        // Choose source
        vector<Territory*>* myTerrs = toDefend(player);
        if (myTerrs->empty()) { delete myTerrs; break; }

        cout << "\n  Your territories (source):\n";
        for (int i = 0; i < (int)myTerrs->size(); ++i)
            cout << "    [" << i << "] " << myTerrs->at(i)->getName()
                 << "  (" << *myTerrs->at(i)->armies << " armies)\n";

        int srcIdx = readInt("  Source [0-" + to_string((int)myTerrs->size()-1) + "]: ",
                             0, (int)myTerrs->size()-1);
        Territory* source = myTerrs->at(srcIdx);
        delete myTerrs;

        // Show neighbours
        vector<Territory*>& nbrs = *source->neighbours;
        if (nbrs.empty()) {
            cout << "  " << source->getName() << " has no neighbours.\n";
            continue;
        }
        cout << "\nNeighbours of " << source->getName() << ":\n";
        for (int i = 0; i < (int)nbrs.size(); ++i) {
            string owner = nbrs[i]->owner ? *nbrs[i]->owner->getName() : "Nobody";
            cout << "    [" << i << "] " << nbrs[i]->getName()
                 << "  owner: " << owner
                 << "  armies: " << *nbrs[i]->armies << "\n";
        }
        int dstIdx = readInt("Destination [0-" + to_string((int)nbrs.size()-1) + "]: ",
                             0, (int)nbrs.size()-1);
        Territory* dest = nbrs[dstIdx];

        int maxA = *source->armies - 1;
        if (maxA <= 0) { cout << "Not enough armies to advance.\n"; continue; }

        int num = readInt("Armies to advance [1-" + to_string(maxA) + "]: ", 1, maxA);
        player->getOrders()->add(new Advance(num, player, source, dest));
        cout << "Advance queued: " << num << "  "
             << source->getName() << " ==> " << dest->getName() << "\n";
    }

    // ── Step 3: Play cards ───────────────────────────────────────────────────
    while (player->getHand()->getNumCards() > 0) {
        int numCards = player->getHand()->getNumCards();

        cout << "\nCARDS (" << *player->getName() << ")\n";
        cout << "Your hand:\n";
        for (int i = 0; i < numCards; ++i) {
            Cards* c = player->getHand()->getCard(i);
            cout << "[" << i << "] " << *c << "\n";
        }
        cout << "Play a card? [y] yes   [n] skip cards\n";
        char ans;
        cin >> ans; flushCin();

        if (ans != 'y' && ans != 'Y') break;

        // Ask which card to play
        int idx = readInt("  Which card [0-" + to_string(numCards - 1) + "]: ",
                          0, numCards - 1);
        Cards* card = player->getHand()->getCard(idx);
        if (!card) { cout << "  Invalid card.\n"; continue; }

        // Cards::play() handles: creating the order, adding it to the
        // player's order list, removing the card from hand, returning
        // it to the deck.
        card->play(player->getHand(), deck, player);
        cout << "Card played. Orders updated.\n";
    }
}

/**
 * Returns all territories adjacent to the player's territories that are
 * owned by another player (attack candidates).
 */
vector<Territory*>* HumanPlayerStrategy::toAttack(Player* player) {
    auto* r = new vector<Territory*>();
    if (!player) return r;
    for (Territory* owned : *player->getTerritories())
        for (Territory* nb : *owned->neighbours)
            if (nb->owner != player &&
                find(r->begin(), r->end(), nb) == r->end())
                r->push_back(nb);
    return r;
}

/** Returns all territories owned by the player. */
vector<Territory*>* HumanPlayerStrategy::toDefend(Player* player) {
    auto* r = new vector<Territory*>();
    if (!player) return r;
    for (Territory* t : *player->getTerritories()) r->push_back(t);
    return r;
}

// =============================================================================
// AggressivePlayerStrategy
// =============================================================================

/**
 * Returns the owned territory with the most armies that also has at least
 * one adjacent enemy neighbour to attack.  If no such territory exists
 * (e.g. the player is completely surrounded by their own territories),
 * falls back to simply the strongest territory overall.
 */
Territory* AggressivePlayerStrategy::findStrongestTerritory(Player* player) {
    if (!player || player->getTerritories()->empty()) return nullptr;

    // Helper: does this territory have at least one enemy neighbour?
    auto hasEnemyNeighbour = [&](Territory* t) {
        for (Territory* nb : *t->neighbours)
            if (nb->owner != player) return true;
        return false;
    };

    // First pass: strongest territory that can actually attack someone
    Territory* best = nullptr;
    for (Territory* t : *player->getTerritories()) {
        if (!hasEnemyNeighbour(t)) continue;
        if (!best || *t->armies > *best->armies) best = t;
    }

    // Fallback: no territory borders an enemy — just return the strongest overall
    if (!best) {
        best = player->getTerritories()->front();
        for (Territory* t : *player->getTerritories())
            if (*t->armies > *best->armies) best = t;
    }

    return best;
}

/**
 * Deploys all reinforcements on the strongest territory, then advances
 * from that territory to every adjacent enemy territory.
 */
void AggressivePlayerStrategy::issueOrder(Player* player, Deck* deck) {
    if (!player) return;

    // Deploy all on strongest
    if (player->getReinforcementPool() > 0) {
        Territory* strongest = findStrongestTerritory(player);
        if (!strongest) return;
        int amount = player->getReinforcementPool();
        player->getOrders()->add(new Deploy(amount, player, strongest));
        player->setReinforcementPool(0);
        cout << "[" << *player->getName() << " (Aggressive)] Deploy "
             << amount << " ==> " << strongest->getName() << "\n";
    }

    // Advance from EVERY owned territory to all its adjacent enemies.
    // If the target territory belongs to a Neutral player, switch them to
    // Aggressive immediately — they are being attacked.
    for (Territory* source : *player->getTerritories()) {
        if (*source->armies <= 1) continue;  // keep at least 1 army behind
        for (Territory* nb : *source->neighbours) {
            if (nb->owner != player) {
                // Neutral player becomes Aggressive when targeted
                if (nb->owner && nb->owner->getStrategy()->strategyName() == "Neutral") {
                    cout << "[Game] " << *nb->owner->getName()
                         << " is being attacked — switching Neutral ==> Aggressive!\n";
                    nb->owner->setStrategy(new AggressivePlayerStrategy());
                }
                int armies = *source->armies - 1;
                player->getOrders()->add(new Advance(armies, player, source, nb));
                cout << "[" << *player->getName() << " (Aggressive)] Advance "
                     << armies << "  " << source->getName()
                     << " ==> " << nb->getName() << "\n";
            }
        }
    }
}

/** Returns all adjacent enemy territories. */
vector<Territory*>* AggressivePlayerStrategy::toAttack(Player* player) {
    auto* r = new vector<Territory*>();
    if (!player) return r;
    for (Territory* owned : *player->getTerritories())
        for (Territory* nb : *owned->neighbours)
            if (nb->owner != player &&
                find(r->begin(), r->end(), nb) == r->end())
                r->push_back(nb);
    return r;
}

/** Returns all owned territories. */
vector<Territory*>* AggressivePlayerStrategy::toDefend(Player* player) {
    auto* r = new vector<Territory*>();
    if (player) for (Territory* t : *player->getTerritories()) r->push_back(t);
    return r;
}

// =============================================================================
// BenevolentPlayerStrategy
// =============================================================================

/** Returns the owned territory with the fewest armies. */
Territory* BenevolentPlayerStrategy::findWeakestTerritory(Player* player) {
    if (!player || player->getTerritories()->empty()) return nullptr;
    Territory* best = player->getTerritories()->front();
    for (Territory* t : *player->getTerritories())
        if (*t->armies < *best->armies) best = t;
    return best;
}

/**
 * Deploys all reinforcements on the weakest territory, then advances
 * armies from strong friendly neighbours to reinforce the weakest.
 * Never issues an order that attacks an enemy territory.
 */
void BenevolentPlayerStrategy::issueOrder(Player* player, Deck* deck) {
    if (!player) return;

    // Deploy all on weakest
    if (player->getReinforcementPool() > 0) {
        Territory* weakest = findWeakestTerritory(player);
        if (!weakest) return;
        int amount = player->getReinforcementPool();
        player->getOrders()->add(new Deploy(amount, player, weakest));
        player->setReinforcementPool(0);
        cout << "[" << *player->getName() << " (Benevolent)] Deploy "
             << amount << " ==> " << weakest->getName() << "\n";
    }

    // Reinforce weakest from friendly neighbours
    if (player->getTerritories()->size() > 1) {
        Territory* weakest = findWeakestTerritory(player);
        if (weakest) {
            for (Territory* nb : *weakest->neighbours) {
                if (nb->owner == player && *nb->armies > 1) {
                    int armies = *nb->armies / 2;
                    if (armies > 0) {
                        player->getOrders()->add(new Advance(armies, player, nb, weakest));
                        cout << "[" << *player->getName() << " (Benevolent)] Reinforce "
                             << armies << "  " << nb->getName()
                             << " ==> " << weakest->getName() << "\n";
                    }
                }
            }
        }
    }
}

/** Benevolent never attacks — always returns empty list. */
vector<Territory*>* BenevolentPlayerStrategy::toAttack(Player* player) {
    return new vector<Territory*>();
}

/** Returns all owned territories, sorted weakest first. */
vector<Territory*>* BenevolentPlayerStrategy::toDefend(Player* player) {
    auto* r = new vector<Territory*>();
    if (!player) return r;
    for (Territory* t : *player->getTerritories()) r->push_back(t);
    sort(r->begin(), r->end(),
         [](Territory* a, Territory* b){ return *a->armies < *b->armies; });
    return r;
}

// =============================================================================
// NeutralPlayerStrategy
// =============================================================================

/**
 * Issues no orders. The game engine is responsible for detecting that this
 * player was attacked and switching their strategy to AggressivePlayerStrategy.
 */
void NeutralPlayerStrategy::issueOrder(Player* player, Deck* deck) {
    if (!player) return;
    cout << "[" << *player->getName() << " (Neutral)] Takes no action this turn.\n";
}

/** Neutral never attacks — always returns empty list. */
vector<Territory*>* NeutralPlayerStrategy::toAttack(Player* player) {
    return new vector<Territory*>();
}

/** Returns all owned territories. */
vector<Territory*>* NeutralPlayerStrategy::toDefend(Player* player) {
    auto* r = new vector<Territory*>();
    if (player) for (Territory* t : *player->getTerritories()) r->push_back(t);
    return r;
}

// =============================================================================
// CheaterPlayerStrategy
// =============================================================================

void CheaterPlayerStrategy::issueOrder(Player* player, Deck* deck) {
    if (!player) return;

    // Snapshot the target list BEFORE modifying ownership so that newly
    // acquired territories don't chain-expand the frontier mid-loop.
    vector<Territory*> toConquer;
    for (Territory* owned : *player->getTerritories())
        for (Territory* nb : *owned->neighbours)
            if (nb->owner != player &&
                find(toConquer.begin(), toConquer.end(), nb) == toConquer.end())
                toConquer.push_back(nb);

    if (toConquer.empty()) {
        cout << "[" << *player->getName() << " (Cheater)] No adjacent enemies.\n";
        return;
    }

    for (Territory* t : toConquer) {
        if (t->owner) t->owner->removeTerritory(t);
        t->setOwner(player);
        player->addTerritory(t);
        cout << "[" << *player->getName() << " (Cheater)] Conquered "
             << t->getName() << "!\n";
    }
}

/** Returns all adjacent enemy territories (informational). */
vector<Territory*>* CheaterPlayerStrategy::toAttack(Player* player) {
    auto* r = new vector<Territory*>();
    if (!player) return r;
    for (Territory* owned : *player->getTerritories())
        for (Territory* nb : *owned->neighbours)
            if (nb->owner != player &&
                find(r->begin(), r->end(), nb) == r->end())
                r->push_back(nb);
    return r;
}

/** Returns all owned territories. */
vector<Territory*>* CheaterPlayerStrategy::toDefend(Player* player) {
    auto* r = new vector<Territory*>();
    if (player) for (Territory* t : *player->getTerritories()) r->push_back(t);
    return r;
}

// =============================================================================
// Copy constructors, assignment operators, stream insertion operators

// ── Base class stream insertion ───────────────────────────────────────────────
ostream& operator<<(ostream& os, const PlayerStrategy& ps) {
    os << "PlayerStrategy: " << ps.strategyName();
    return os;
}

// ── HumanPlayerStrategy ───────────────────────────────────────────────────────
HumanPlayerStrategy::HumanPlayerStrategy(const HumanPlayerStrategy& other) {
    // No data members to copy
}

HumanPlayerStrategy& HumanPlayerStrategy::operator=(const HumanPlayerStrategy& other) {
    // No data members to copy; self-assignment safe by default
    return *this;
}

ostream& operator<<(ostream& os, const HumanPlayerStrategy& ps) {
    os << "PlayerStrategy: " << ps.strategyName();
    return os;
}

// ── AggressivePlayerStrategy ──────────────────────────────────────────────────
AggressivePlayerStrategy::AggressivePlayerStrategy(const AggressivePlayerStrategy& other) {
    // No data members to copy
}

AggressivePlayerStrategy& AggressivePlayerStrategy::operator=(const AggressivePlayerStrategy& other) {
    return *this;
}

ostream& operator<<(ostream& os, const AggressivePlayerStrategy& ps) {
    os << "PlayerStrategy: " << ps.strategyName();
    return os;
}

// ── BenevolentPlayerStrategy ──────────────────────────────────────────────────
BenevolentPlayerStrategy::BenevolentPlayerStrategy(const BenevolentPlayerStrategy& other) {
    // No data members to copy
}

BenevolentPlayerStrategy& BenevolentPlayerStrategy::operator=(const BenevolentPlayerStrategy& other) {
    return *this;
}

ostream& operator<<(ostream& os, const BenevolentPlayerStrategy& ps) {
    os << "PlayerStrategy: " << ps.strategyName();
    return os;
}

// ── NeutralPlayerStrategy ─────────────────────────────────────────────────────
NeutralPlayerStrategy::NeutralPlayerStrategy(const NeutralPlayerStrategy& other) {
    // No data members to copy
}

NeutralPlayerStrategy& NeutralPlayerStrategy::operator=(const NeutralPlayerStrategy& other) {
    return *this;
}

ostream& operator<<(ostream& os, const NeutralPlayerStrategy& ps) {
    os << "PlayerStrategy: " << ps.strategyName();
    return os;
}

// ── CheaterPlayerStrategy ─────────────────────────────────────────────────────
CheaterPlayerStrategy::CheaterPlayerStrategy(const CheaterPlayerStrategy& other) {
    // No data members to copy
}

CheaterPlayerStrategy& CheaterPlayerStrategy::operator=(const CheaterPlayerStrategy& other) {
    return *this;
}

ostream& operator<<(ostream& os, const CheaterPlayerStrategy& ps) {
    os << "PlayerStrategy: " << ps.strategyName();
    return os;
}