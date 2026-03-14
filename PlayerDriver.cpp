#include "Player.h"
#include "Cards.h"
#include "Orders.h"
#include "Map.h"
#include <iostream>

/**
 * PlayerDriver: demonstrates the Player class interacting with the real
 * Map (Territory), Hand (Cards.h), and OrdersList (Orders.h).
 */
int main() {
    std::cout << "===== Player Driver Demo =====" << std::endl;

    // ---- Build a small map manually ----
    // Two continents, four territories, with adjacency between them.
    //
    //  [NorthAmerica]          [Asia]
    //   Canada -- USA   |   China -- Russia
    //      \________________/
    //        (cross-continent edge)

    Continent* northAmerica = new Continent("NorthAmerica", 5);
    Continent* asia         = new Continent("Asia", 7);

    Territory* canada = new Territory("Canada");
    Territory* usa    = new Territory("USA");
    Territory* china  = new Territory("China");
    Territory* russia = new Territory("Russia");

    // Assign territories to continents
    canada->setContinent(northAmerica);
    usa->setContinent(northAmerica);
    china->setContinent(asia);
    russia->setContinent(asia);

    // Set up adjacency (bidirectional)
    canada->addNeighbour(usa);
    usa->addNeighbour(canada);
    china->addNeighbour(russia);
    russia->addNeighbour(china);
    // Cross-continent edge so the map is a single connected graph
    usa->addNeighbour(china);
    china->addNeighbour(usa);

    // ---- Create Players ----
    Player* player1 = new Player("Alice");
    Player* player2 = new Player("Bob");

    // Assign territories — addTerritory also sets territory->owner
    player1->addTerritory(canada);
    player1->addTerritory(usa);
    player2->addTerritory(china);
    player2->addTerritory(russia);

    std::cout << "\nPlayers created and territories assigned." << std::endl;

    // ---- Hand ownership (draw cards from a shared deck) ----
    Deck* deck = new Deck(10);
    deck->draw(player1->getHand());
    deck->draw(player1->getHand());
    deck->draw(player2->getHand());
    deck->draw(player2->getHand());

    std::cout << "\nHands after drawing:" << std::endl;
    std::cout << "  " << *player1->getName() << ": " << *player1->getHand() << std::endl;
    std::cout << "  " << *player2->getName() << ": " << *player2->getHand() << std::endl;

    // ---- toDefend() ----
    std::cout << "\ntoDefend():" << std::endl;
    std::vector<Territory*>* defend1 = player1->toDefend();
    std::cout << "  " << *player1->getName() << " defends: ";
    for (Territory* t : *defend1) std::cout << t->getName() << " ";
    std::cout << std::endl;
    delete defend1;

    // ---- toAttack() — now uses real map neighbours ----
    std::cout << "\ntoAttack() (real map neighbours not owned by this player):" << std::endl;
    std::vector<Territory*>* attack1 = player1->toAttack();
    std::cout << "  " << *player1->getName() << " can attack: ";
    for (Territory* t : *attack1) std::cout << t->getName() << " ";
    std::cout << std::endl;
    delete attack1; // do NOT delete the Territory pointers — they belong to the map

    // ---- issueOrder() ----
    std::cout << "\nissueOrder() (creates a real Deploy order):" << std::endl;
    player1->issueOrder();
    player1->issueOrder();
    player2->issueOrder();

    std::cout << "\nPlayer1's OrdersList:" << std::endl;
    std::cout << *player1->getOrders() << std::endl;

    // ---- Copy constructor ----
    std::cout << "Copy constructor test:" << std::endl;
    Player player1Copy = *player1;
    std::cout << "  Original: " << *player1->getName()
              << " | Copy: "    << *player1Copy.getName() << std::endl;
    std::cout << "  Copy territories (" << player1Copy.getTerritories()->size() << "): ";
    for (Territory* t : *player1Copy.getTerritories()) std::cout << t->getName() << " ";
    std::cout << std::endl;

    // ---- Assignment operator ----
    std::cout << "\nAssignment operator test:" << std::endl;
    Player player3("TempPlayer");
    player3 = *player2;
    std::cout << "  After assignment, name: " << *player3.getName() << std::endl;
    std::cout << "  Territories (" << player3.getTerritories()->size() << "): ";
    for (Territory* t : *player3.getTerritories()) std::cout << t->getName() << " ";
    std::cout << std::endl;

    // ---- Stream insertion operator ----
    std::cout << "\nStream insertion operator:" << std::endl;
    std::cout << *player1 << std::endl;
    std::cout << *player2 << std::endl;

    // ---- Cleanup ----
    // Territory pointers were allocated here (not by a Map), so delete them manually.
    // Clear player territory vectors first so ~Player doesn't dangle.
    player1->getTerritories()->clear();
    player2->getTerritories()->clear();
    player1Copy.getTerritories()->clear();
    player3.getTerritories()->clear();

    delete player1;
    delete player2;
    delete deck;

    delete canada; delete usa; delete china; delete russia;
    delete northAmerica; delete asia;

    std::cout << "\n===== Player Driver Done =====" << std::endl;
    return 0;
}