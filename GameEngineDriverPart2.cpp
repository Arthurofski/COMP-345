/**
 * GameEngineDriver.cpp
 *
 * Demonstrates all 6 required game-engine criteria:
 *
 *  (1) Reinforcement phase - correct army counts across 3 cases:
 *        (a) few territories  -> minimum 3 armies
 *        (b) many territories -> territories / 3 armies
 *        (c) continent bonus  -> base + continent bonus
 *
 *  (2) Deploy-only while reinforcement pool > 0 - no other order type issued
 *      until the pool is fully spent.
 *
 *  (3) Advance orders - a player issues advance orders both to ATTACK an
 *      enemy territory (toAttack list) and to DEFEND by reinforcing a weaker
 *      friendly territory (toDefend list).
 *
 *  (4) Card playing - a player plays a card from their hand to issue an order.
 *
 *  (5) Player elimination - a player with 0 territories is removed from the game.
 *
 *  (6) Game ends - the game loop exits and declares a winner when one player
 *      controls all territories.
 */

#include "GameEngine.h"
#include "Orders.h"
#include "Cards.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>

using namespace std;

// Write a small but valid Warzone .map file to disk.
//
//  [NorthAmerica: 5]         [Asia: 7]
//   Canada -- USA   <--->   China -- Russia -- Japan
static void writeDemoMap(const string& path) {
    ofstream f(path);
    f << "[continents]\n"
      << "NorthAmerica 5 red\n"
      << "Asia 7 blue\n"
      << "\n"
      << "[countries]\n"
      << "1 Canada 1 0 0\n"
      << "2 USA    1 0 0\n"
      << "3 China  2 0 0\n"
      << "4 Russia 2 0 0\n"
      << "5 Japan  2 0 0\n"
      << "\n"
      << "[borders]\n"
      << "1 2\n"        // Canada -- USA
      << "2 1 3\n"      // USA -- Canada, China (cross-continent bridge)
      << "3 2 4\n"      // China -- USA, Russia
      << "4 3 5\n"      // Russia -- China, Japan
      << "5 4\n";       // Japan -- Russia
}

// Run startupPhase() with a scripted sequence of commands by
// redirecting cin to a stringstream.
static void runStartup(GameEngine& engine, const string& commands) {
    istringstream input(commands);
    streambuf* orig = cin.rdbuf(input.rdbuf());
    engine.startupPhase();
    cin.rdbuf(orig);
}


// Helper: build a small connected map in memory.
//
//  ContinentA (bonus 3): T1 -- T2 -- T3
//  ContinentB (bonus 5): T4 -- T5
//  Cross-continent edge : T3 -- T4
static Map* buildMap() {
    Map* m = new Map();

    Continent* contA = new Continent("ContinentA", 3);
    Continent* contB = new Continent("ContinentB", 5);
    m->continents->push_back(contA);
    m->continents->push_back(contB);

    Territory* t1 = new Territory("T1");
    Territory* t2 = new Territory("T2");
    Territory* t3 = new Territory("T3");
    Territory* t4 = new Territory("T4");
    Territory* t5 = new Territory("T5");

    t1->setContinent(contA); t2->setContinent(contA); t3->setContinent(contA);
    t4->setContinent(contB); t5->setContinent(contB);

    m->territories->push_back(t1); m->territories->push_back(t2);
    m->territories->push_back(t3); m->territories->push_back(t4);
    m->territories->push_back(t5);

    t1->addNeighbour(t2); t2->addNeighbour(t1);
    t2->addNeighbour(t3); t3->addNeighbour(t2);
    t3->addNeighbour(t4); t4->addNeighbour(t3);
    t4->addNeighbour(t5); t5->addNeighbour(t4);

    return m;
}

// Helper: assign a territory to a player and set its army count
static void giveTerritory(Player* p, Territory* t, int armies = 3) {
    t->setOwner(p);
    p->addTerritory(t);
    *t->armies = armies;
}

// Helper: print a player's current order list
static void printOrders(const string& label, Player* p) {
    cout << "    " << label << " orders for " << *p->getName() << ":\n";
    OrdersList* ol = p->getOrders();
    if (ol->size() == 0) { cout << "      (none)\n"; return; }
    for (int i = 0; i < ol->size(); ++i)
        cout << "      [" << i << "] " << *ol->get(i) << "\n";
}

