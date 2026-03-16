#include "Orders.h"
#include "Player.h"
#include "Map.h"
#include "Cards.h"
#include <iostream>

using std::cout;
using std::endl;

static void printOwners(Territory* a, Territory* b, Territory* c) {
    cout << "Owners: "
         << a->getName() << "=" << (a->owner ? *a->owner->getName() : "None") << ", "
         << b->getName() << "=" << (b->owner ? *b->owner->getName() : "None") << ", "
         << c->getName() << "=" << (c->owner ? *c->owner->getName() : "None") << endl;
}

int main() {
    // Players and deck
    Player* alice = new Player("Alice");
    Player* bob = new Player("Bob");
    Deck* deck = new Deck(30);

    // Territories and adjacency
    Territory* alpha = new Territory("Alpha");
    Territory* beta = new Territory("Beta");
    Territory* gamma = new Territory("Gamma");
    Territory* delta = new Territory("Delta");

    alpha->addNeighbour(beta);
    beta->addNeighbour(alpha);
    beta->addNeighbour(gamma);
    gamma->addNeighbour(beta);
    alpha->addNeighbour(gamma);
    gamma->addNeighbour(alpha);

    // Ownership and armies
    alice->addTerritory(alpha);
    alice->addTerritory(beta);
    bob->addTerritory(gamma);
    bob->addTerritory(delta);

    *alpha->armies = 10;
    *beta->armies = 4;
    *gamma->armies = 1;
    *delta->armies = 6;

    alice->setReinforcementPool(8);

    cout << "=== Initial State ===" << endl;
    printOwners(alpha, beta, gamma);
    cout << "Armies: Alpha=" << *alpha->armies
         << " Beta=" << *beta->armies
         << " Gamma=" << *gamma->armies
         << " Delta=" << *delta->armies << endl;

    // 1) Validation and execution for each order type
    cout << "\n=== Validation + Execute per order ===" << endl;

    Deploy validDeploy(3, alice, alpha);
    Deploy invalidDeploy(3, alice, gamma);
    cout << "Deploy valid? " << validDeploy.validate() << endl;
    validDeploy.execute();
    cout << "Deploy invalid? " << invalidDeploy.validate() << endl;
    invalidDeploy.execute();

    Airlift validAirlift(2, alice, alpha, beta);
    Airlift invalidAirlift(2, alice, alpha, gamma);
    cout << "Airlift valid? " << validAirlift.validate() << endl;
    validAirlift.execute();
    cout << "Airlift invalid? " << invalidAirlift.validate() << endl;
    invalidAirlift.execute();

    Bomb validBomb(alice, gamma);
    Bomb invalidBomb(alice, alpha);
    cout << "Bomb valid? " << validBomb.validate() << endl;
    validBomb.execute();
    cout << "Bomb invalid? " << invalidBomb.validate() << endl;
    invalidBomb.execute();

    Blockade validBlockade(alice, beta);
    Blockade invalidBlockade(alice, gamma);
    cout << "Blockade valid? " << validBlockade.validate() << endl;
    validBlockade.execute();
    cout << "Blockade invalid? " << invalidBlockade.validate() << endl;
    invalidBlockade.execute();

    Negotiate validNegotiate(alice, bob);
    Negotiate invalidNegotiate(alice, alice);
    cout << "Negotiate valid? " << validNegotiate.validate() << endl;
    validNegotiate.execute();
    cout << "Negotiate invalid? " << invalidNegotiate.validate() << endl;
    invalidNegotiate.execute();

    // 2) Negotiate prevents attacks
    cout << "\n=== Negotiate blocks attacks ===" << endl;
    Advance blockedAttack(3, alice, alpha, gamma);
    blockedAttack.execute();

    // Clear negotiate and try attack again
    alice->clearNegotiations();
    bob->clearNegotiations();

    // 3) Conquest transfer by Advance
    cout << "\n=== Advance conquest ownership transfer ===" << endl;
    *alpha->armies = 12;
    *gamma->armies = 0;
    Advance conquestAttempt(6, alice, alpha, gamma);
    conquestAttempt.execute();
    printOwners(alpha, beta, gamma);

    // 4) Card awarded once per turn after at least one conquest
    cout << "\n=== One-card-per-turn award ===" << endl;
    int handBefore = alice->getHand()->getNumCards();
    bool awarded1 = alice->awardCardIfEligible(deck);
    bool awarded2 = alice->awardCardIfEligible(deck);
    int handAfter = alice->getHand()->getNumCards();
    cout << "Award #1: " << awarded1 << ", Award #2: " << awarded2
         << ", hand before=" << handBefore << ", hand after=" << handAfter << endl;

    // 5) Blockade ownership to Neutral
    cout << "\n=== Blockade transfers to Neutral ===" << endl;
    // If beta is no longer Alice's because of earlier blockade, make sure Alpha is Alice and use Alpha.
    if (alpha->owner != alice) {
        if (alpha->owner) alpha->owner->removeTerritory(alpha);
        alice->addTerritory(alpha);
    }
    Blockade neutralTransfer(alice, alpha);
    neutralTransfer.execute();
    cout << "Alpha owner after blockade: " << (alpha->owner ? *alpha->owner->getName() : "None") << endl;

    // 6) All orders issued by player and executed by engine-style pass
    cout << "\n=== Issue and execute all order kinds via OrdersList ===" << endl;

    // Re-establish some ownership for a clean issue/execute demo
    if (alpha->owner && alpha->owner != alice) alpha->owner->removeTerritory(alpha);
    if (beta->owner && beta->owner != alice) beta->owner->removeTerritory(beta);
    if (gamma->owner && gamma->owner != bob) gamma->owner->removeTerritory(gamma);
    if (delta->owner && delta->owner != bob) delta->owner->removeTerritory(delta);
    if (!alice->ownsTerritory(alpha)) alice->addTerritory(alpha);
    if (!alice->ownsTerritory(beta)) alice->addTerritory(beta);
    if (!bob->ownsTerritory(gamma)) bob->addTerritory(gamma);
    if (!bob->ownsTerritory(delta)) bob->addTerritory(delta);

    *alpha->armies = 9;
    *beta->armies = 3;
    *gamma->armies = 4;
    *delta->armies = 5;
    alice->setReinforcementPool(10);

    alice->getOrders()->add(new Deploy(4, alice, alpha));
    alice->getOrders()->add(new Advance(3, alice, alpha, gamma));
    alice->getOrders()->add(new Airlift(2, alice, alpha, beta));
    alice->getOrders()->add(new Bomb(alice, gamma));
    alice->getOrders()->add(new Blockade(alice, beta));
    alice->getOrders()->add(new Negotiate(alice, bob));

    while (alice->getOrders()->size() > 0) {
        Order* o = const_cast<Order*>(alice->getOrders()->get(0));
        o->execute();
        alice->getOrders()->remove(0);
    }

    cout << "\n=== Final State ===" << endl;
    printOwners(alpha, beta, gamma);
    cout << "Armies: Alpha=" << *alpha->armies
         << " Beta=" << *beta->armies
         << " Gamma=" << *gamma->armies
         << " Delta=" << *delta->armies << endl;

    delete deck;
    delete alice;
    delete bob;
    delete alpha;
    delete beta;
    delete gamma;
    delete delta;

    return 0;
}
