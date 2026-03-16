#ifndef COMMANDPROCESSING_H
#define COMMANDPROCESSING_H

#include "LoggingObserver.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
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
	~Command();
	void saveEffect(string effect);
	std::string stringToLog() const override;
};

// CommandProcessor class declaration
class CommandProcessor : public Subject, public ILoggable {
private:
	vector<Command*> command_list;
	string readCommand(GameEngine* ge);

public:
	CommandProcessor();
	~CommandProcessor();
	string getCommand(GameEngine* ge);
	bool validate(string cmd_input, GameEngine* ge);
	void saveCommand(Command* new_cmd);
	std::string stringToLog() const override;
};

class FileLineReader {
private:
    std::ifstream file;
    std::string filename;

public:
    FileLineReader(const std::string& filename);
    ~FileLineReader();
    std::string readNextLine();
    bool notDone() const;
};

// FileCommandProcessorAdapter class declaration
class FileCommandProcessorAdapter : public CommandProcessor {
private:
    FileLineReader* fileReader;  // the adaptee being wrapped

    string readCommand(GameEngine* ge); // redirects to file instead of cin

public:
    FileCommandProcessorAdapter(const std::string& filename);
    ~FileCommandProcessorAdapter();
};

#endif //COMMANDPROCESSING_H