// Helper: count Deploy vs non-Deploy orders in a player's list
static void countOrderTypes(Player* p, int& deploys, int& others) {
    deploys = others = 0;
    OrdersList* ol = p->getOrders();
    for (int i = 0; i < ol->size(); ++i) {
        if (dynamic_cast<const Deploy*>(ol->get(i))) ++deploys;
        else                                          ++others;
    }
}

// Condition 1: Reinforcement - correct army counts in three cases
static void demo_reinforcement() {
    cout << "\n============================================================\n"
         << " DEMO 1 [Condition 1]: Reinforcement army calculations\n"
         << "============================================================\n";

    // --- Case (a): few territories -> minimum 3 ---
    cout << "\n  Case A: 1 territory owned -> minimum of 3 armies applied\n";
    {
        Map* m = buildMap();
        GameEngine ge;
        ge.loadMap(m);
        ge.addPlayer("Alice");
        ge.addPlayer("Dummy"); // second player needed

        Player* alice = ge.getPlayers()->at(0);
        Player* dummy = ge.getPlayers()->at(1);

        giveTerritory(alice, (*m->territories)[0]); // T1 only
        giveTerritory(dummy, (*m->territories)[1]); // give dummy something

        ge.setState(AssignReinforcement);
        ge.reinforcementPhase();

        int pool = alice->getReinforcementPool();
        cout << "    Alice owns 1 territory, 1/3 = 0, floor applied -> pool = "
             << pool << " (expected 3)\n";
        assert(pool == 3);
        cout << "    PASS: minimum 3 enforced\n";
    }

    // --- Case (b): many territories -> territories / 3 > minimum ---
    cout << "\n  Case B: 12 territories owned \n";
    {
        // Build a 13-territory chain map
        Map* m = new Map();
        Continent* c = new Continent("LargeContinent", 0);
        m->continents->push_back(c);
        vector<Territory*> ts;
        for (int i = 0; i < 13; i++) {
            Territory* t = new Territory("TR" + to_string(i));
            t->setContinent(c);
            m->territories->push_back(t);
            ts.push_back(t);
        }
        for (int i = 0; i < 12; i++) {
            ts[i]->addNeighbour(ts[i+1]);
            ts[i+1]->addNeighbour(ts[i]);
        }

        GameEngine ge;
        ge.loadMap(m);
        ge.addPlayer("Bob");
        ge.addPlayer("Eve");

        Player* bob = ge.getPlayers()->at(0);
        Player* eve = ge.getPlayers()->at(1);

        for (int i = 0; i < 12; i++) giveTerritory(bob, ts[i]); // Bob: 12
        giveTerritory(eve, ts[12]);                              // Eve: 1

        ge.setState(AssignReinforcement);
        ge.reinforcementPhase();

        int pool = bob->getReinforcementPool();
        int expected = max(3, 12/3); // = 4
        cout << "    Bob owns 12 territories, pool = " << pool
             << " (expected " << expected << ")\n";
        assert(pool == expected);
        cout << "   PASS: territories/3 calculation correct\n";
    }

    // --- Case (c): continent bonus added on top of base ---
    cout << "\n  Case C: Continent ownership adds bonus armies\n";
    {
        Map* m = buildMap();
        GameEngine ge;
        ge.loadMap(m);
        ge.addPlayer("Carol");
        ge.addPlayer("Dave");

        Player* carol = ge.getPlayers()->at(0);
        Player* dave  = ge.getPlayers()->at(1);

        // Carol owns all of ContinentA (T1,T2,T3  bonus=3)
        // Dave  owns all of ContinentB (T4,T5     bonus=5)
        giveTerritory(carol, (*m->territories)[0]); // T1
        giveTerritory(carol, (*m->territories)[1]); // T2
        giveTerritory(carol, (*m->territories)[2]); // T3
        giveTerritory(dave,  (*m->territories)[3]); // T4
        giveTerritory(dave,  (*m->territories)[4]); // T5

        ge.setState(AssignReinforcement);
        ge.reinforcementPhase();

        // Carol: max(3, 3/3)=3 base + 3 bonus = 6
        // Dave:  max(3, 2/3)=3 base + 5 bonus = 8
        int carolPool = carol->getReinforcementPool();
        int davePool  = dave->getReinforcementPool();
        cout << "    Carol (3 territories + ContinentA bonus 3): pool = " << carolPool
             << " (expected 6 = 3 base + 3 bonus)\n";
        cout << "    Dave  (2 territories + ContinentB bonus 5): pool = " << davePool
             << " (expected 8 = 3 base + 5 bonus)\n";
        assert(carolPool == 6);
        assert(davePool  == 8);
        cout << "   PASS: continent bonuses applied correctly\n";
    }
}

