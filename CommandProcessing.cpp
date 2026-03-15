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
}


// ----------------------COMMAND PROCESSOR CLASS IMPLEMENTATION------------------------

CommandProcessor::CommandProcessor() {}

// Reads command, validates it, then creates a command object and saves it in the command list
void CommandProcessor::readCommand(GameEngine* ge) {
	string cmd_input;
	do {
		cin >> cmd_input;
		if (validate(cmd_input, ge)) {
			Command* new_cmd = new Command(cmd_input);
			saveCommand(new_cmd);
		}
		else
			cout << "\nInvalid command!\n";
	} while (!validate(cmd_input, ge));
}

// Saves a command object in the command list
void CommandProcessor::saveCommand(Command* new_cmd) {
	command_list.push_back(new_cmd);
}

// Public method for getting commands from the user
void CommandProcessor::getCommand(GameEngine* ge) {
	readCommand(ge);
}

// Validates a command based on whether it exists and if it can be used in the current game state
bool CommandProcessor::validate(string cmd_input, GameEngine* ge)  {
	string state = ge->getCurrentState();

	if (cmd_input == "loadmap" && (state == "Start" || "MapLoaded"))
		return true;
	if (cmd_input == "validatemap" && (state == "MapLoaded"))
		return true;
	if (cmd_input == "addplayer" && (state == "MapValidated" || "PlayersAdded"))
		return true;
	if (cmd_input == "gamestart" && (state == "PlayersAdded"))
		return true;
	if (cmd_input == "replay" && (state == "Win"))
		return true;
	if (cmd_input == "quit" && (state == "Win"))
		return true;
		
	return false;
}
