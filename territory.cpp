#include <iostream>


using namespace std;

#include "territory.h"



Territory::Territory(){
    string continent = "None";
    string name = "None";
    int armyCount = 0;
    bool occupied = false;
    string occupiedBy = "None";
}

Territory::Territory(string cont, string nm, int armyCnt, bool occ, string oB){

    if (armyCnt < 0 || cont == "" || nm == "" || oB == ""){
        cout << "Illegal argument to Territory constructor.";
        exit(1);
    }
    if(cont != "North America" && cont != "South America" &&
       cont != "Europe" && cont != "Africa" &&
       cont != "Asia" && cont != "Australia"){
        cout << "Illegal argument to Territory constructor.";
        exit(1);
    }
    continent = cont;
    name = nm;
    armyCount = armyCnt;
    occupied = occ;
    occupiedBy = oB;
}

int Territory::getArmyCount() const{
    return armyCount;
}
string Territory::getContinent() const{
    return continent;
}
string Territory::getName() const{
    return name;
}
bool Territory::isOccupied() const{
    return occupied;
}
string Territory::getOccupiedBy() const{
    return occupiedBy;
}
void Territory::setArmyCount(int armyCnt){
    armyCount = armyCnt;
}
void Territory::setOccupied(bool occ){
    occupied = occ;
}
void Territory::setContinent(string cont){
    continent = cont;
}
void Territory::setName(string nm){
    name = nm;
}

void Territory::incrementArmyCount(){
    armyCount++;
}
void Territory::setOccupiedBy(string oB){
    occupiedBy = oB;
}