// Condition 2: Only Deploy orders issued while pool > 0
static void demo_deployOnly() {
    cout << "\n============================================================\n"
         << " DEMO 2 [Condition 2]: Only Deploy orders while pool > 0\n"
         << "============================================================\n";

    Map* m = buildMap();
    GameEngine ge;
    ge.loadMap(m);
    ge.addPlayer("Alice");
    ge.addPlayer("Bob");

    Player* alice = ge.getPlayers()->at(0);
    Player* bob   = ge.getPlayers()->at(1);

    giveTerritory(alice, (*m->territories)[0], 5); // T1
    giveTerritory(alice, (*m->territories)[1], 5); // T2
    giveTerritory(bob,   (*m->territories)[2], 5); // T3 (enemy, adjacent to T2)
    giveTerritory(bob,   (*m->territories)[3], 5); // T4
    giveTerritory(bob,   (*m->territories)[4], 5); // T5

    alice->setReinforcementPool(3);

    cout << "\n  Alice has reinforcement pool = 3.\n"
         << "  Calling issueOrder() 3 times to drain the pool:\n";
    for (int i = 0; i < 3; i++) {
        int poolBefore = alice->getReinforcementPool();
        alice->issueOrder(nullptr);
        int poolAfter  = alice->getReinforcementPool();
        int d, o;
        countOrderTypes(alice, d, o);
        cout << "    Call " << (i+1) << ": pool " << poolBefore << "->" << poolAfter
             << "  |  orders: " << d << " Deploy, " << o << " other\n";
        assert(o == 0); // no non-Deploy orders while pool > 0
    }
    cout << "    PASS: 0 non-Deploy orders issued while pool > 0\n";

    assert(alice->getReinforcementPool() == 0);

    cout << "\n  Pool is now 0. One more issueOrder() -> Advance order expected:\n";
    int dBefore, oBefore;
    countOrderTypes(alice, dBefore, oBefore);
    alice->issueOrder(nullptr);
    int dAfter, oAfter;
    countOrderTypes(alice, dAfter, oAfter);
    cout << "    Non-Deploy orders before: " << oBefore
         << "  after: " << oAfter << "\n";
    assert(oAfter > oBefore);
    cout << "    PASS: non-Deploy order issued once pool is empty\n";

    printOrders("Final", alice);
    while (alice->getOrders()->size() > 0) alice->getOrders()->remove(0);
}

