#include "Cards.h"
#include "Orders.h"   // real order subclasses: Bomb, Blockade, Airlift, Negotiate, Deploy
#include <iostream>
#include <vector>
#include <cstdlib>    // rand()

using namespace std;

// ==================== Cards ====================

Cards::Cards(CardType* type) {
    _cardType = type;
}

Cards::Cards(const Cards& copy) {
    _cardType = new CardType(*copy._cardType);  // deep copy the enum value
}

Cards::~Cards() {
    delete _cardType;
    _cardType = nullptr;
}

Cards& Cards::operator=(const Cards& assign) {
    if (this != &assign) {
        delete _cardType;
        _cardType = new CardType(*assign._cardType);
    }
    return *this;
}

ostream& operator<<(ostream& out, const Cards& card) {
    switch (*card._cardType) {
        case BOMB:          out << "Bomb";          break;
        case REINFORCEMENT: out << "Reinforcement"; break;
        case BLOCKADE:      out << "Blockade";      break;
        case AIRLIFT:       out << "Airlift";       break;
        case DIPLOMACY:     out << "Diplomacy";     break;
    }
    return out;
}

CardType* Cards::getCardType() const {
    return _cardType;
}

/**
 * Plays this card:
 *   1. Creates the corresponding Order using the real Order subclasses.
 *   2. Adds it to the player's real OrdersList.
 *   3. Removes this card from the hand.
 *   4. Returns this card to the deck.
 */
void Cards::play(Hand* hand, Deck* deck, Player* player) {
    if (!hand || !deck || !player) {
        cerr << "play() called with null pointer — cannot play card." << endl;
        return;
    }

    cout << "Playing card: " << *this << endl;

    Order* order = nullptr;

    switch (*_cardType) {
        case BOMB:
            // Bombs an arbitrary enemy territory (placeholder target)
            order = new Bomb("EnemyTerritory");
            break;

        case REINFORCEMENT:
            // Deploy uses player's first territory as target, or "NoTerritory" if none owned
            {
                string target = player->getTerritories()->empty()
                                ? "NoTerritory"
                                : player->getTerritories()->front()->getName();
                order = new Deploy(3, target);
            }
            break;

        case BLOCKADE:
            // Blockades one of the player's own territories
            {
                string target = player->getTerritories()->empty()
                                ? "NoTerritory"
                                : player->getTerritories()->front()->getName();
                order = new Blockade(target);
            }
            break;

        case AIRLIFT:
            // Airlifts armies between two of the player's territories (placeholder)
            order = new Airlift(5, "SourceTerritory", "TargetTerritory");
            break;

        case DIPLOMACY:
            // Negotiates with an arbitrary opponent (placeholder)
            order = new Negotiate("OpponentPlayer");
            break;
    }

    // Add the created order to the player's real OrdersList
    player->getOrders()->add(order);
    cout << "  -> Added order: " << *order << " to " << *player->getName() << "'s order list." << endl;

    // Remove from hand and return to deck
    hand->removeCard(this);
    deck->addCard(this);
}

// ==================== Deck ====================

Deck::Deck(int numCards) {
    for (int i = 0; i < numCards; i++) {
        CardType* type = new CardType(static_cast<CardType>(i % 5));
        _cards.push_back(new Cards(type));
    }
}

Deck::Deck(const Deck& copy) {
    for (Cards* card : copy._cards) {
        _cards.push_back(new Cards(*card));
    }
}

Deck::~Deck() {
    for (Cards* card : _cards) {
        delete card;
    }
    _cards.clear();
    cout << "Deck destructor called" << endl;
}

Deck& Deck::operator=(const Deck& assign) {
    if (this != &assign) {
        for (Cards* card : _cards) {
            delete card;
        }
        _cards.clear();
        for (Cards* card : assign._cards) {
            _cards.push_back(new Cards(*card));
        }
    }
    return *this;
}

ostream& operator<<(ostream& out, const Deck& deck) {
    out << "Deck contains " << deck._cards.size() << " cards:" << endl;
    for (Cards* card : deck._cards) {
        out << "  " << *card << endl;
    }
    return out;
}

void Deck::draw(Hand* hand) {
    if (_cards.empty()) {
        cout << "Deck is empty, cannot draw." << endl;
        return;
    }
    if (!hand) {
        cerr << "hand pointer is null in draw()." << endl;
        return;
    }
    int index = rand() % _cards.size();
    Cards* drawnCard = _cards.at(index);
    hand->addCard(drawnCard);
    removeCard(index);
}

void Deck::addCard(Cards* card) {
    _cards.push_back(card);
}

void Deck::removeCard(int index) {
    if (index < 0 || index >= static_cast<int>(_cards.size())) {
        cerr << "Index out of bounds in Deck::removeCard." << endl;
        return;
    }
    _cards.erase(_cards.begin() + index);
}

int Deck::getNumCards() const {
    return static_cast<int>(_cards.size());
}

// ==================== Hand ====================

Hand::Hand() {}

Hand::Hand(const Hand& copy) {
    for (Cards* card : copy._cardsInHand) {
        _cardsInHand.push_back(new Cards(*card));
    }
}

Hand::~Hand() {
    for (Cards* card : _cardsInHand) {
        delete card;
    }
    _cardsInHand.clear();
    cout << "Hand destructor called" << endl;
}

Hand& Hand::operator=(const Hand& assign) {
    if (this != &assign) {
        for (Cards* card : _cardsInHand) {
            delete card;
        }
        _cardsInHand.clear();
        for (Cards* card : assign._cardsInHand) {
            _cardsInHand.push_back(new Cards(*card));
        }
    }
    return *this;
}

ostream& operator<<(ostream& out, const Hand& hand) {
    out << "Hand contains " << hand._cardsInHand.size() << " cards:";
    for (Cards* card : hand._cardsInHand) {
        out << " [" << *card << "]";
    }
    return out;
}

void Hand::addCard(Cards* card) {
    _cardsInHand.push_back(card);
}

void Hand::removeCard(Cards* card) {
    for (int i = 0; i < static_cast<int>(_cardsInHand.size()); i++) {
        if (_cardsInHand[i] == card) {
            _cardsInHand.erase(_cardsInHand.begin() + i);
            return;
        }
    }
    cout << "Card not found in hand during removeCard()." << endl;
}

int Hand::getNumCards() const {
    return static_cast<int>(_cardsInHand.size());
}