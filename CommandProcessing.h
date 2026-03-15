#ifndef COMMANDPROCESSING_H
#define COMMANDPROCESSING_H

#include <iostream>
#include <vector>
#include <string>
#include "GameEngine.h"
using namespace std;

// Command class declaration
class Command {
private:
	string command;
	string effect;

public:
	Command();
	Command(string command);
	void saveEffect(string effect);
};

// CommandProcessor class declaration
class CommandProcessor {
private:
	vector<Command*> command_list;
	string readCommand(GameEngine* ge);
	void saveCommand(Command* new_cmd);

public:
	CommandProcessor();
	string getCommand(GameEngine* ge);
	bool validate(string cmd_input, GameEngine* ge);
};

// FileCommandProcessorAdapter class declaration
class FileCommandProcessorAdapter {

};

#endif
