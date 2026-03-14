#include "GameEngine.h"
#include <iostream>
#include <string>

// Function to convert state enum to string for display purposes
std::string stateToString( state s ) {
    switch(s) {
        case Start: return "Start";
        case MapLoaded: return "MapLoaded";
        case MapValidated: return "MapValidated";
        case PlayersAdded: return "PlayersAdded";
        case AssignReinforcement: return "AssignReinforcement";
        case IssueOrders: return "IssueOrders";
        case ExecuteOrders: return "ExecuteOrders";
        case Win: return "Win";
        case End: return "End";
        default: return "Unknown State";
    }
}

// Default constructor for GameEngine, state is set to Start
GameEngine::GameEngine() {
    currentState = new state(Start);
}

// Copy constructor for GameEngine when given another GameEngine object
GameEngine::GameEngine(const GameEngine& other) {
    currentState = new state(*(other.currentState));
}

// Constructor for GameEngine when given a state
GameEngine::GameEngine(state currState) {
    currentState = new state(currState);
}

// Destructor for GameEngine
GameEngine::~GameEngine() {
    delete currentState;
}

// Assignment operator for GameEngine
GameEngine& GameEngine::operator=(const GameEngine& other) {
    if (this == &other) {
        return *this;
    }
    delete currentState;
    currentState = new state(*(other.currentState));
    return *this;
}

// Main validation function to check if a command is valid and return true if it is, otherwise return false and print an error message
bool GameEngine::stateValidation(const std::string& command) {
    // Switch statement to determine valid commands based on the current state
    switch(*currentState) {
        // Start, can only move to MapLoaded
        case Start:
            if (command == "loadmap") {
                setState(MapLoaded);
                return true;
            } else {
                std::cout << "Invalid command in Start state. Only allows loadmap." << std::endl;
                return false;
            }
        // MapLoaded, can move to MapValidated or stay in MapLoaded
        case MapLoaded:
            if (command == "loadmap") {
                return true;
            } else if (command == "validatemap") {
                setState(MapValidated);
                return true;
            } else {
                std::cout << "Invalid command in MapLoaded state. Only allows loadmap or validatemap." << std::endl;
                return false;
            }
        // MapValidated, can move to PlayersAdded or stay in MapValidated
        case MapValidated:
            if (command == "addplayer") {
                setState(PlayersAdded);
                return true;
            } else {
                std::cout << "Invalid command in MapValidated state. Only allows addplayer." << std::endl;
                return false;
            }
        // PlayersAdded, can move to AssignReinforcement or stay in PlayersAdded
        case PlayersAdded:
            if (command == "addplayer") {
                return true;
            } else if (command == "assigncountries") {
                setState(AssignReinforcement);
                return true;
            } else {
                std::cout << "Invalid command in PlayersAdded state. Only allows addplayer or assigncountries." << std::endl;
                return false;
            }
        // AssignReinforcement, can move to IssueOrders or stay in AssignReinforcement
        case AssignReinforcement:
            if (command == "issueorder") {
                setState(IssueOrders);
                return true;
            } else {
                std::cout << "Invalid command in AssignReinforcement state. Only allows issueorder." << std::endl;
                return false;
            }
        // IssueOrders, can move to ExecuteOrders or stay in IssueOrders
        case IssueOrders:
            if (command == "issueorder") {
                return true;
            } else if (command == "endissueorders") {
                setState(ExecuteOrders);
                return true;
            } else {
                std::cout << "Invalid command in IssueOrders state. Only allows issueorder or endissueorders." << std::endl;
                return false;
            }
        // ExecuteOrders, can move to AssignReinforcement or Win or stay in ExecuteOrders
        case ExecuteOrders:
            if (command == "endexecorders") {
                setState(AssignReinforcement);
                return true;
            } else if (command == "win") {
                setState(Win);
                return true;
            } else if (command == "execorders") {
                return true;
            } else {
                std::cout << "Invalid command in ExecuteOrders state. Only allows execorders, endexecorders or win." << std::endl;
                return false;
            }
        // Win, can move to Start or End
        case Win:
            if (command == "play") {
                setState(Start);
                return true;
            } else if (command == "end") {
                setState(End);
                return true;
            } else {
                std::cout << "Invalid command in Win state. Only allows play or end." << std::endl;
                return false;
            }
        // End, can move to Start or stay in End
        case End:
            std::cout << "Game has ended. No further commands allowed." << std::endl;
            return false;
        default:
            std::cout << "Unknown state." << std::endl;
            return false;
     }
}

// Get the current state of the game engine as a string
std::string GameEngine::getCurrentState() {
    return stateToString(*currentState);
}

// Set the current state of the game engine
void GameEngine::setState(state newState) {
    *currentState = newState;
}

// Stream insertion operator for GameEngine
std::ostream& operator<<(std::ostream& os, const GameEngine& engine) {
    os << "GameEngine State: " << stateToString(*engine.currentState);
    return os;
}