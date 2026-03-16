/**
 * CommandProcessingDriver.cpp
 *
 * Usage:
 *   ./driver -console
 *   ./driver -file <filename>
 *
 * Tests the CommandProcessing module:
 *   1. Validation - valid commands accepted, invalid ones rejected per state
 *   2. The chosen input mode - console or file
 *
 * Compile alongside: CommandProcessing.cpp, GameEngine.cpp, Map.cpp,
 *                    Player.cpp, Orders.cpp, Cards.cpp, LoggingObserver.cpp
 */

#include "CommandProcessing.h"
#include "GameEngine.h"
#include "LoggingObserver.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

using std::cout;
using std::string;

// -----------------------------------------------------------------------------
// Test 1 - validate(): spot-check valid and invalid commands across key states
// -----------------------------------------------------------------------------
static void testValidation() {
    cout << "\n=== TEST 1: validate() - valid & invalid commands per state ===\n";

    CommandProcessor cp;

    struct Case { state s; string cmd; bool expected; };
    std::vector<Case> cases = {
        // Valid transitions
        { Start,        "loadmap",     true  },
        { MapLoaded,    "validatemap", true  },
        { MapValidated, "addplayer",   true  },
        { PlayersAdded, "gamestart",   true  },
        { Win,          "replay",      true  },
        { Win,          "quit",        true  },
        // Invalid: right command, wrong state
        { Start,        "validatemap", false },
        { MapLoaded,    "addplayer",   false },
        { MapValidated, "gamestart",   false },
        // Invalid: garbage input
        { Start,        "garbage",     false },
        { PlayersAdded, "garbage",     false },
    };

    for (auto& c : cases) {
        GameEngine ge(c.s);
        bool result = cp.validate(c.cmd, &ge);
        bool pass   = (result == c.expected);
        cout << "  [" << ge.getCurrentState() << "] "
             << c.cmd << std::string(14 - c.cmd.size(), ' ')
             << "-> " << (result ? "ACCEPTED" : "REJECTED")
             << "\n";
        assert(pass);
    }

    cout << "All validation assertions passed.\n";
}

// -----------------------------------------------------------------------------
// Helper: advance engine state based on an accepted command
// -----------------------------------------------------------------------------
static void advanceState(GameEngine& ge, const string& cmd) {
    if      (cmd == "loadmap"     && ge.getCurrentState() == "Start")        ge.setState(MapLoaded);
    else if (cmd == "validatemap" && ge.getCurrentState() == "MapLoaded")    ge.setState(MapValidated);
    else if (cmd == "addplayer"   && ge.getCurrentState() == "MapValidated") ge.setState(PlayersAdded);
    else if (cmd == "gamestart"   && ge.getCurrentState() == "PlayersAdded") ge.setState(Win);
}

// -----------------------------------------------------------------------------
// Test 2a - Console: read commands from stdin, validate, change state
// -----------------------------------------------------------------------------
static void testConsoleInput() {
    cout << "\n=== TEST 2: Console CommandProcessor ===\n";
    cout << "Valid commands per state:\n";
    cout << "  Start        -> loadmap\n";
    cout << "  MapLoaded    -> loadmap, validatemap\n";
    cout << "  MapValidated -> addplayer\n";
    cout << "  PlayersAdded -> addplayer, gamestart\n";
    cout << "  Win          -> replay, quit\n";
    cout << "Type 'done' to exit.\n\n";

    GameEngine ge(Start);
    CommandProcessor cp;
    LogObserver logger;
    cp.attach(&logger);
    ge.attach(&logger);

    while (true) {
        cout << "[" << ge.getCurrentState() << "] > ";
        string line;
        if (!std::getline(std::cin, line) || line == "done") break;
        if (line.empty()) continue;

        std::istringstream iss(line);
        string cmd; iss >> cmd;

        if (cp.validate(cmd, &ge)) {
            Command* c = new Command(cmd);
            c->attach(&logger);
            cp.saveCommand(c);
            c->saveEffect("Accepted in state " + ge.getCurrentState());
            advanceState(ge, cmd);
            cout << "  -> Accepted. New state: [" << ge.getCurrentState() << "]\n";
        } else {
            cout << "  -> Rejected (invalid for state [" << ge.getCurrentState() << "])\n";
        }
    }

    cout << "\nFinal state: [" << ge.getCurrentState() << "]\n";
    cout << "Commands accepted this session:\n" << cp;
}

// -----------------------------------------------------------------------------
// Test 2b - File: read commands from file, invalid ones skipped, state changes
// -----------------------------------------------------------------------------
static void testFileInput(const string& filename) {
    cout << "\n=== TEST 2: FileCommandProcessorAdapter - reading from '" << filename << "' ===\n";

    GameEngine ge(Start);
    FileCommandProcessorAdapter fcp(filename);
    LogObserver logger;
    fcp.attach(&logger);
    ge.attach(&logger);

    string line;
    while (!(line = fcp.getCommand(&ge)).empty()) {
        std::istringstream iss(line);
        string cmd; iss >> cmd;

        cout << "  State [" << ge.getCurrentState() << "]: got '" << cmd << "'";
        advanceState(ge, cmd);
        cout << " -> New state: [" << ge.getCurrentState() << "]\n";
    }

    cout << "\nFinal state: [" << ge.getCurrentState() << "]\n";
}

// -----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    cout << "===== CommandProcessing Driver =====\n";

    if (argc < 2) {
        cout << "Usage:\n";
        cout << "  " << argv[0] << " -console\n";
        cout << "  " << argv[0] << " -file <filename>\n";
        return 1;
    }

    string mode = argv[1];

    testValidation();

    if (mode == "-console") {
        testConsoleInput();
    } else if (mode == "-file") {
        if (argc < 3) {
            cout << "Error: -file requires a filename argument.\n";
            cout << "Usage: " << argv[0] << " -file <filename>\n";
            return 1;
        }
        testFileInput(argv[2]);
    } else {
        cout << "Error: unknown mode '" << mode << "'.\n";
        cout << "Usage:\n";
        cout << "  " << argv[0] << " -console\n";
        cout << "  " << argv[0] << " -file <filename>\n";
        return 1;
    }

    cout << "\n===== Driver complete =====\n";
    return 0;
}