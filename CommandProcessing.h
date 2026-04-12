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
	Command(const Command& other);
    Command& operator=(const Command& other);
    friend ostream& operator<<(ostream& os, const Command& cmd);
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
	CommandProcessor(const CommandProcessor& other);
    CommandProcessor& operator=(const CommandProcessor& other);
    friend ostream& operator<<(ostream& os, const CommandProcessor& cp);
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
    FileLineReader(const FileLineReader& other); 
    FileLineReader& operator=(const FileLineReader& other); 
    friend ostream& operator<<(ostream& os, const FileLineReader& flr);
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
	FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other); 
    FileCommandProcessorAdapter& operator=(const FileCommandProcessorAdapter& other);
    friend ostream& operator<<(ostream& os, const FileCommandProcessorAdapter& fcpa); 
    string getCommand(GameEngine* ge);
};

#endif //COMMANDPROCESSING_H
