#include "Cards.h"
#include <iostream>
#include <random>
#include <vector>

using namespace std;

void cardsTest(){
    Deck* deck = new Deck(42);
    Hand* hand1 = new Hand();
    

    deck->draw(hand1);
    deck->draw(hand1);
    deck->draw(hand1);

    // cout << *deck << endl;
    // cout << *hand1 << endl;

    for(int i = 0; i<hand1->getNumCards();i++){
        hand1->getCards()->at(i)->play(hand1,deck);
    }
    

    // cout << *deck << endl;
    // cout << *hand1 << endl;

    delete deck;
    deck = nullptr;

    delete hand1;
    hand1= nullptr;


}