// PlayerStrategyDriver.cpp
//
// Demonstrates the 3 requirements from the assignment:
//   (1) Different players assigned different strategies → different behaviour
//   (2) Strategy can be changed dynamically at runtime
//   (3) Human player uses cin; AI players are fully automated

#include "Player.h"
#include "PlayerStrategies.h"
#include "Map.h"
#include "Orders.h"
#include "Cards.h"
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

//map builder
static Map* makeMap() {
    Map* m = new Map();

    Continent* europe = new Continent("NorthAmerica", 5);
    m->continents->push_back(europe);

    Territory* france = new Territory("France");
    Territory* england    = new Territory("England");
    Territory* russia = new Territory("Russia");
    Territory* germany = new Territory("Germany");
    Territory* italy = new Territory("Italy");
    Territory* spain = new Territory("Spain");
    Territory* portugal = new Territory("Portugal");

    france->setContinent(europe);
    england->setContinent(europe);
    russia->setContinent(europe);
    germany->setContinent(europe);
    italy->setContinent(europe);
    spain->setContinent(europe);
    portugal->setContinent(europe);

    france->addNeighbour(england);
    france->addNeighbour(germany);
    france->addNeighbour(spain);

    england->addNeighbour(france);

    germany->addNeighbour(france);
    germany->addNeighbour(italy);
    germany->addNeighbour(russia);
    
    spain->addNeighbour(france);
    spain->addNeighbour(portugal);

    portugal->addNeighbour(spain);

    italy->addNeighbour(germany);

    russia->addNeighbour(germany);


    m->territories->push_back(france);   // index 0
    m->territories->push_back(england);      // index 1
    m->territories->push_back(germany);   // index 2
    m->territories->push_back(italy);   // index 3
    m->territories->push_back(spain);   // index 4
    m->territories->push_back(russia);   // index 5
    m->territories->push_back(portugal);   // index 6

    return m;
}


static void printMap(Map* m) {
    cout << "\n  Map state:\n";
    for (Territory* t : *m->territories) {
        string owner = t->owner ? *t->owner->getName() : "Nobody";
        cout << "    " << t->getName()
             << "  owner: " << owner
             << "  armies: " << *t->armies << "\n";
    }
    cout << "\n";
}

