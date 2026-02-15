#include "Cards.h"
#include <iostream>
#include <vector>
#include <random>

using namespace std;

Cards::Cards(CardType* type){
    cardType = type;
}
Cards::Cards(const Cards& copy){
    cardType = copy.cardType;
}
Cards::~Cards(){
    delete cardType;
    cardType = nullptr;
}
Cards& Cards::operator=(const Cards& assign){
    cout << "Assignment operator called." << endl;
    if(this != &assign){
        delete cardType;
        cardType = assign.cardType;
    }
    return *this;
}
ostream &operator<<(ostream &out, const Cards &card)
{
    // TODO: insert return statement here
    switch(*card.cardType){
        case BOMB:
            out << "Bomb";
            break;
        case REINFORCEMENT:
            out << "Reinforcement";
            break;
        case BLOCKADE:
            out << "Blockade";
            break;
        case AIRLIFT:
            out << "Airlift";
            break;
        case DIPLOMACY:
            out << "Diplomacy";
            break;
    }
    return out;
}
void Cards::getCardType(){
    cout << *cardType << endl;
}

void Cards::play(Hand* hand, Deck* deck, Player* player){
    if(!hand){
        cerr<<"Hand pointer is null, cannot play card. " << endl;
        return;
    }
    cout << "Playing card: " << *cardType << endl;
    Order *order = nullptr;
    switch(*cardType){
        //Assuming these order classes weill be implemented
        // case BOMB:
        //     order = new BombOrder();
        //     break;
        // case BLOCKADE:
        //     order = new BlockadeOrder();
        //     break;
        // case AIRLIFT:
        //     order = new AirliftOrder();
        //     break;
        default:
            cout << "Card type not implemented yet" << endl;
            return;
    }
    player->getOrders()->add(order);
    hand->removeCard(this);
    deck->addCard(this);

}

//=================Deck Class Implementation=================

Deck::Deck(int numCards){
    for(int i = 0; i< numCards;i++){
        CardType* type = new CardType(static_cast<CardType>(i%5));
        cards.push_back(new Cards(type));
    }

}

Deck::Deck(const Deck& copy){
    for(Cards* card : copy.cards){
        cards.push_back(new Cards(*card));
    }
}

Deck::~Deck(){
    for(Cards* card : cards){
        delete card;
        card = nullptr;
    }
    cout << "Deck destructor called" << endl;
}

Deck& Deck::operator=(const Deck& assign){
    cout << "Assignment operator called." << endl;
    if(this != &assign){
        for(Cards* card : cards){
            delete card;
        }
        cards.clear();
        for(Cards* card : assign.cards){
            cards.push_back(new Cards(*card));
        }
    }
    return *this;
}

ostream &operator<<(ostream &out, const Deck &deck){
    out << "Deck contains " << deck.cards.size() << " cards: " << endl;
    for(Cards* card : deck.cards){
        out << *card << endl;
    }
    return out;
}

void Deck::draw(Hand* hand){
    if(cards.empty()){
        cout << "Dek is empty, cannot draw cards" << endl;
        return;
    }
    if(!hand){
        cerr << "hand pointer is null " << endl;
        return;
    }
    int index = rand() % cards.size();
    Cards* drawnCard = cards.at(index);
    hand->addCard(drawnCard);
    removeCard(index);
}

void Deck::addCard(Cards* card){
    cards.push_back(card);
}

void Deck::removeCard(int index){
    if(index < 0 || index >= cards.size()){
        cerr << "Index out of bounds in removeCard" << endl;
        return;
    }
    cards.erase(cards.begin() + index);
}

int Deck::getNumCards() const{
    return cards.size();
}

//=================Hand Class Implementation=================
Hand::Hand(){
    cardsInHand = vector<Cards*>();
}
Hand::Hand(const Hand& copy){
    for(int i = 0; i < copy.getNumCards(); i++){
        cardsInHand.push_back(new Cards(*copy.cardsInHand[i]));
    }
}

Hand::~Hand(){
    for(Cards* card : cardsInHand){
        delete card;
        card = nullptr;
    }
    cout << "Hand destructor called" << endl;

}

Hand& Hand::operator=(const Hand& assign){
    cout << "Assignment operator called." << endl;
    if(this != &assign){
        for(Cards* card : cardsInHand){
            delete card;
        }
        cardsInHand.clear();
        for(Cards* card : assign.cardsInHand){
            cardsInHand.push_back(new Cards(*card));
        }
    }
    return *this;
}

ostream &operator<<(ostream &out, const Hand &hand){
    out << "Hand contains " << hand.getNumCards() << " cards: " << endl;
    for(Cards* card : hand.cardsInHand){
        out << *card << endl;
    }
    return out;
}

void Hand::addCard(Cards* card){
    cardsInHand.push_back(card);
}

void Hand::removeCard(Cards* card){
    for(int i = 0;i<getNumCards();i++){
        if(cardsInHand[i] == card){
            cardsInHand.erase(cardsInHand.begin() + i);
            return;
        }
    }
    cout << "Card not found in hand" << endl;
    return;
}

int Hand::getNumCards() const{
    return cardsInHand.size();
}  
