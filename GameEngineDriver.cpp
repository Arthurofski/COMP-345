/**
 * The driver first writes a small valid .map file to disk so MapLoader
 * can read it without needing an external file.  It then feeds scripted
 * command sequences through stdin to show each startup step, followed
 * by a full mainGameLoop() run to demonstrate the three game phases.
 */

#include "GameEngine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

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

//  Main
int main() {
    srand(42);  // fixed seed for reproducible card draws

    cout << "\n============= GameEngineDriver Demo ==============\n";

    writeDemoMap("demo.map");

    // DEMO 1: Functional startup sequence + valid commands
    // loadmap -> validatemap -> addplayer x3 -> gamestart
    cout << "\n-----DEMO 1 - Full startup sequence (happy path):-----\n";
    {
        GameEngine ge;
        runStartup(ge,
            "loadmap demo.map\n"
            "validatemap\n"
            "addplayer Alice\n"
            "addplayer Bob\n"
            "addplayer Carol\n"
            "gamestart\n");

        assert(ge.getCurrentState() == "AssignReinforcement");
        cout << "[ASSERT] State == AssignReinforcement after gamestart — PASS\n\n";
    }

    // DEMO 2: Error, gamestart with only 1 player, then recover
    cout << "\n-----DEMO 2 - Error: gamestart with < 2 players, then recover:-----\n";
    {
        GameEngine ge;
        runStartup(ge,
            "loadmap demo.map\n"
            "validatemap\n"
            "addplayer Solo\n"
            "gamestart\n"       // rejected – only 1 player
            "addplayer Partner\n"
            "gamestart\n");     // now valid

        assert(ge.getCurrentState() == "AssignReinforcement");
        cout << "[ASSERT] Recovered and reached play phase — PASS\n\n";
    }

    // DEMO 3: Error, commands issued out of order
    cout << "\n-----DEMO 3 - Error: out-of-order commands:-----\n";
    {
        GameEngine ge;
        runStartup(ge,
            "validatemap\n"      // wrong state, rejected
            "addplayer Early\n"  // wrong state, rejected
            "loadmap demo.map\n"
            "addplayer Early\n"  // wrong state (MapLoaded, not MapValidated)
            "validatemap\n"
            "gamestart\n"        // rejected, no players
            "addplayer X\n"
            "addplayer Y\n"
            "gamestart\n");

        assert(ge.getCurrentState() == "AssignReinforcement");
        cout << "[ASSERT] Invalid commands handled, recovered — PASS\n\n";
    }

    // DEMO 4: Maximum 6 players; 7th is rejected
    cout << "\n-----DEMO 4 - Maximum 6 players (7th rejected):-----\n";
    {
        GameEngine ge;
        runStartup(ge,
            "loadmap demo.map\n"
            "validatemap\n"
            "addplayer P1\n"
            "addplayer P2\n"
            "addplayer P3\n"
            "addplayer P4\n"
            "addplayer P5\n"
            "addplayer P6\n"
            "addplayer P7\n"    // rejected, max 6 players
            "gamestart\n");

        assert(ge.getCurrentState() == "AssignReinforcement");
        assert(ge.getPlayerCount() == 6);
        cout << "[ASSERT] 7th player rejected, 6 players in game — PASS\n\n";
    }

    // DEMO 5: loadmap can be re-issued before validatemap
    cout << "\n-----DEMO 5 - Reload map before validating:-----\n";
    {
        writeDemoMap("demo2.map");
        GameEngine ge;
        runStartup(ge,
            "loadmap demo.map\n"
            "loadmap demo2.map\n"   // legal: replace before validatemap
            "validatemap\n"
            "addplayer A\n"
            "addplayer B\n"
            "gamestart\n");

        assert(ge.getCurrentState() == "AssignReinforcement");
        cout << "[ASSERT] Map reload then gamestart succeeded — PASS\n\n";
    }

    // DEMO 6: Full mainGameLoop() after programmatic setup
    // (uses the existing loadMap/addPlayer/assignCountries path to show
    //  the three game phases running end-to-end)
    cout << "-----DEMO 6 - Full mainGameLoop() (reinforcement, issueOrders, executeOrders):-----\n";
    {
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
        ge.loadMap(m);
        ge.addPlayer("Alice");
        ge.addPlayer("Bob");
        ge.assignCountries();

        ge.mainGameLoop();
    }

    cout << "Driver demo complete.\n";

    return 0;
}