// Condition 3: Advance orders - attack via toAttack(), defend via toDefend()
static void demo_advance() {
    cout << "\n============================================================\n"
         << " DEMO 3 [Condition 3]: Advance to attack and to defend\n"
         << "============================================================\n";

    // --- Part A: ATTACK ---
    cout << "\n  Part A - ATTACK: Alice has enemy neighbour in toAttack()\n";
    {
        Map* m = buildMap();
        GameEngine ge;
        ge.loadMap(m);
        ge.addPlayer("Alice");
        ge.addPlayer("Bob");

        Player* alice = ge.getPlayers()->at(0);
        Player* bob   = ge.getPlayers()->at(1);

        giveTerritory(alice, (*m->territories)[2], 10); // T3, heavily armed
        giveTerritory(alice, (*m->territories)[1], 2);  // T2
        giveTerritory(bob,   (*m->territories)[3], 5);  // T4, adjacent to T3
        giveTerritory(bob,   (*m->territories)[0], 5);  // T1
        giveTerritory(bob,   (*m->territories)[4], 5);  // T5

        alice->setReinforcementPool(0);

        vector<Territory*>* atk = alice->toAttack();
        cout << "    Alice's toAttack() list: ";
        for (Territory* t : *atk) cout << t->getName() << " ";
        cout << "\n";
        assert(!atk->empty());
        cout << "    PASS: toAttack() is not empty\n";
        delete atk;

        alice->issueOrder(nullptr);
        int d, o;
        countOrderTypes(alice, d, o);
        printOrders("Attack", alice);
        assert(o >= 1);
        cout << "    PASS: Advance (attack) order issued\n";
        while (alice->getOrders()->size() > 0) alice->getOrders()->remove(0);
    }

    // --- Part B: DEFEND ---
    cout << "\n  Part B - DEFEND: Alice owns all territories, toAttack() is empty\n"
         << "    -> issues Advance to reinforce weaker own territory\n";
    {
        // Build a 3-territory map, all owned by Alice
        Map* m2 = new Map();
        Continent* c2 = new Continent("Solo", 0);
        m2->continents->push_back(c2);
        Territory* d1 = new Territory("Home1");
        Territory* d2 = new Territory("Home2");
        Territory* d3 = new Territory("Home3");
        d1->setContinent(c2); d2->setContinent(c2); d3->setContinent(c2);
        d1->addNeighbour(d2); d2->addNeighbour(d1);
        d2->addNeighbour(d3); d3->addNeighbour(d2);
        m2->territories->push_back(d1);
        m2->territories->push_back(d2);
        m2->territories->push_back(d3);

        GameEngine ge2;
        ge2.loadMap(m2);
        ge2.addPlayer("Alice2");
        ge2.addPlayer("Dummy2");

        Player* alice2 = ge2.getPlayers()->at(0);
        Player* dummy2 = ge2.getPlayers()->at(1);

        // Alice2 owns all 3; no enemy -> toAttack() empty
        giveTerritory(alice2, d1, 1);  // weak
        giveTerritory(alice2, d2, 10); // strong -> will move to d1
        giveTerritory(alice2, d3, 1);  // weak
        // dummy2 gets nothing (will be eliminated but irrelevant here)
        alice2->setReinforcementPool(0);

        vector<Territory*>* atk = alice2->toAttack();
        cout << "    Alice2's toAttack() list: ";
        if (atk->empty()) cout << "(empty)";
        else for (Territory* t : *atk) cout << t->getName() << " ";
        cout << "\n";
        assert(atk->empty());
        cout << "    PASS: toAttack() is empty\n";
        delete atk;

        alice2->issueOrder(nullptr);
        int d, o;
        countOrderTypes(alice2, d, o);
        printOrders("Defend", alice2);
        assert(o >= 1);
        cout << "    PASS: Defensive Advance order issued via toDefend()\n";
    }
}

// Condition 4: Card playing - player plays a card to issue an order

static void demo_cardPlaying() {
    cout << "\n============================================================\n"
         << " DEMO 4 [Condition 4]: Playing a card issues an order\n"
         << "============================================================\n";

    Map* m = buildMap();
    GameEngine ge;
    ge.loadMap(m);
    ge.addPlayer("Alice");
    ge.addPlayer("Bob");

    Player* alice = ge.getPlayers()->at(0);
    Player* bob   = ge.getPlayers()->at(1);

    giveTerritory(alice, (*m->territories)[0], 5); // T1
    giveTerritory(alice, (*m->territories)[1], 5); // T2
    giveTerritory(bob,   (*m->territories)[2], 5); // T3
    giveTerritory(bob,   (*m->territories)[3], 5); // T4
    giveTerritory(bob,   (*m->territories)[4], 5); // T5

    Deck* deck = new Deck(10);
    deck->draw(alice->getHand());

    cout << "\n  Alice's hand: " << *alice->getHand() << "\n";
    int handBefore  = alice->getHand()->getNumCards();
    int deckBefore  = deck->getNumCards();
    int ordersBefore = alice->getOrders()->size();

    assert(handBefore >= 1);
    cout << "  Hand size = " << handBefore
         << ", Deck size = " << deckBefore
         << ", Orders = " << ordersBefore << "\n";

    alice->setReinforcementPool(0); // skip straight to card phase
    cout << "  Calling issueOrder() with pool=0 (card-play phase)...\n";
    alice->issueOrder(deck);

    int handAfter   = alice->getHand()->getNumCards();
    int deckAfter   = deck->getNumCards();
    int ordersAfter = alice->getOrders()->size();

    cout << "  Hand:   " << handBefore << " -> " << handAfter  << " (card removed from hand)\n";
    cout << "  Deck:   " << deckBefore << " -> " << deckAfter  << " (card returned to deck)\n";
    cout << "  Orders: " << ordersBefore << " -> " << ordersAfter << " (new order added)\n";

    assert(handAfter   == handBefore  - 1);
    assert(deckAfter   == deckBefore  + 1);
    assert(ordersAfter >  ordersBefore);
    cout << "  PASS: card removed from hand, returned to deck, order added\n";

    printOrders("Card-generated", alice);
    delete deck;
}

