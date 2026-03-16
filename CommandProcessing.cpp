#include "CommandProcessing.h"
#include "GameEngine.h"

// ---------------------------COMMAND CLASS IMPLEMENTATION-----------------------------

// Default command constructor
Command::Command() {}

// Command constructor with a valid command
Command::Command(string command) {
	this->command = command;
    this->effect = "unknown";
}

// Method to save a command's effect to its command object
void Command::saveEffect(string effect) {
	this->effect = effect;
	notify(this); // Notify observers with the current command object
}

// Method to convert a command's effect to a string for logging purposes
string Command::stringToLog() const {
	return "Effect: " + effect;
}

// ----------------------COMMAND PROCESSOR CLASS IMPLEMENTATION------------------------

CommandProcessor::CommandProcessor() {}

// Reads command, validates it, then creates a command object and saves it in the command list
std::string CommandProcessor::readCommand(GameEngine* ge) {
	std::string line, cmd;
    while (true) {
        std::cout << "[" << ge->getCurrentState() << "] > ";
        if (!std::getline(std::cin, line)) break; // EOF / pipe end
        if (line.empty()) continue;

        std::istringstream iss(line);
        iss >> cmd;

		if (validate(cmd, ge)) {
			Command* new_cmd = new Command(cmd);
			saveCommand(new_cmd);
			break;
		}
		else
			cout << "\nInvalid command!\n";
	}
	if (!line.empty())
		return line;

	if (cmd == "replay") {
		std::cout << "Starting new game...\n";
		delete ge;
		ge = new GameEngine();
		ge->startupPhase();
	}

	if (cmd == "quit") {
		std::cout << "Quitting game...\n";
		exit(0);
	}
	return "";
}

// Saves a command object in the command list
void CommandProcessor::saveCommand(Command* new_cmd) {
	command_list.push_back(new_cmd);
}

// Public method for getting commands from the user
string CommandProcessor::getCommand(GameEngine* ge) {
	return readCommand(ge);
}

// Validates a command based on whether it exists and if it can be used in the current game state
bool CommandProcessor::validate(string cmd_input, GameEngine* ge)  {
	string state = ge->getCurrentState();

	if (cmd_input == "loadmap" && (state == "Start" || state == "MapLoaded"))
		return true;
	if (cmd_input == "validatemap" && state == "MapLoaded")
		return true;
	if (cmd_input == "addplayer" && (state == "MapValidated" || state == "PlayersAdded"))
		return true;
	if (cmd_input == "gamestart" && state == "PlayersAdded")
		return true;
	if (cmd_input == "replay" && state == "Win")
		return true;
	if (cmd_input == "quit" && state == "Win")
		return true;
		
	return false;
}

// Method to convert a command's effect to a string for logging purposes
string CommandProcessor::stringToLog() const {
	return "Command saved to list";
}