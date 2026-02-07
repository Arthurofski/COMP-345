#ifndef TERRITORY_H 
#define TERRITORY_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Territory {
public:
    Territory();
    Territory(string cont, string nm, int armyCnt, bool occ, string oB);
    int getArmyCount() const;
    string getContinent() const;
    string getName() const;
    bool isOccupied() const;
    string getOccupiedBy() const;
    void setArmyCount(int armyCnt);
    void setOccupied(bool occ);
    void setContinent(string cont);
    void setName(string nm);
    void incrementArmyCount();  
    void setOccupiedBy(string oB);
private:
    string continent;
    string name;
    int armyCount;
    bool occupied;
    string occupiedBy;
};


class Map {
public:
    Map();
    Map(const Map& other);
    Map& operator=(const Map& other); 
    friend std::ostream& operator<<(std::ostream& out, const Map& m);
    ~Map(); // Will be a cleanup helper

    void addTerritory(Territory* t);
    void addContinent(Continent* c);

    std::vector<Territory*> getTerritories() const; // All territories in map
    std::vector<Continent*> getContinents() const; // All continents in map

    bool validate(); // Will check if map is a connected graph, a continent is a connected subgraph & territory belongs to exactly one continent

};


class Continent {
public:

};
    




#endif //TERRITORY_H