// ===========================================================================
// Condition 5: Player with 0 territories is eliminated from the game
// ===========================================================================
static void demo_elimination() {
    cout << "\n============================================================\n"
         << " DEMO 5 [Condition 5]: Player with 0 territories is eliminated\n"
         << "============================================================\n";

    Map* m = buildMap();
    GameEngine ge;
    ge.loadMap(m);
    ge.addPlayer("Alice");
    ge.addPlayer("Bob");
    ge.addPlayer("Carol"); // Carol will get no territories

    Player* alice = ge.getPlayers()->at(0);
    Player* bob   = ge.getPlayers()->at(1);
    // Carol: index 2, gets nothing

    giveTerritory(alice, (*m->territories)[0]);
    giveTerritory(alice, (*m->territories)[1]);
    giveTerritory(alice, (*m->territories)[2]);
    giveTerritory(bob,   (*m->territories)[3]);
    giveTerritory(bob,   (*m->territories)[4]);

    cout << "\n  Players before phase: ";
    for (Player* p : *ge.getPlayers()) cout << *p->getName() << " ";
    cout << "\n  (Carol owns 0 territories)\n";
    assert(ge.getPlayerCount() == 3);

    ge.setState(ExecuteOrders);
    ge.executeOrdersPhase(); // internally calls removeEliminatedPlayers()

    cout << "  Players after  phase: ";
    for (Player* p : *ge.getPlayers()) cout << *p->getName() << " ";
    cout << "\n";

    assert(ge.getPlayerCount() == 2);
    for (Player* p : *ge.getPlayers())
        assert(*p->getName() != "Carol");
    cout << "  PASS: Carol eliminated, 2 players remain\n";
}

// Condition 6: Game ends when one player controls all territories
static void demo_gameEnd() {
    cout << "\n============================================================\n"
         << " DEMO 6 [Condition 6]: Game ends when one player owns all territories\n"
         << "============================================================\n";

    // Bob starts with 0 territories -> immediately eliminated -> Alice wins
    Map* m = buildMap();
    GameEngine ge;
    ge.loadMap(m);
    ge.addPlayer("Alice");
    ge.addPlayer("Bob");

    Player* alice = ge.getPlayers()->at(0);
    // Bob gets nothing

    for (Territory* t : *m->territories)
        giveTerritory(alice, t, 5);

    cout << "\n  Alice owns all " << m->territories->size()
         << " territories. Bob owns none.\n"
         << "  Running mainGameLoop()...\n\n";

    ge.mainGameLoop();

    cout << "\n  Final game state: " << ge.getCurrentState() << "\n";
    cout << "  Players remaining: " << ge.getPlayerCount() << "\n";
    cout << "  Winner: " << *ge.getPlayers()->at(0)->getName() << "\n";

    assert(ge.getCurrentState() == "Win");
    assert(ge.getPlayerCount() == 1);
    assert(*ge.getPlayers()->at(0)->getName() == "Alice");
    cout << "  PASS: state == Win, Alice is the sole remaining player\n";
}
//Demo 7: Main Game loop
static void demo_main_game(){
    cout << "\n============================================================\n"
         << " DEMO 7: Main Game loop\n"
         << "============================================================\n";

    // Build the map programmatically so the demo is self-contained
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

        GameEngine ge;
        runStartup(ge, "loadmap demo.map\n"
            "validatemap\n"
            "addplayer Alice\n"
            "addplayer Bob\n"
            "addplayer Carol\n"
            "gamestart\n");
        assert(ge.getCurrentState() == "AssignReinforcement");
        cout << "[ASSERT] State == AssignReinforcement after gamestart - PASS\n\n";
        ge.mainGameLoop();

}

// main

int main() {
    srand(42);

    

    demo_main_game();   //(7) Main game demo demonstrating main game loop works + game Startup phase works.
    return 0;
}