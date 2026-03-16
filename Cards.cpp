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

    cout << "  [Card played: " << *this << "]\n";

    Order* order = nullptr;

    switch (*_cardType) {
        case BOMB: {
            // Bomb an adjacent enemy territory
            vector<Territory*>* attack = player->toAttack();
            Territory* tgt = (!attack->empty()) ? attack->front() : nullptr;
            order = new Bomb(player, tgt);
            delete attack;
            break;
        }
        case REINFORCEMENT: {
            // Extra deploy to the player's weakest territory
            vector<Territory*>* defend = player->toDefend();
            if (!defend->empty()) {
                Territory* weakest = defend->front();
                for (Territory* t : *defend)
                    if (*t->armies < *weakest->armies) weakest = t;
                // Add 3 directly to player's pool so Deploy::validate() passes,
                // then queue the order (execute() will deduct them back).
                player->addReinforcementPool(3);
                order = new Deploy(3, player, weakest);
            } else {
                order = new Deploy(); // empty default — validate() will fail safely
            }
            delete defend;
            break;
        }
        case BLOCKADE: {
            // Blockade the player's most-threatened territory
            vector<Territory*>* defend = player->toDefend();
            Territory* tgt = (!defend->empty()) ? defend->front() : nullptr;
            order = new Blockade(player, tgt);
            delete defend;
            break;
        }
        case AIRLIFT: {
            // Airlift from strongest to weakest owned territory
            vector<Territory*>* defend = player->toDefend();
            Territory* src = nullptr;
            Territory* tgt = nullptr;
            int moving = 5;
            if (defend->size() >= 2) {
                Territory* strongest = defend->front();
                Territory* weakest   = defend->front();
                for (Territory* t : *defend) {
                    if (*t->armies > *strongest->armies) strongest = t;
                    if (*t->armies < *weakest->armies)   weakest   = t;
                }
                src    = strongest;
                tgt    = weakest;
                moving = *strongest->armies / 2;
            } else if (defend->size() == 1) {
                src = defend->front();
                tgt = defend->front();
            }
            order = new Airlift(moving, player, src, tgt);
            delete defend;
            break;
        }
        case DIPLOMACY: {
            // Negotiate with a random neighbour's owner
            vector<Territory*>* attack = player->toAttack();
            Player* tgt = nullptr;
            if (!attack->empty() && attack->front()->owner)
                tgt = attack->front()->owner;
            order = new Negotiate(player, tgt);
            delete attack;
            break;
        }
    }
    if(order!=nullptr)
        player->getOrders()->add(order);

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