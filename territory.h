#ifndef TERRITORY_H
#define TERRITORY_H

#include <iostream>

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



#endif //TERRITORY_H