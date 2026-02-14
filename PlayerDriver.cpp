#include "Player.h"
#include <iostream>

int main() {
    std::cout << "----- Player Driver Demo -----" << std::endl;
    
    // Create Players
    std::cout << "\nCreating players:" << std::endl;
    Player* player1 = new Player("Bob");
    Player* player2 = new Player("John");
    
    std::cout << "Created: " << *(player1->getName()) << std::endl;
    std::cout << "Created: " << *(player2->getName()) << std::endl;
    
    // Create territories
    Territory* t1 = new Territory("Canada");
    Territory* t2 = new Territory("USA");
    Territory* t3 = new Territory("China");
    Territory* t4 = new Territory("Russia");
    
    // Assign territories to players
    player1->addTerritory(t1);
    player1->addTerritory(t2);
    player2->addTerritory(t3);
    player2->addTerritory(t4);
    
    // Player owns a Hand of cards
    std::cout << "\nHand Ownership:" << std::endl;
    std::cout << "Player1 hand: " << *(player1->getHand()) << std::endl;
    std::cout << "Player2 hand: " << *(player2->getHand()) << std::endl;
    
    // toDefend() method
    std::cout << "\ntoDefend() method:" << std::endl;
    
    std::vector<Territory*>* defend1 = player1->toDefend();
    std::cout << "Player1 territories to defend: ";
    for (Territory* t : *defend1) {
        std::cout << t->getName() << " ";
    }
    std::cout << std::endl;
    delete defend1;
    defend1 = nullptr;
    
    std::vector<Territory*>* defend2 = player2->toDefend();
    std::cout << "Player2 territories to defend: ";
    for (Territory* t : *defend2) {
        std::cout << t->getName() << " ";
    }
    std::cout << std::endl;
    delete defend2;
    defend2 = nullptr;
    
    // toAttack() method
    std::cout << "\ntoAttack() method:" << std::endl;
    
    std::vector<Territory*>* attack1 = player1->toAttack();
    std::cout << "Player1 territories to attack: ";
    for (Territory* t : *attack1) {
        std::cout << t->getName() << " ";
    }
    std::cout << std::endl;\
    
    // creates new Territory objects (for this demo only, will be changed later), so delete them before deleting the vector
    for (Territory* t : *attack1) {
        delete t;
    }
    delete attack1;
    attack1 = nullptr;
    
    std::vector<Territory*>* attack2 = player2->toAttack();
    std::cout << "Player2 territories to attack: ";
    for (Territory* t : *attack2) {
        std::cout << t->getName() << " ";
    }
    std::cout << std::endl;
    
    // creates new Territory objects (for this demo only, will be changed later), so delete them before deleting the vector
    for (Territory* t : *attack2) {
        delete t;
    }
    delete attack2;
    attack2 = nullptr;
    
    // issueOrder() method
    std::cout << "\nissueOrder() method:" << std::endl;
    
    std::cout << "Player1 issuing orders:" << std::endl;
    player1->issueOrder();
    
    std::cout << "Player2 issuing orders:" << std::endl;
    player2->issueOrder();
    
    std::cout << "\nPlayer1 orders: " << *(player1->getOrders()) << std::endl;
    std::cout << "Player2 orders: " << *(player2->getOrders()) << std::endl;

    // Copy constructor
    std::cout << "\nTest copy constructor:" << std::endl;

    Player player1Copy = *player1;
    std::cout << "Created copy of " << *(player1->getName()) << std::endl;
    std::cout << "Copy name: " << *(player1Copy.getName()) << std::endl;
    
    // Verify copy has same territories
    std::vector<Territory*>* copyTerritories = player1Copy.getTerritories();
    std::cout << "Copy has " << copyTerritories->size() << " territories: ";
    for (Territory* t : *copyTerritories) {
        std::cout << t->getName() << " ";
    }
    std::cout << std::endl;
    
    // Assignment Operator
    std::cout << "\nTest assignment operator:" << std::endl;
    
    // Create a temporary player and assign player2 to it
    Player player3("TempPlayer");
    std::cout << "Before assignment: " << *(player3.getName()) << std::endl;
    
    player3 = *player2;
    std::cout << "After assignment: " << *(player3.getName()) << std::endl;
    
    // Verify assigned player has same territories
    std::vector<Territory*>* assignedTerritories = player3.getTerritories();
    std::cout << "Assigned player has " << assignedTerritories->size() << " territories: ";
    for (Territory* t : *assignedTerritories) {
        std::cout << t->getName() << " ";
    }
    std::cout << std::endl;

    // player1Copy and player3 are stack allocated, so will be deleted automatically after
    
    // Stream Insertion Operator
    std::cout << "\nStream insertion operator:" << std::endl;
    std::cout << "\n" << *player1 << std::endl;
    std::cout << "\n" << *player2 << std::endl;
    
    // Cleanup - delete heap allocations and set pointers to nullptr
    
    delete player1;
    player1 = nullptr;
    
    delete player2;
    player2 = nullptr;
    
    delete t1;
    t1 = nullptr;

    delete t2;
    t2 = nullptr;

    delete t3;
    t3 = nullptr;

    delete t4;
    t4 = nullptr;
    
    std::cout << "\n----- Demo finished -----" << std::endl;
    
    return 0;
}
