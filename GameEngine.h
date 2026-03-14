#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <string>
#include <iostream>

// Enum to represent the different states of the game engine
enum state{
    Start,
    MapLoaded,
    MapValidated,
    PlayersAdded,
    AssignReinforcement,
    IssueOrders,
    ExecuteOrders,
    Win,
    End
};

// Function to convert state enum to string for display purposes
std::string stateToString(state s);

// GameEngine class definition
class GameEngine {
public:
    // Constructors, destructor and operators
    GameEngine();
    GameEngine(const GameEngine& other);
    GameEngine(state currState);
    ~GameEngine();
    GameEngine& operator=(const GameEngine& other);
    friend std::ostream& operator<<(std::ostream& os, const GameEngine& engine);

    // Main functions
    bool stateValidation(const std::string& command);

    // Getter for the current state
    std::string getCurrentState();

private:
    // currentState pointer
    state* currentState;

    // Setter for the current state
    void setState(state newState);
};

#endif