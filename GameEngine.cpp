#include "GameEngine.h"
#include "CommandProcessing.h"
#include "PlayerStrategies.h"
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>

static PlayerStrategy* createStrategyByName(const std::string& strategyName) {
    if (strategyName == "Aggressive") return new AggressivePlayerStrategy();
    if (strategyName == "Benevolent") return new BenevolentPlayerStrategy();
    if (strategyName == "Neutral") return new NeutralPlayerStrategy();
    if (strategyName == "Cheater") return new CheaterPlayerStrategy();
    return nullptr;
}

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
    players = new std::vector<Player*>();
    map=nullptr;
    deck = new Deck(40);
}

// Copy constructor for GameEngine when given another GameEngine object
GameEngine::GameEngine(const GameEngine& other) {
    currentState = new state(*(other.currentState));
    players = new std::vector<Player*>();
    map = other.map ? new Map(*other.map): nullptr;
    deck = new Deck(40);
    for(Player* p : *other.players){
        players->push_back(new Player(*p));
    }
}

// Constructor for GameEngine when given a state
GameEngine::GameEngine(state currState) {
    currentState = new state(currState);
    players = new std::vector<Player*>();
    map = nullptr;
    deck = new Deck(40);
}

// Destructor for GameEngine
GameEngine::~GameEngine() {
    delete currentState;
    for (Player* p: *players)
        delete p;
    delete players;
    delete map;
    delete deck;
    players = nullptr;
    currentState=nullptr;
    map=nullptr;
    deck=nullptr;
}

// Assignment operator for GameEngine
GameEngine& GameEngine::operator=(const GameEngine& other) {
    if (this == &other) {
        return *this;
    }
    for (Player* p : *players)
        delete p;
    delete currentState;
    delete map;
    delete deck;


    currentState = new state(*(other.currentState));
    map = other.map ? new Map(*other.map):nullptr;
    deck = new Deck(40);
    for(Player* p: *other.players)
        players->push_back(new Player(*p));
    
    return *this;
}

