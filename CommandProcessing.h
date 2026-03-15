#ifndef COMMANDPROCESSING_H
#define COMMANDPROCESSING_H

#include "LoggingObserver.h"
#include <iostream>
#include <vector>
#include <string>
#include "GameEngine.h"
using namespace std;

class GameEngine;

// Command class declaration
class Command : public Subject, public ILoggable {
private:
	string command;
	string effect;

public:
	Command();
	Command(string command);
	void saveEffect(string effect);
	std::string stringToLog() const override;
};

// CommandProcessor class declaration
class CommandProcessor : public Subject, public ILoggable {
private:
	vector<Command*> command_list;
	void readCommand(GameEngine* ge);
	string readCommand(GameEngine* ge);
	void saveCommand(Command* new_cmd);

public:
	CommandProcessor();
	string getCommand(GameEngine* ge);
	bool validate(string cmd_input, GameEngine* ge);
	void saveCommand(Command* new_cmd);
	std::string stringToLog() const override;
};

// FileCommandProcessorAdapter class declaration
class FileCommandProcessorAdapter {

};

#endif //COMMANDPROCESSING_H
