#include "Map.h"
#include <iostream>
#include <queue>
#include <set>
#include <map>
#include <fstream>
#include <sstream>
using namespace std; 

// ------------ TERRITORY --------------

// Constructor for a territory object
Territory::Territory(string n) {
    name = new string(n);
    owner = nullptr;
    continent = nullptr;
    neighbours= new vector<Territory*>();
}
// Copy constructor for territory
Territory::Territory(const Territory& other) {
    name = new string(*other.name);
    owner = other.owner; //here we do a shallow copy as the owner pointer is not this object's responsibliity, its player's responsibility
    continent = nullptr;    //map copy constructor will re-establish continent
    neighbours = new vector<Territory*>(); //Map copy constructor will re-estabilish neighbours  
}

Territory::~Territory(){
    delete name;
    delete neighbours;
    name = nullptr;
    neighbours = nullptr;
    //other data members are not owned by territory 
}
// Associates a territory & continent with eachother
void Territory::setContinent(Continent* c) {
    continent = c;
    c->territories->push_back(this);
}

// Associates a territory with a given neighbour
void Territory::addNeighbour(Territory* t) {
    neighbours->push_back(t);
}


string Territory::getName() const {
    return *name;
}
Territory& Territory::operator=(const Territory& other) {
    if (this != &other) {
        delete name;
        name = new string(*other.name);
        owner = other.owner;
    }
    return *this;
}

// Stream insertion operator for Territory
ostream& operator<<(ostream& os, const Territory& territory) {
    os << "Territory: " << *territory.name;
    
    if (territory.continent) {
        os << " Continent: " << *territory.continent->name << ")";
    } else {
        os << " No Continent";
    }
    
    if (territory.owner) {
        os << " is Occupied ";
    } else {
        os << " is Unoccupied";
    }
    
    os << "Neighbours: ";
    if (territory.neighbours->empty()) {
        os << "None";
    } else {
        for (size_t i = 0; i < territory.neighbours->size(); ++i) {
            os << *(*territory.neighbours)[i]->name;
            if (i < territory.neighbours->size() - 1) {
                os << ", ";
            }
        }
    }
    
    return os;
}

// ------------ CONTINENT --------------

// Default constructor for continent
Continent::Continent() {
    name = new string("");
    bonus = new int(0);
    territories = new vector<Territory*>();
}

// Constructor for a continent object given a name and army bonus
Continent::Continent(string n, int b) {
    name = new string(n);
    bonus = new int(b);
    territories = new vector<Territory*>();
}

// Copy constructor for continent
Continent::Continent(const Continent& other) {
    name = new string(*other.name);
    bonus = new int(*other.bonus);
    territories = new vector<Territory*>(); // re-populated by Map copy constructor
}
Continent::~Continent(){
    delete name;
    delete bonus;
    delete territories;
    name = nullptr;
    bonus = nullptr;
    territories = nullptr; 
}
Continent& Continent::operator=(const Continent& other) {
    if(this != &other){
        delete name;
        delete bonus;
        territories->clear();
        name = new string(*other.name);
        bonus = new int(*other.bonus);
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Continent& continent){
    os << "Continent: " << *continent.name 
       << " (Bonus: " << *continent.bonus << " armies)";
    
    os << "Territories (" << continent.territories->size() << "): ";
    
    if (continent.territories->empty()) {
        os << "None";
    } else {
        for (size_t i = 0; i < continent.territories->size(); ++i) {
            os << *(*continent.territories)[i]->name;
            if (i < continent.territories->size() - 1) {
                os << ", ";
            }
        }
    }
    
    return os;
}

// ------------ MAP --------------

// Default map constructor
Map::Map() {
    territories = new vector<Territory*>();
    continents = new vector<Continent*>();
}

// Map destructor
Map::~Map() {
    for (Territory* t : *territories) delete t;
    for (Continent* c : *continents) delete c;
    delete territories;
    delete continents;
    territories = nullptr;
    continents = nullptr;
}

// Uses BFS to verify if a map is a connected graph
bool Map::verifyMap() {
    if (territories->empty()) 
        return false;

    set<Territory*> visited;
    queue<Territory*> q;

    Territory* start = (*territories)[0];
    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        Territory* current = q.front();
        q.pop();

        for (Territory* n : *current->neighbours) {
            if (visited.find(n) == visited.end()) {
                visited.insert(n);
                q.push(n);
            }
        }
    }
    return visited.size() == territories->size();
}

// Uses BFS to verify if a continent is a connected subgraph
bool Map::verifyContinents() {
    for (Continent* c : *continents) {

        if (c->territories->empty()) 
            return false;

        set<Territory*> visited;
        queue<Territory*> q;

        Territory* start = (*c->territories)[0];
        q.push(start);
        visited.insert(start);

        while (!q.empty()) {
            Territory* current = q.front();
            q.pop();

            for (Territory* n : *current->neighbours) {

                // difference from map connectivity
                if (n->continent == c && !visited.count(n)) {
                    visited.insert(n);
                    q.push(n);
                }
            }
        }

        if (visited.size() != c->territories->size())
            return false;
    }

    return true;
}

// Verifies that all territories are associated with 1 continent
bool Map::verifyTerritories() {
    for (Territory* t : *territories) {
        if (t->continent == nullptr)
            return false;
    }
    return true;
}

// Runs the 3 methods above to fully validate the map
bool Map::validate() {
    return verifyMap() && verifyContinents() && verifyTerritories();
}

