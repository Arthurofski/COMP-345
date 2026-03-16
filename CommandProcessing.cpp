#include "CommandProcessing.h"
#include "GameEngine.h"

// ---------------------------COMMAND CLASS IMPLEMENTATION-----------------------------

// Default command constructor
Command::Command() {}

// Command destructor
Command::~Command() {}

// Copy constructor
Command::Command(const Command& other) {
    this->command = other.command;
    this->effect  = other.effect;
}

// Assignment operator
Command& Command::operator=(const Command& other) {
    if (this != &other) 
        this->command = other.command;
    return *this;
}

// Stream insertion operator 
ostream& operator<<(ostream& os, const Command& cmd) {
    os << "Command: " << cmd.command;
    return os;
}

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

// CommandProcessor default constructor
CommandProcessor::CommandProcessor() {}

// CommandProcessor destructor
CommandProcessor::~CommandProcessor() {
    for (Command* cmd : command_list) {
        delete cmd;
    }
    command_list.clear();
}

CommandProcessor::CommandProcessor(const CommandProcessor& other) {
    for (Command* cmd : other.command_list) {
        command_list.push_back(new Command(*cmd)); 
    }
}

CommandProcessor& CommandProcessor::operator=(const CommandProcessor& other) {
    if (this != &other) {
        for (Command* cmd : command_list) {
            delete cmd;
        }
        command_list.clear();

        for (Command* cmd : other.command_list) {
            command_list.push_back(new Command(*cmd));
        }
    }
    return *this;
}

ostream& operator<<(ostream& os, const CommandProcessor& cp) {
    os << "CommandProcessor [" << cp.command_list.size() << " commands]:\n";
    for (Command* cmd : cp.command_list) {
        os << "  - " << *cmd << "\n";
    }
    return os;
}

// Reads command, validates it, then creates a command object and saves it in the command list
string CommandProcessor::readCommand(GameEngine* ge) {
	std::string line, cmd;
	Command* new_cmd;
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
		else {
			cout << "\nInvalid command!\n";
			new_cmd->saveEffect("Invalid command");
		}
	}
	if (!line.empty())
		return line;

	if (cmd == "replay") {
		new_cmd->saveEffect("Started a new game");
		std::cout << "Starting new game...\n";
		delete ge;
		ge = new GameEngine();
		ge->startupPhase();
	}

	if (cmd == "quit") {
		new_cmd->saveEffect("Quit game");
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


// -----------------------FILE LINE READER CLASS IMPLEMENTATION------------------------

FileLineReader::FileLineReader(const std::string& filename) : filename(filename) {
	file.open(filename);
	if (!file.is_open())
		cout << "Could not open file: " << filename << endl;
	else 
		cout << "Opened file: " << filename << endl;
}

FileLineReader::~FileLineReader() {
    if (file.is_open()) file.close();
}

FileLineReader::FileLineReader(const FileLineReader& other) : filename(other.filename) {
    file.open(filename); 
    if (!file.is_open())
		cout << "Could not open file: " << filename << endl;
	else 
		cout << "Opened file: " << filename << endl;
}

FileLineReader& FileLineReader::operator=(const FileLineReader& other) {
    if (this != &other) {
        if (file.is_open()) file.close();
        filename = other.filename;
        file.open(filename);
        if (!file.is_open())
			cout << "Could not open file: " << filename << endl;
		else 
			cout << "Opened file: " << filename << endl;
    }
    return *this;
}

ostream& operator<<(ostream& os, const FileLineReader& flr) {
    os << "FileLineReader [file: " << flr.filename << "]";
    return os;
}

std::string FileLineReader::readNextLine() {
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) return line;
    }
    return "";
}

bool FileLineReader::notDone() const {
    return file.good() && !file.eof();
}

// ----------------FILE COMMAND PROCESSOR ADAPTER CLASS IMPLEMENTATION------------------

FileCommandProcessorAdapter::FileCommandProcessorAdapter(const FileCommandProcessorAdapter& other) : CommandProcessor(other) {  
    this->fileReader = new FileLineReader(*other.fileReader); 
}

FileCommandProcessorAdapter& FileCommandProcessorAdapter::operator=(const FileCommandProcessorAdapter& other) {
    if (this != &other) {
        CommandProcessor::operator=(other); 
        delete fileReader;
        fileReader = new FileLineReader(*other.fileReader); 
    }
    return *this;
}

ostream& operator<<(ostream& os, const FileCommandProcessorAdapter& fcpa) {
    os << "FileCommandProcessorAdapter [file: " << *fcpa.fileReader << "]\n";
    os << static_cast<const CommandProcessor&>(fcpa); 
	return os;
}

FileCommandProcessorAdapter::FileCommandProcessorAdapter(const std::string& filename) {
    fileReader = new FileLineReader(filename);  
}

FileCommandProcessorAdapter::~FileCommandProcessorAdapter() {
    delete fileReader;
}

// Overrides readCommand to read from file instead of user input
string FileCommandProcessorAdapter::readCommand(GameEngine* ge) {
    while (fileReader->notDone()) {
        std::string line = fileReader->readNextLine();
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (validate(cmd, ge)) {
            Command* new_cmd = new Command(cmd);
            saveCommand(new_cmd); 
            return line;
        } 
		else {
            Command* new_cmd = new Command(cmd);
            new_cmd->saveEffect("Invalid command '" + cmd + "' in state " + ge->getCurrentState());
            saveCommand(new_cmd);
            std::cout << "Invalid command in file: " << cmd << "\n";
        }
    }
    std::cout << "End of command file reached.\n";
    return "";
}