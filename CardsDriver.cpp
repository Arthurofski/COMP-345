#include "Cards.h"
#include "Orders.h"
#include "Map.h"
#include <iostream>

using namespace std;

/**
 * CardsDriver: demonstrates the Deck and Hand classes, with Cards::play()
 * creating real Orders that land in the player's real OrdersList.
 */
int main() {
    cout << "===== Cards Driver Demo =====" << endl;

    // Build a minimal map so the player has real territories
    Territory* alaska  = new Territory("Alaska");
    Territory* ontario = new Territory("Ontario");
    Continent* na      = new Continent("NorthAmerica", 5);
    alaska->setContinent(na);
    ontario->setContinent(na);
    alaska->addNeighbour(ontario);
    ontario->addNeighbour(alaska);

    // Create a player and assign territories
    Player* player = new Player("CardPlayer");
    player->addTerritory(alaska);
    player->addTerritory(ontario);

    // Create a full deck (one of each type, cycling through 5 types)
    Deck* deck = new Deck(5);
    Hand* hand = new Hand();

    cout << "\nInitial deck:\n" << *deck << endl;

    // Draw all 5 cards into hand
    cout << "Drawing 5 cards from deck into hand..." << endl;
    for (int i = 0; i < 5; i++) deck->draw(hand);

    cout << "\nDeck after drawing:\n" << *deck << endl;
    cout << "Hand after drawing:\n" << *hand << endl;

    // Play all cards — each creates a real Order in player's OrdersList,
    // then the card moves back to the deck.
    cout << "\nPlaying all cards from hand:" << endl;
    vector<Cards*> toPlay(*hand->getCards());
    for (Cards* card : toPlay) {
        card->play(hand, deck, player);
    }

    cout << "\nDeck after playing (cards returned):\n" << *deck << endl;
    cout << "Hand after playing (should be empty):\n" << *hand << endl;
    cout << "\nPlayer's OrdersList after playing all cards:\n"
         << *player->getOrders() << endl;

    // Cleanup — clear territory vector before deleting player so ~Player
    // doesn't hold dangling pointers to stack-freed territories.
    player->getTerritories()->clear();
    delete player;
    delete hand;
    delete deck;
    delete alaska;
    delete ontario;
    delete na;

    cout << "===== Cards Driver Done =====" << endl;
    return 0;
}