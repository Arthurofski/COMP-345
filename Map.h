#ifndef MAP_H
#define MAP_H
#include <string>
#include <vector>
#include "Player.h"

// Empty player class, so that territories can have an association with a player 
class Player;

class Continent;

// All class definitions

//Single territory owned by Player belonging to only 1 Continent
class Territory {
    public:    
        std::string* name;
        Player* owner;
        Continent* continent;
        std::vector<Territory*>* neighbours;
        int* armies;  // number of armies stationed on this territory

        //Parameterized constructor + copy constructor
        Territory(std::string n);
        Territory(const Territory& other);
        ~Territory(); //destructor

        void addNeighbour(Territory* t);
        void setContinent(Continent* c);

        std::string getName() const; //name getter
        Player* getOwner() const;
        void setOwner(Player* player);

        Territory& operator=(const Territory& other);
        friend std::ostream& operator<<(std::ostream& os, const Territory& territory);
};

class Continent {
    public:
        std::string* name;
        int* bonus;
        std::vector<Territory*>* territories;

        //Default constructor, parameterized constructor, copy constructor
        Continent();
        Continent(std::string n, int b);
        Continent(const Continent& other);
        ~Continent(); //destructor


        //stream operator and assignment operator
        friend std::ostream& operator<<(std::ostream& os, const Continent& continent);
        Continent& operator=(const Continent& other);

};

class Map {
    public:    
        std::vector<Territory*>* territories;
        std::vector<Continent*>* continents;


        //default constructor, copy constructor
        Map();
        Map(const Map& other);
        ~Map();

        bool verifyMap();
        bool verifyContinents();
        bool verifyTerritories();
        bool validate();

        Map& operator=(const Map& other);
        friend std::ostream& operator<<(std::ostream& os, const Map& map);
};

class MapLoader {
    public:
        static Map* load(std::string filename);
};

#endif
