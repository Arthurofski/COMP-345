#ifndef CARDS_H
#define CARDS_H

#include <iostream>
#include <vector>
#include "Player.h"  // gives us Territory, and forward-declared Hand/OrdersList/Order

// Forward declarations (Hand and Deck reference each other)
class Cards;
class Deck;
class Hand;

/**
 * The five card types available in Warzone.
 */
enum CardType { BOMB, REINFORCEMENT, BLOCKADE, AIRLIFT, DIPLOMACY };

/**
 * Represents a single Warzone card.
 * Owns a CardType pointer as required by the assignment (data members of user-defined
 * class type must be pointer type).
 */
class Cards {
private:
    CardType* _cardType;  // heap-allocated card type

public:
    Cards(CardType* type);
    Cards(const Cards& copy);
    ~Cards();

    Cards& operator=(const Cards& assign);
    friend std::ostream& operator<<(std::ostream& out, const Cards& card);

    CardType* getCardType() const;

    /**
     * Plays this card: creates the appropriate Order, adds it to the player's
     * OrdersList, removes the card from the hand, and returns it to the deck.
     */
    void play(Hand* hand, Deck* deck, Player* player);
};

/**
 * The Warzone card deck.
 * Owns all Cards that are currently not in any player's hand.
 */
class Deck {
private:
    std::vector<Cards*> _cards;  // cards currently in the deck

public:
    Deck(int numCards);
    Deck(const Deck& copy);
    ~Deck();

    Deck& operator=(const Deck& assign);
    friend std::ostream& operator<<(std::ostream& out, const Deck& deck);

    // Draws one random card from the deck into the given hand.
    void draw(Hand* hand);

    // Returns a card to the deck (called by Cards::play).
    void addCard(Cards* card);

    // Removes a card at the given index (used internally by draw).
    void removeCard(int index);

    int getNumCards() const;
    std::vector<Cards*>* getCards() { return &_cards; }
};

/**
 * A player's hand of Warzone cards.
 */
class Hand {
private:
    std::vector<Cards*> _cardsInHand;  // cards currently held

public:
    Hand();
    Hand(const Hand& copy);
    ~Hand();

    Hand& operator=(const Hand& assign);
    friend std::ostream& operator<<(std::ostream& out, const Hand& hand);

    void addCard(Cards* card);
    void removeCard(Cards* card);
    int getNumCards() const;
    Cards* getCard(int index) const;
    std::vector<Cards*>* getCards() { return &_cardsInHand; }
};

#endif // CARDS_H