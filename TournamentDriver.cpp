/**
 * TournamentDriver.cpp
 *
 * Demonstrates the Tournament Mode functionality where multiple games are played
 * across different maps with different player strategies in a round-robin fashion.
 */

#include "GameEngine.h"
#include "CommandProcessing.h"
#include <iostream>

using namespace std;

int main() {
    cout << "\n============================================================\n"
         << "         WARZONE TOURNAMENT MODE DEMONSTRATION\n"
         << "============================================================\n";

    // Create a GameEngine instance
    GameEngine* ge = new GameEngine();

    // Define tournament parameters
    vector<string> maps = {"map1.map", "map2.map"};
    vector<string> strategies = {"Aggressive", "Defensive", "Benevolent"};
    int numGames = 2;           // Play 2 games per map
    int maxTurns = 20;          // Max 20 turns per game

    cout << "\nTournament Configuration:\n"
         << "  Maps: ";
    for (const auto& m : maps) cout << m << " ";
    cout << "\n  Strategies: ";
    for (const auto& s : strategies) cout << s << " ";
    cout << "\n  Games per map: " << numGames
         << "\n  Max turns per game: " << maxTurns << "\n";

    // Run the tournament
    cout << "\nStarting tournament...\n";
    bool success = ge->runTournament(maps, strategies, numGames, maxTurns);

    if (success) {
        cout << "\n============================================================\n"
             << "              TOURNAMENT COMPLETED SUCCESSFULLY\n"
             << "============================================================\n";
    } else {
        cout << "\nTournament failed to complete. Check map files exist.\n";
    }

    delete ge;
    return 0;
}
