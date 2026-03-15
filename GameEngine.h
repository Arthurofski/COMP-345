#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Cards.h"
#include "Orders.h"
#include "Player.h"
#include "Map.h"

class Player;


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
    void loadMap(Map* map);
    void addPlayer(const std::string& playerName); //adds players to the game (max 6)
    void assignCountries();//distribute map terrritories among players and set initial army

    //Part 3 methods
    //Reinforcement phase method
    void reinforcementPhase();
    void issueOrdersPhase();
    void executeOrdersPhase();
    //main game loop method containing the 3 phases
    void mainGameLoop();

    // Getter for the current state
    std::string getCurrentState() const;
    int getPlayerCount() const;

private:
    // currentState pointer
    state* currentState;
    std::vector<Player*>* players;
    Map* map;
    Deck* deck;
    // Setter for the current state
    void setState(state newState);
    bool checkWinCondition() const;
    void removeEliminatedPlayers();
};

#endif //GAMEENGINE_H
