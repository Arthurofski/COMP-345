#include "GameEngine.hpp"
#include <iostream>

int main() {
    // Welcome Message
    std::cout << "=== Game Engine State Machine Demo ===" << std::endl << std::endl;

    // Create a GameEngine instance
    GameEngine engine;

    // Display the initial state
    std::cout << "Initial State: " << engine.getCurrentState() << std::endl << std::endl;
    std::cout << "Enter commands to transition between states (type 'exit' to quit):" << std::endl;

    // Main loop to show change of states
    while (true) {
        // Prompt the user for a command
        std::string command;
        std::cin >> command;

        // Check for exit command or validate the command and transition states accordingly
        if (command == "exit") {
            std::cout << "Exiting the game engine demo." << std::endl;
            break;
        } else {
            if (engine.stateValidation(command)) {
                std::cout << "Current State: " << engine.getCurrentState() << std::endl << std::endl;
            } else {
                std::cout << "Command '" << command << "' is not valid in the current state." << std::endl << std::endl;
            }
        }
    }

    return 0;
}
