#include "Map.h"
#include <iostream>
using namespace std;

void testMap(string filename) {
    Map* m = MapLoader::load(filename);

    cout << "Testing " << filename << endl;

    if (m->validate())
        cout << "Result: VALID\n";
    else
        cout << "Result: INVALID\n";

    cout << "-----------------\n";

    delete m;
}


static Map* buildValidMap() {
    Map* m = new Map();
 
    Continent* na   = new Continent("NorthAmerica", 5);
    Continent* asia = new Continent("Asia", 7);
    m->continents->push_back(na);
    m->continents->push_back(asia);
 
    Territory* canada = new Territory("Canada");
    Territory* usa    = new Territory("USA");
    Territory* china  = new Territory("China");
    Territory* russia = new Territory("Russia");
 
    canada->setContinent(na);
    usa->setContinent(na);
    china->setContinent(asia);
    russia->setContinent(asia);
 
    m->territories->push_back(canada);
    m->territories->push_back(usa);
    m->territories->push_back(china);
    m->territories->push_back(russia);
 
    canada->addNeighbour(usa);    usa->addNeighbour(canada);
    china->addNeighbour(russia);  russia->addNeighbour(china);
    usa->addNeighbour(china);     china->addNeighbour(usa);
 
    return m;
}
 
/**
 * Builds a map that is NOT fully connected (Japan is isolated).
 */
static Map* buildDisconnectedMap() {
    Map* m = new Map();
 
    Continent* na   = new Continent("NorthAmerica", 5);
    Continent* asia = new Continent("Asia", 7);
    m->continents->push_back(na);
    m->continents->push_back(asia);
 
    Territory* canada = new Territory("Canada");
    Territory* usa    = new Territory("USA");
    Territory* japan  = new Territory("Japan");   // isolated — no neighbours
 
    canada->setContinent(na);
    usa->setContinent(na);
    japan->setContinent(asia);
 
    m->territories->push_back(canada);
    m->territories->push_back(usa);
    m->territories->push_back(japan);
 
    canada->addNeighbour(usa);
    usa->addNeighbour(canada);
    // japan has no neighbours — breaks global connectivity
 
    return m;
}
 
/**
 * Builds a map where a continent is not a connected subgraph
 * (Alaska and Hawaii are both in NorthAmerica but have no path between them
 * that stays within the continent).
 */
static Map* buildDisconnectedContinentMap() {
    Map* m = new Map();
 
    Continent* na   = new Continent("NorthAmerica", 5);
    Continent* asia = new Continent("Asia", 7);
    m->continents->push_back(na);
    m->continents->push_back(asia);
 
    Territory* alaska = new Territory("Alaska");
    Territory* hawaii = new Territory("Hawaii");  // also NA but only connected via Japan
    Territory* japan  = new Territory("Japan");
 
    alaska->setContinent(na);
    hawaii->setContinent(na);  // same continent as Alaska — but only linked through Japan
    japan->setContinent(asia);
 
    m->territories->push_back(alaska);
    m->territories->push_back(hawaii);
    m->territories->push_back(japan);
 
    // Global connectivity: alaska -- japan -- hawaii
    alaska->addNeighbour(japan); japan->addNeighbour(alaska);
    japan->addNeighbour(hawaii); hawaii->addNeighbour(japan);
    // NorthAmerica subgraph: alaska and hawaii have no direct path within NA
 
    return m;
}
 
/**
 * Builds a map where a territory has no continent assigned.
 */
static Map* buildMissingContinentMap() {
    Map* m = new Map();
 
    Continent* na = new Continent("NorthAmerica", 5);
    m->continents->push_back(na);
 
    Territory* canada = new Territory("Canada");
    Territory* usa    = new Territory("USA");   // continent intentionally not assigned
 
    canada->setContinent(na);
    // usa has no continent — verifyTerritories() should catch this
 
    m->territories->push_back(canada);
    m->territories->push_back(usa);
 
    canada->addNeighbour(usa);
    usa->addNeighbour(canada);
 
    return m;
}
 
