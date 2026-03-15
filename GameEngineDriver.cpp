#include "GameEngine.h"
#include "Map.h"
#include "Player.h"
#include "Orders.h"
#include <iostream>

using namespace std;

/**
 * Builds a small but complete map:
 *
 *  [NorthAmerica: 5]        [Asia: 7]
 *   Canada -- USA   |   China -- Russia -- Japan
 *        \________________/
 *          (cross-continent edge)
 */
static Map* buildDemoMap() {
    Map* m = new Map();

    Continent* na   = new Continent("NorthAmerica", 5);
    Continent* asia = new Continent("Asia", 7);
    m->continents->push_back(na);
    m->continents->push_back(asia);

    Territory* canada = new Territory("Canada");
    Territory* usa    = new Territory("USA");
    Territory* china  = new Territory("China");
    Territory* russia = new Territory("Russia");
    Territory* japan  = new Territory("Japan");

    canada->setContinent(na);
    usa->setContinent(na);
    china->setContinent(asia);
    russia->setContinent(asia);
    japan->setContinent(asia);

    m->territories->push_back(canada);
    m->territories->push_back(usa);
    m->territories->push_back(china);
    m->territories->push_back(russia);
    m->territories->push_back(japan);

    canada->addNeighbour(usa);    usa->addNeighbour(canada);
    china->addNeighbour(russia);  russia->addNeighbour(china);
    russia->addNeighbour(japan);  japan->addNeighbour(russia);
    usa->addNeighbour(china);     china->addNeighbour(usa);

    return m;
}

int main() {
    srand(42);  // fixed seed for reproducible demo output

    cout << "========================================\n";
    cout << "       GAME ENGINE DRIVER DEMO          \n";
    cout << "========================================\n\n";

    // ---- Demo 1: State machine ----
    cout << "--- Demo 1: State machine (stateValidation) ---\n";
    {
        GameEngine ge;
        cout << "State: " << ge.getCurrentState() << "\n";
        ge.stateValidation("loadmap");
        cout << "After 'loadmap': " << ge.getCurrentState() << "\n";
        ge.stateValidation("loadmap");     // stay in MapLoaded
        ge.stateValidation("validatemap");
        cout << "After 'validatemap': " << ge.getCurrentState() << "\n";
        ge.stateValidation("addplayer");
        ge.stateValidation("assigncountries");
        cout << "After 'assigncountries': " << ge.getCurrentState() << "\n";
        ge.stateValidation("badcommand");  // invalid — should print error
        ge.stateValidation("issueorder");
        ge.stateValidation("endissueorders");
        ge.stateValidation("execorder");
        ge.stateValidation("win");
        cout << "After 'win': " << ge.getCurrentState() << "\n";
        ge.stateValidation("end");
        cout << "After 'end': " << ge.getCurrentState() << "\n\n";
    }

    // ---- Demo 2: Reinforcement phase — different territory counts ----
    cout << "--- Demo 2: Reinforcement phase (territory count cases) ---\n";
    {
        GameEngine ge;
        ge.loadMap(buildDemoMap());
        ge.addPlayer("Alice");
        ge.addPlayer("Bob");
        ge.assignCountries();

        cout << "\nBefore reinforcement:\n";
        for (int i = 0; i < ge.getPlayerCount(); i++) {
            // Print via GameEngine which owns players internally;
            // we demonstrate through the phase output below
        }
        ge.reinforcementPhase();
        // Show: Alice owns 3 territories → max(3, 3/3)=3 armies
        //       Bob owns 2 territories   → max(3, 2/3)=3 armies (minimum)
        cout << "\n";
    }

    // ---- Demo 3–6: Full main game loop ----
    cout << "--- Demo 3-6: Full main game loop ---\n";
    cout << "(3) Deploy-only while pool > 0\n";
    cout << "(4) Advance orders after pool empty\n";
    cout << "(5) Cards played\n";
    cout << "(6) Eliminated players removed; game ends with one winner\n\n";
    {
        GameEngine ge;
        ge.loadMap(buildDemoMap());
        ge.addPlayer("Alice");
        ge.addPlayer("Bob");
        ge.assignCountries();

        ge.mainGameLoop();
    }

    cout << "\n========== Driver Done ==========\n";
    return 0;
}