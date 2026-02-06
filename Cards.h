#include <iostream>
#include <vector>

using namespace std;

class Cards;
class Deck;
class Hand;

enum CardType {BOMB,REINFORCEMENT, BLOCKADE, AIRLIFT, DIPLOMACY};

class Cards{
    private:
        CardType* cardType;
    public:
    Cards(CardType* type);
    Cards(const Cards& copy);

    ~Cards();

    Cards& operator=(const Cards& assign);
    friend ostream &operator<<(ostream &out, const Cards &card);

    void getCardType();
    void play(Hand* hand, Deck* deck);

};

class Deck{
    private:
        vector<Cards*> cards;
    public:
        Deck(int numCards);
        Deck(const Deck& copy);
        ~Deck();


        Deck& operator=(const Deck& assign);
        friend ostream &operator<<(ostream &out, const Deck &card);


        void draw(Hand* hand);
        void addCard(Cards* card);
        void removeCard(int index);
        int getNumCards() const;
        vector<Cards*>* getCards() { return &cards; }


};

class Hand{
    private:
        vector<Cards*> cardsInHand;
    public:
        Hand();
        Hand(const Hand& copy);
        ~Hand();

        Hand& operator=(const Hand& assign);
        friend ostream &operator<<(ostream &out, const Hand &hand);

        void addCard(Cards* card);
        void removeCard(Cards* card);
        int getNumCards() const;
        vector<Cards*>* getCards() { return &cardsInHand; }

};