// Main validation function to check if a command is valid and return true if it is, otherwise return false and print an error message
bool GameEngine::stateValidation(const std::string& command) {
    // Switch statement to determine valid commands based on the current state
    switch(*currentState) {
        // Start, can only move to MapLoaded
        case Start:
            if (command == "loadmap") {
                notify(this);
                setState(MapLoaded);
                return true;
            } else {
                std::cout << "Invalid command in Start state. Only allows loadmap." << std::endl;
                return false;
            }
        // MapLoaded, can move to MapValidated or stay in MapLoaded
        case MapLoaded:
            if (command == "loadmap") {
                notify(this);
                return true;
            } else if (command == "validatemap") {
                setState(MapValidated);
                notify(this);
                return true;
            } else {
                std::cout << "Invalid command in MapLoaded state. Only allows loadmap or validatemap." << std::endl;
                return false;
            }
        // MapValidated, can move to PlayersAdded or stay in MapValidated
        case MapValidated:
            if (command == "addplayer") {
                setState(PlayersAdded);
                notify(this);
                return true;
            } else {
                std::cout << "Invalid command in MapValidated state. Only allows addplayer." << std::endl;
                return false;
            }
        // PlayersAdded, can move to AssignReinforcement or stay in PlayersAdded
        case PlayersAdded:
            if (command == "addplayer") {
                notify(this);
                return true;
            } else if (command == "assigncountries") {
                setState(AssignReinforcement);
                notify(this);
                return true;
            } else {
                std::cout << "Invalid command in PlayersAdded state. Only allows addplayer or assigncountries." << std::endl;
                return false;
            }
        // AssignReinforcement, can move to IssueOrders or stay in AssignReinforcement
        case AssignReinforcement:
            if (command == "issueorder") {
                setState(IssueOrders);
                notify(this);
                return true;
            } else {
                std::cout << "Invalid command in AssignReinforcement state. Only allows issueorder." << std::endl;
                return false;
            }
        // IssueOrders, can move to ExecuteOrders or stay in IssueOrders
        case IssueOrders:
            if (command == "issueorder") {
                notify(this);
                return true;
            } else if (command == "endissueorders") {
                setState(ExecuteOrders);
                notify(this);
                return true;
            } else {
                std::cout << "Invalid command in IssueOrders state. Only allows issueorder or endissueorders." << std::endl;
                return false;
            }
        // ExecuteOrders, can move to AssignReinforcement or Win or stay in ExecuteOrders
        case ExecuteOrders:
            if (command == "endexecorders") {
                setState(AssignReinforcement);
                notify(this);
                return true;
            } else if (command == "win") {
                setState(Win);
                notify(this);
                return true;
            } else if (command == "execorders") {
                notify(this);
                return true;
            } else {
                std::cout << "Invalid command in ExecuteOrders state. Only allows execorders, endexecorders or win." << std::endl;
                return false;
            }
        // Win, can move to Start or End
        case Win:
            if (command == "play") {
                setState(Start);
                notify(this);
                return true;
            } else if (command == "end") {
                setState(End);
                notify(this);
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
std::string GameEngine::getCurrentState() const {
    return stateToString(*currentState);
}

// Set the current state of the game engine
void GameEngine::setState(state newState) {
    *currentState = newState;
    notify(this);
}
//get player count
int GameEngine::getPlayerCount() const { return players->size(); }

// get raw player list (non-owning)
std::vector<Player*>* GameEngine::getPlayers() const { return players; }

// Stream insertion operator for GameEngine
std::ostream& operator<<(std::ostream& os, const GameEngine& engine) {
    os << "GameEngine State: " << stateToString(*engine.currentState) << "\nPlayers: " << engine.players->size();
    return os;
}

//Game startup functions
void GameEngine::loadMap(Map* m){
    delete map;
    map = m;
    setState(MapLoaded);
    std::cout <<"Map loaded: " << map->territories->size() << " territories, " << map->continents->size() << " coninents.\n";
}
void GameEngine::addPlayer(const std::string& playerName){
    if(players->size() >=6){
        std::cout << "Maximum amount of players reached.\n";
    }else{
        players->push_back(new Player(playerName));
        std::cout << "Player " << playerName << " added\n";
    }
}

void GameEngine::assignCountries(){
    if(!map || players->empty())
        return;
    int pie = 0;
    for (Territory* t: *map->territories){
        (*players)[pie %players->size()]->addTerritory(t);
        *t->armies = 3;//each territory starts with 3 armies can be changed according to warzone rules
        pie++;
    }
    // Deal 2 cards to each player from the shared deck
    for (Player* p : *players) {
        if (deck->getNumCards() >= 2) {
            deck->draw(p->getHand());
            deck->draw(p->getHand());
        }
    }
 
    setState(AssignReinforcement);
    std::cout << "Countries assigned. Each player starts with "
         << (*players)[0]->getTerritories()->size() << " territories (approx).\n";
}

//winning and elimination methods
// BUG FIX 1: The original required the last player to own every territory on
// the map. This breaks Demo 7 where Bob starts with 0 territories and is
// eliminated immediately — Alice never "captures" territories so the old
// check never fires. Being the last player standing is the correct win condition.
bool GameEngine::checkWinCondition() const {
    return players->size() == 1;
}
void GameEngine::removeEliminatedPlayers() {
    auto it = players->begin();
    while (it != players->end()) {
        if ((*it)->getTerritories()->empty()) {
            std::cout << "  *** Player '" << *(*it)->getName()
                 << "' eliminated (no territories). ***\n";
            delete *it;
            it = players->erase(it);
        } else {
            ++it;
        }
    }
}

//====================Phase 1 reinforcement=====================
//each player gets territories/3 armies minimum of 3 + continent bonus to players owning continent
void GameEngine::reinforcementPhase(){
    std::cout << "Reinforcement phase has begun \n";
    for (Player* p: *players){
        int count = p->getTerritories()->size();
        int armies = std::max(3, count/3);

        //include continent bonus
        for (Continent* c: *map->continents){
            bool ownsAll = true;
            for (Territory* t: *c->territories){
                if(t->owner !=p){
                    ownsAll=false;
                    break;
                }
            }
            if(ownsAll){
                armies += *c->bonus;
                std::cout << *p->getName() << " earns + " << *c->bonus << " (owns all of " << *c->name << ")\n";
            }
        }
        p->setReinforcementPool(armies);
        std::cout << "  " << *p->getName() << " receives " << armies << " armies (owns " << count << " territories).\n";
    }
}
//=======================Phase 2 issue orders=====================
//round robin calls to each player's issueOrder() until all players emptied their reinforcement pool and issued at least one advance/card order
void GameEngine::issueOrdersPhase(){
    std::cout << "Issue Order phase has begun\n";
    bool poolLeft = true;
    while(poolLeft){
        poolLeft=false;
        for(Player* p: *players){
            if(p->getReinforcementPool()>0){
                p->issueOrder(deck);
                poolLeft=true;
            }
        }
    }
    std::cout << "All reinforcement deployed. Issuing advance/card orders... \n";
    for(Player* p: *players){
        p->issueOrder(deck);
    }
}

//=======================Phase 3 execute order======================
//order is: 1-Execute all deploy orders accross all players first 
//2:then execute remiaining orders until all lists are empty
//after execution we remove eliminated players.
void GameEngine::executeOrdersPhase(){
    // Step 1: Execute all deploys first
    std::cout << "\n--- Execute Orders Phase ---\n";
 
    // Helper: find the index of the first Deploy order in a player's list.
    auto firstDeployIndex = [](OrdersList* ol) -> int {
        for (int i = 0; i < ol->size(); ++i)
            if (dynamic_cast<const Deploy*>(ol->get(i))) return i;
        return -1;
    };
 
    // Step 0: Restore each player's pool to cover their queued Deploy orders.
    // issueOrder() decremented the pool per order queued; Deploy::execute() will
    // deduct it again, so we add back exactly what was queued.
    for (Player* p : *players) {
        int deployTotal = 0;
        for (int i = 0; i < p->getOrders()->size(); ++i) {
            const Deploy* d = dynamic_cast<const Deploy*>(p->getOrders()->get(i));
            if (d) deployTotal += d->getArmies();
        }
        p->addReinforcementPool(deployTotal);  // restore to pre-issue level
    }
 
    // Step 1: Execute all Deploy orders first, round-robin across players.
    std::cout << "  Executing Deploy orders...\n";
    bool anyDeploy = true;
    while (anyDeploy) {
        anyDeploy = false;
        for (Player* p : *players) {
            int idx = firstDeployIndex(p->getOrders());
            if (idx >= 0) {
                p->getOrders()->move(idx, 0);
                const Order* co = p->getOrders()->get(0);
                Order* o = const_cast<Order*>(co);
                o->execute();
                p->getOrders()->remove(0);
                anyDeploy = true;
            }
        }
    }
 
    // Step 2: Execute remaining orders round-robin.
    std::cout << "  Executing remaining orders...\n";
    bool anyLeft = true;
    while (anyLeft) {
        anyLeft = false;
        for (Player* p : *players) {
            if (p->getOrders()->size() > 0) {
                const Order* co = p->getOrders()->get(0);
                Order* o = const_cast<Order*>(co);
                o->execute();
                p->getOrders()->remove(0);
                anyLeft = true;
            }
        }
    }
 
    removeEliminatedPlayers();
}

void GameEngine::mainGameLoop(){
    if (!map || players->empty()) {
        std::cout << "Cannot start game: map not loaded or no players.\n";
        return;
    }
 
    std::cout << "\n========== MAIN GAME LOOP ==========\n";
    setState(AssignReinforcement);

    // BUG FIX 2: Eliminate any players who already start with 0 territories
    // (e.g. Demo 7 where Bob is given no territories before the loop begins).
    // Without this, checkWinCondition() is never evaluated before round 1,
    // so the loop runs with a "dead" player still in the list.
    removeEliminatedPlayers();
    if (checkWinCondition()) {
        setState(Win);
        std::cout << "\n*** " << *(*players)[0]->getName()
                  << " wins the game! (opponent had no territories) ***\n";
        return;
    }
 
    int round = 0;
    const int MAX_ROUNDS = 10;  // safety cap to prevent infinite loops in demo
 
    while (!checkWinCondition() && round < MAX_ROUNDS) {
        round++;
        std::cout << "\n====== Round " << round << " ======"
             << "  Players remaining: " << players->size() << "\n";
 
        reinforcementPhase();
 
        setState(IssueOrders);
        issueOrdersPhase();
 
        setState(ExecuteOrders);
        executeOrdersPhase();
 
        // Check for winner after each round
        if (checkWinCondition()) break;
 
        // All surviving players move to next reinforcement phase
        setState(AssignReinforcement);
    }
 
    if (players->size() == 1) {
        setState(Win);
        std::cout << "\n*** " << *(*players)[0]->getName()
             << " wins the game! ***\n";
    } else if (round >= MAX_ROUNDS) {
        std::cout << "\n[Reached maximum of " << MAX_ROUNDS << " rounds. Declaring draw.]\n";
        std::cout << "Remaining players: ";
        for (Player* p : *players) std::cout << *p->getName() << " ";
        std::cout << "\n";
    }
}

bool GameEngine::runTournament(const std::vector<std::string>& mapFiles,
    const std::vector<std::string>& strategyNames,
    int numGames,
    int maxTurns) {

    std::ofstream log("gamelog.txt", std::ios::app);
    if (!log) {
        std::cout << "Failed to open log file for tournament output.\n";
    }

    auto join = [](const std::vector<std::string>& items) {
        std::string out;
        for (size_t i = 0; i < items.size(); ++i) {
            out += items[i];
            if (i + 1 < items.size()) out += ", ";
        }
        return out;
    };

    std::cout << "\nTournament mode:\n";
    log << "Tournament mode:" << std::endl;
    std::cout << "M: " << join(mapFiles) << "\n";
    std::cout << "P: " << join(strategyNames) << "\n";
    std::cout << "G: " << numGames << "\n";
    std::cout << "D: " << maxTurns << "\n";
    log << "M: " << join(mapFiles) << std::endl;
    log << "P: " << join(strategyNames) << std::endl;
    log << "G: " << numGames << std::endl;
    log << "D: " << maxTurns << std::endl;
    log << "Results:" << std::endl;

    for (size_t mapIndex = 0; mapIndex < mapFiles.size(); ++mapIndex) {
        const std::string& mapFile = mapFiles[mapIndex];
        Map* loadedMap = MapLoader::load(mapFile);
        if (!loadedMap) {
            std::cout << "Failed to load map: " << mapFile << "\n";
            return false;
        }
        if (!loadedMap->validate()) {
            std::cout << "Invalid map in tournament: " << mapFile << "\n";
            delete loadedMap;
            return false;
        }

        std::cout << "Map " << (mapIndex + 1) << ": " << mapFile << "\n";
        log << "Map " << (mapIndex + 1) << ": " << mapFile << std::endl;

        for (int gameIndex = 0; gameIndex < numGames; ++gameIndex) {
            GameEngine tournamentGame;
            tournamentGame.loadMap(new Map(*loadedMap));

            for (size_t playerIndex = 0; playerIndex < strategyNames.size(); ++playerIndex) {
                const std::string& strategyName = strategyNames[playerIndex];
                std::string playerName = strategyName + std::to_string(playerIndex + 1);
                tournamentGame.addPlayer(playerName);
                Player* p = (*tournamentGame.getPlayers())[playerIndex];
                PlayerStrategy* strat = createStrategyByName(strategyName);
                if (!strat) {
                    std::cout << "Unknown strategy: " << strategyName << "\n";
                    delete loadedMap;
                    return false;
                }
                p->setStrategy(strat);
            }

            tournamentGame.assignCountries();
            tournamentGame.mainGameLoop(maxTurns);

            std::string gameResult;
            if (tournamentGame.getPlayers()->size() == 1) {
                gameResult = *(*tournamentGame.getPlayers())[0]->getName();
            } else {
                gameResult = "Draw";
            }

            std::cout << "Game " << (gameIndex + 1) << ": " << gameResult << "\n";
            log << "Game " << (gameIndex + 1) << ": " << gameResult << std::endl;
        }

        delete loadedMap;
    }

    log << std::endl;
    return true;
}

std::string GameEngine::stringToLog() const {
    std::string log = "GameEngine transitioned to State: " + stateToString(*currentState);
    return log;
}

void GameEngine::startupPhase() {
    std::cout << "\n----- Warzone - Startup Phase: -----\n"
              << "Commands: loadmap <file>  validatemap  addplayer <name>  gamestart  tournament\n";

    CommandProcessor* cmdproc = new CommandProcessor();
    std::string line;
    while (true) {
        line = cmdproc->getCommand(this);
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd; 
        Command* new_cmd = new Command(cmd); 

        // ---- loadmap <filename> ----------------------------------------
        if (cmd == "loadmap") {
            std::string filename;
            if (!(iss >> filename)) {
                std::cout << "Usage: loadmap <filename>\n";
                continue;
            }
            // Valid in Start or MapLoaded states only
            if (*currentState != Start && *currentState != MapLoaded) {
                std::cout << "Cannot use 'loadmap' in state: " << getCurrentState() << "\n";
                continue;
            }
            Map* loaded = MapLoader::load(filename);
            if (!loaded) {
                std::cout << "ERROR: failed to load map from '" << filename << "'.\n";
                continue;
            }
            new_cmd->saveEffect("Loaded map: " + filename);
            cmdproc->saveCommand(new_cmd);
            loadMap(loaded);   // calls setState(MapLoaded) internally

        // ---- validatemap -----------------------------------------------
        } else if (cmd == "validatemap") {
            if (*currentState != MapLoaded) {
                std::cout << "Must be in MapLoaded state. Current: " << getCurrentState() << "\n";
                continue;
            }
            if (!map) {
                std::cout << "No map loaded. Use 'loadmap <filename>' first.\n";
                continue;
            }
            std::cout << "Validating map...\n";
            if (map->validate()) {
                std::cout << "Map is VALID.\n";
                setState(MapValidated);
            } else {
                std::cout << "Map is INVALID. Please load a different map.\n";
                new_cmd->saveEffect("Invalid map");
                cmdproc->saveCommand(new_cmd);
                
                // Reset so the user can try a new file
                delete map;
                map = nullptr;
                setState(Start);
            }

        // ---- addplayer <n> -------------------------------------------
        } else if (cmd == "addplayer") {
            std::string playerName;
            if (!(iss >> playerName)) {
                std::cout << "Usage: addplayer <n>\n";
                continue;
            }
            if (*currentState != MapValidated && *currentState != PlayersAdded) {
                std::cout << "Cannot add players in state: " << getCurrentState() << "\n";
                continue;
            }
            if (players->size() >= 6) {
                std::cout << "Maximum of 6 players already reached.\n";
                continue;
            }
            // addPlayer() handles the push and prints confirmation
            addPlayer(playerName);
            // Advance state MapValidated -> PlayersAdded on first player added
            if (*currentState == MapValidated)
                setState(PlayersAdded);

        // ---- tournament -------------------------------------------------
        } else if (cmd == "tournament") {
            if (*currentState != Start) {
                std::cout << "Cannot use 'tournament' in state: " << getCurrentState() << "\n";
                continue;
            }

            std::vector<std::string> maps;
            std::vector<std::string> strategies;
            int num_games = 0;
            int max_turns = 0;

            if (!cmdproc->parseTournamentCommand(line, maps, strategies, num_games, max_turns)) {
                std::cout << "Invalid tournament command. Usage: tournament -M <listofmapfiles> -P <listofplayerstrategies> -G <numberofgames> -D <maxnumberofturns>\n";
                continue;
            }

            if (!runTournament(maps, strategies, num_games, max_turns)) {
                std::cout << "Tournament execution failed. Check map files and strategy names.\n";
            }
            continue;

        // ---- gamestart -------------------------------------------------
        } else if (cmd == "gamestart") {
            if (*currentState != PlayersAdded) {
                std::cout << "Cannot use 'gamestart' in state: " << getCurrentState() << "\n";
                continue;
            }
            if (players->size() < 2) {
                std::cout << "Need at least 2 players. Currently have "
                          << players->size() << ".\n";
                continue;
            }

            std::cout << "\n--- gamestart ---\n";
            
            // (a) Fairly distribute all territories round-robin
            std::cout << "\n(a) Distributing territories...\n";
            {
                // Shuffle territory list so distribution is random each game
                std::vector<Territory*> all = *map->territories;
                std::mt19937 rng(std::random_device{}());
                std::shuffle(all.begin(), all.end(), rng);

                size_t n = players->size();
                for (size_t i = 0; i < all.size(); ++i) {
                    Player* owner = (*players)[i % n];
                    owner->addTerritory(all[i]);
                    *all[i]->armies = 3;
                }
                for (Player* p : *players)
                    std::cout << "  " << *p->getName() << ": "
                              << p->getTerritories()->size() << " territories\n";
            }
        } else {
            std::cout << "Unknown command: '" << cmd
                      << "'. Valid: loadmap, validatemap, addplayer, gamestart\n";
        }
        delete new_cmd;
        new_cmd = nullptr;
    }
    delete cmdproc;
    cmdproc = nullptr;
}