// Copy constructor for Map
Map::Map(const Map& other) {
    territories = new vector<Territory*>();
    continents  = new vector<Continent*>();
 
    // Copy continents (without territories yet)
    for (Continent* c : *other.continents) {
        continents->push_back(new Continent(*c->name, *c->bonus));
    }
 
    // Copy territories (without neighbours or continent yet)
    for (Territory* t : *other.territories) {
        territories->push_back(new Territory(*t->name));
    }
 
    // Re-link territory -> continent
    for (size_t i = 0; i < other.territories->size(); ++i) {
        Territory* oldT = (*other.territories)[i];
        Territory* newT = (*territories)[i];
        for (size_t j = 0; j < other.continents->size(); ++j) {
            if (oldT->continent == (*other.continents)[j]) {
                newT->setContinent((*continents)[j]);
                break;
            }
        }
    }
 
    // Re-link neighbour adjacency
    for (size_t i = 0; i < other.territories->size(); ++i) {
        Territory* oldT = (*other.territories)[i];
        Territory* newT = (*territories)[i];
        for (Territory* oldN : *oldT->neighbours) {
            for (size_t k = 0; k < other.territories->size(); ++k) {
                if ((*other.territories)[k] == oldN) {
                    newT->addNeighbour((*territories)[k]);
                    break;
                }
            }
        }
    }
}

Map& Map::operator=(const Map& other) {
    if (this != &other) {
        // Delete existing data
        for (Territory* t : *territories) delete t;
        for (Continent* c : *continents) delete c;
        territories->clear();
        continents->clear();
        
        for (Continent* c : *other.continents)
            continents->push_back(new Continent(*c->name, *c->bonus));
 
        for (Territory* t : *other.territories)
            territories->push_back(new Territory(*t->name));
 
        for (size_t i = 0; i < other.territories->size(); ++i) {
            Territory* oldT = (*other.territories)[i];
            Territory* newT = (*territories)[i];
            for (size_t j = 0; j < other.continents->size(); ++j) {
                if (oldT->continent == (*other.continents)[j]) {
                    newT->setContinent((*continents)[j]);
                    break;
                }
            }
        }
 
        for (size_t i = 0; i < other.territories->size(); ++i) {
            Territory* oldT = (*other.territories)[i];
            Territory* newT = (*territories)[i];
            for (Territory* oldN : *oldT->neighbours) {
                for (size_t k = 0; k < other.territories->size(); ++k) {
                    if ((*other.territories)[k] == oldN) {
                        newT->addNeighbour((*territories)[k]);
                        break;
                    }
                }
            }
        }
    }
    return *this;
}

ostream& operator<<(ostream& os, const Map& m) {
    os << "Total Continents: " << m.continents->size()
       << "\nTotal Territories: " << m.territories->size() << "\n\n";
 
    os << "CONTINENTS:\n";
    for (size_t i = 0; i < m.continents->size(); ++i)
        os << "  " << (i + 1) << ". " << *(*m.continents)[i] << "\n";
 
    os << "\nTERRITORIES:\n";
    for (size_t i = 0; i < m.territories->size(); ++i)
        os << "  " << (i + 1) << ". " << *(*m.territories)[i] << "\n";
 
    return os;
}

// ------------ MAPLOADER --------------

// Creates a map from a .map file
Map* MapLoader::load(string filename) {
    Map* gamemap = new Map();
    enum Mode { NONE, CONTINENTS, TERRITORIES, BORDERS };
    Mode current = NONE;
    map<int, Territory*> terr_ids;

    ifstream file(filename);
    if (!file) {
        cout << "Cannot open file." << endl;
        return gamemap;
    }

    // switches modes when encountering a new "section" in the .map file 
    string line;
    while (getline(file, line)) {

        if (line.empty()) 
            continue;
        if (line == "[continents]") {
            current = CONTINENTS;
            continue;
        }
        if (line == "[countries]") {
            current = TERRITORIES;
            continue;
        }
        if (line == "[borders]") {
            current = BORDERS;
            continue;
        }


        // LOADING CONTINENTS
        
        if (current == CONTINENTS) {
            string name, colour; // continent name, and an unused colour string
            int bonus; // bonus armies for acquiring all territories on a continent

            stringstream ss(line);
            ss >> name >> bonus >> colour;

            Continent* new_c = new Continent(name, bonus); // creates continent with .map info
            gamemap->continents->push_back(new_c); // adds it to the map's continents vector
        }


        // LOADING TERRITORIES

        if (current == TERRITORIES) {
            int id, cont_id, x , y; // territory id, continent id, and 2 unused numbers
            string tname; // territory name

            stringstream ss(line);
            ss >> id >> tname >> cont_id >> x >> y;

            Territory* new_t = new Territory(tname);

            // store in map
            gamemap->territories->push_back(new_t);
            terr_ids[id] = new_t;

            if (cont_id < 1 || cont_id > (int)gamemap->continents->size()) {
                cout << "ERROR: Territory " << tname << " has invalid continent ID " << cont_id << endl;
                continue; 
            }

            // assign continent 
            Continent* c = (*gamemap->continents)[cont_id - 1];
            new_t->setContinent(c);
        }


        // LOADING EDGES

        if (current == BORDERS) {
            stringstream ss(line);

            int id; 
            ss >> id;
            Territory* t = terr_ids[id];

            int neighbour_id;
            while (ss >> neighbour_id) {
                t->addNeighbour(terr_ids[neighbour_id]);
            }
        }
    }
    return gamemap;
}