int main() {
    cout << "========== Map Driver Demo ==========\n\n";
 
    // ---- 1. Valid map ----
    cout << "--- Test 1: Valid Map ---\n";
    Map* validMap = buildValidMap();
    cout << *validMap;
    cout << "verifyMap()          : " << (validMap->verifyMap()         ? "PASS" : "FAIL") << "\n";
    cout << "verifyContinents()   : " << (validMap->verifyContinents()  ? "PASS" : "FAIL") << "\n";
    cout << "verifyTerritories()  : " << (validMap->verifyTerritories() ? "PASS" : "FAIL") << "\n";
    cout << "validate() (overall) : " << (validMap->validate()          ? "VALID" : "INVALID") << "\n\n";
 
    // ---- 2. Assign territories to players and demonstrate owner link ----
    cout << "--- Test 2: Player ownership via Map territories ---\n";
    Player* alice = new Player("Alice");
    Player* bob   = new Player("Bob");
 
    Territory* canada = (*validMap->territories)[0];  // Canada
    Territory* usa    = (*validMap->territories)[1];  // USA
    Territory* china  = (*validMap->territories)[2];  // China
    Territory* russia = (*validMap->territories)[3];  // Russia
 
    alice->addTerritory(canada);
    alice->addTerritory(usa);
    bob->addTerritory(china);
    bob->addTerritory(russia);
 
    cout << *alice << "\n";
    cout << *bob   << "\n";
 
    cout << "Alice's toAttack() (real map neighbours):\n  ";
    vector<Territory*>* attack = alice->toAttack();
    for (Territory* t : *attack) cout << t->getName() << " ";
    cout << "\n\n";
    delete attack;
 
    // ---- 3. Copy constructor ----
    cout << "--- Test 3: Map copy constructor ---\n";
    Map* mapCopy = new Map(*validMap);
    cout << "Copy validate(): " << (mapCopy->validate() ? "VALID" : "INVALID") << "\n\n";
 
    // ---- 4. Disconnected map ----
    cout << "--- Test 4: Disconnected map (Japan isolated) ---\n";
    Map* disconnected = buildDisconnectedMap();
    cout << "verifyMap()          : " << (disconnected->verifyMap()         ? "PASS" : "FAIL (expected)") << "\n";
    cout << "validate() (overall) : " << (disconnected->validate()          ? "VALID" : "INVALID (expected)") << "\n\n";
 
    // ---- 5. Disconnected continent ----
    cout << "--- Test 5: Disconnected continent (NorthAmerica not a subgraph) ---\n";
    Map* badContinent = buildDisconnectedContinentMap();
    cout << "verifyMap()          : " << (badContinent->verifyMap()         ? "PASS" : "FAIL") << "\n";
    cout << "verifyContinents()   : " << (badContinent->verifyContinents()  ? "PASS" : "FAIL (expected)") << "\n";
    cout << "validate() (overall) : " << (badContinent->validate()          ? "VALID" : "INVALID (expected)") << "\n\n";
 
    // ---- 6. Territory without continent ----
    cout << "--- Test 6: Territory missing continent assignment ---\n";
    Map* missingContinent = buildMissingContinentMap();
    cout << "verifyTerritories()  : " << (missingContinent->verifyTerritories() ? "PASS" : "FAIL (expected)") << "\n";
    cout << "validate() (overall) : " << (missingContinent->validate()           ? "VALID" : "INVALID (expected)") << "\n\n";
 
    // ---- 7. Load from file (optional — pass a path as argument) ----
    cout << "--- Test 7: Load from .map file ---\n";
    cout << "(Pass a .map file path as argv[1] to test MapLoader)\n";
    cout << "Skipping: no file provided.\n\n";
 
    // Cleanup
    // Clear player territory vectors before deleting players — territories are owned by validMap
    alice->getTerritories()->clear();
    bob->getTerritories()->clear();
    delete alice;
    delete bob;
    delete validMap;
    delete mapCopy;
    delete disconnected;
    delete badContinent;
    delete missingContinent;
 
    cout << "========== Map Driver Done ==========\n";
    return 0;
}