int main() {

    Map* gameMap = makeMap();
    
    Deck* deck = new Deck(10);

    cout << "DEMO 1: Different strategies\n";

    // Create one player per strategy
    Player* alice   = new Player("Alice");    // Human
    Player* hovig     = new Player("Hovig");      // Aggressive
    Player* stephan   = new Player("Stephan");    // Benevolent
    Player* sahag = new Player("Sahag");  // Neutral
    Player* arthur     = new Player("Arthur");      // Cheater

    alice->setStrategy(new HumanPlayerStrategy());
    hovig->setStrategy(new AggressivePlayerStrategy());
    stephan->setStrategy(new BenevolentPlayerStrategy());
    sahag->setStrategy(new NeutralPlayerStrategy());
    arthur->setStrategy(new CheaterPlayerStrategy());

    Territory* france = gameMap->territories->at(0); 
    Territory* england = gameMap->territories->at(1);
    Territory* germany = gameMap->territories->at(2); 
    Territory* italy = gameMap->territories->at(3); 
    Territory* spain = gameMap->territories->at(4); 
    Territory* russia = gameMap->territories->at(5); 
    Territory* portugal = gameMap->territories->at(6);

    stephan->addTerritory(france);
    stephan->addTerritory(italy);  
    france->setOwner(stephan);
    italy->setOwner(stephan);

    arthur->addTerritory(england);  
    england->setOwner(arthur);

    sahag->addTerritory(germany); 
    germany->setOwner(sahag);

    alice->addTerritory(spain);   
    alice->addTerritory(portugal);
    spain->setOwner(alice);
    portugal->setOwner(alice);

    hovig->addTerritory(russia);  
    russia->setOwner(hovig);

    *france->armies = 3;
    *england->armies = 5;   
    *germany->armies = 5;
    *italy->armies = 2;
    *spain->armies = 5;
    *russia->armies = 5;
    *portugal->armies = 5;

    alice->setReinforcementPool(3);
    stephan->setReinforcementPool(3);
    sahag->setReinforcementPool(3);
    hovig->setReinforcementPool(3);
    arthur->setReinforcementPool(3);

    // Show toDefend / toAttack results per strategy
    cout << " results of toDefend()\n";
    {
        auto* d = alice->toDefend();
        cout << *alice->getName() << " (Human)     defends: ";
        for (Territory* t : *d) cout << t->getName() << " "; cout << "\n";
        delete d;
    }
    {
        auto* d = hovig->toDefend();
        cout << *hovig->getName() << " (Aggressive) defends: ";
        for (Territory* t : *d) cout << t->getName() << " "; cout << "\n";
        delete d;
    }
    {
        auto* d = stephan->toDefend();
        cout << *stephan->getName() << " (Benevolent) defends (weakest first): ";
        for (Territory* t : *d) cout << t->getName() << " "; cout << "\n";
        delete d;
    }
    {
        auto* d = sahag->toDefend();
        cout << *sahag->getName() << " (Neutral)    defends: ";
        for (Territory* t : *d) cout << t->getName() << " "; cout << "\n";
        delete d;
    }
    {
        auto* d = arthur->toDefend();
        cout << *arthur->getName() << " (Cheater)    defends: ";
        for (Territory* t : *d) cout << t->getName() << " "; cout << "\n";
        delete d;
    }

    cout << "\n results of toAttack() \n";
    {
        auto* a = alice->toAttack();
        cout << *alice->getName() << " (Human) attacks: ";
        for (Territory* t : *a) cout << t->getName() << " "; cout << "\n";
        delete a;
    }
    {
        auto* a = stephan->toAttack();
        cout <<  *stephan->getName() << " (Benevolent) attacks: ";
        if(a->empty()){
            cout << "(none - doesn't attack)\n";
        }
        else{
            for (Territory* t : *a) cout << t->getName() << " "; cout << "\n";
        }
        delete a;
    }
    {
        auto* a = sahag->toAttack();
        cout << *sahag->getName() << "  (Neutral)    attacks: ";
        if(a->empty()){
            cout << "(none - doesn't attack)\n";
        }
        else{
            for (Territory* t : *a) cout << t->getName() << " "; cout << "\n";
        }
        delete a;
    }
    {
        auto* a = hovig->toAttack();
        cout << *hovig->getName() << "  (Neutral)    attacks: ";
        if(a->empty()){
            cout << "(none - doesn't attack)\n";
        }
        else{
            for (Territory* t : *a) cout << t->getName() << " "; cout << "\n";
        }
        delete a;
    }
    {
        auto* a = arthur->toAttack();
        cout << *arthur->getName() << "  (Neutral)    attacks: ";
        if(a->empty()){
            cout << "(none - doesn't attack)\n";
        }
        else{
            for (Territory* t : *a) cout << t->getName() << " "; cout << "\n";
        }
        delete a;
    }

    cout << "\n--- issueOrder() results ---\n";
    
    cout << "\nSahag (Neutral) issues orders:\n";
    sahag->issueOrder(deck);
    
    cout << "\nHovig (Aggressive) issues orders:\n";
    hovig->issueOrder(deck);

    cout << "\nStephan (Benevolent) issues orders:\n";
    stephan->issueOrder(deck);
    //strategy dynamically changed at runtime;
    cout << "\nSahag (aggressive) issues orders:\n";
    sahag->issueOrder(deck);

    cout << "\nArthur (Cheater) issues orders:\n";
    arthur->issueOrder(deck);
    


    printMap(gameMap);



    

    // // =========================================================================
    // // DEMO (3): Human player uses cin, AI players are automated
    // // =========================================================================
    cout << "\n============================================\n";
    cout << "DEMO 3: Human player uses cin (interactive)\n";
    cout << "============================================\n\n";

    // Give Alice fresh armies to deploy
    alice->setReinforcementPool(4);

    deck->draw(alice->getHand());  // Give Alice a card to optionally play
    deck->draw(alice->getHand());
    deck->draw(alice->getHand());

    cout << "Alice (Human) will now issue orders interactively.\n";
    cout << "(Deploy phase first, then you can issue Advance orders.\n";
    cout << " Type 'd' at the Advance prompt when done.)\n\n";



    alice->issueOrder(deck);

    // Cleanup
    delete alice;
    delete sahag;
    delete arthur;
    delete hovig;
    delete stephan;
    delete deck;
    delete gameMap;

    return 0;
}