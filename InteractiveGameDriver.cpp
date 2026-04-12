/**
 * InteractiveGameDriver.cpp
 *
 * Runs the Warzone game engine in interactive mode.
 * Supports all commands including tournament mode.
 *
 * Tournament command example:
 *   tournament -M solar.map -P Aggressive Defensive -G 2 -D 20
 */

#include "GameEngine.h"
#include <iostream>

using namespace std;

int main() {
    cout << "\n============================================================\n"
         << "         WARZONE GAME ENGINE - INTERACTIVE MODE\n"
         << "============================================================\n"
         << "\nAvailable commands:\n"
         << "  loadmap <filename>          - Load a map file\n"
         << "  validatemap                 - Validate the loaded map\n"
         << "  addplayer <name>            - Add a player (max 6)\n"
         << "  gamestart                   - Start the game\n"
         << "  tournament -M <maps> -P <strategies> -G <games> -D <turns>\n"
         << "                              - Run tournament mode\n"
         << "  replay                      - Start a new game (after winning)\n"
         << "  quit                        - Exit the game\n"
         << "\nTournament constraints:\n"
         << "  Maps: 1-5, Strategies: 2-4, Games: 1-5, Turns: 10-50\n"
         << "============================================================\n\n";

    GameEngine ge;
    ge.startupPhase();

    return 0;
}
