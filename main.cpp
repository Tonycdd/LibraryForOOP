#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "LibrarySystem.h"

void printHelpMessage() {
	std::cout << "(add unit) - only admin!\n";
	std::cout << "(add user) - only admin!\n";
	std::cout << "(remove unit) - only admin!\n";
	std::cout << "(remove user) - only admin!\n";
	std::cout << "(change unit) - only admin!\n";
	std::cout << "(find users) - only admin!\n";
}

void printMessage() {
	std::cout << "Welcome to our LibrarySystem!\n";
	std::cout << "\n";
	std::cout << "Here is a list of commands, that you can perform, based on different rights (some commands are only for admins) :\n";
	std::cout << "1. (help) - different rights\n";
	std::cout << "2. (login) - after use you should input valid username and password for the system! \n";
	std::cout << "3. (logout) - current logged user is logging out from the system (BUT NOT REMOVED!)!\n";
	std::cout << "4. (exit) - end of the program!\n";
	std::cout << "5. (books all) - prints info about books in our system!\n";
	std::cout << "6. (periodicals all) - prints info about periodicals in our system!\n";
	std::cout << "7. (series all) - prints info about series in our system!\n";
	std::cout << "8. (all) - prints info about all of the units in our system!\n";
	std::cout << "9. (info)  with ISBN/ISSN number - prints detailed info about unit with current ISBN/ISSN!\n";
	std::cout << "10. (find units) - it searches a specific unit in our system and prints it out!\n";
	std::cout << "11. (find users) - it searches a specific user in our system and prints it out!\n";
	std::cout << "12. (find units) - it searches a specific unit in our system!\n";
	std::cout << "13. (add unit) - interactively creates and adds it to our system!\n";
	std::cout << "14. (add user) - interactively creates and adds it to our system!\n";
	std::cout << "15. (remove unit) with id - removes that unit from the system!\n";
	std::cout << "16. (remove user) with id - removes that user from the system!\n";
	std::cout << "17. (change unit) with id - changes interactively that unit !\n";
	std::cout << "18. (change user) - can change his password!\n";
	std::cout << "\n";
}

// Function to split a string into tokens
std::vector<std::string> tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

// Process the command and call the appropriate function in the LibrarySystem
bool processCommand(const std::vector<std::string>& tokens, LibrarySystem& system) {
    if (tokens.empty()) {
        return true; // Empty command, just continue
    }

    const std::string& command = tokens[0];

    if (command == "exit") {
        return false; // Signal to exit the loop
    }
    else if (command == "help") {
        printMessage();
    }
    else if (command == "login") {
        if (tokens.size() < 3) {
            std::cout << "Usage: login <username> <password>\n";
        } else {
            system.login(tokens[1], tokens[2]);
        }
    }
    else if (command == "logout") {
        system.logout();
    }
    else if (command == "books" && tokens.size() > 1 && tokens[1] == "all") {
        system.booksAll();
    }
    else if (command == "periodicals" && tokens.size() > 1 && tokens[1] == "all") {
        system.periodicalsAll();
    }
    else if (command == "series" && tokens.size() > 1 && tokens[1] == "all") {
        system.seriesAll();
    }
    else if (command == "all") {
        system.all();
    }
    else if (command == "info" && tokens.size() > 1) {
        system.listInfo(tokens[1]);
    }
    else if (command == "find" && tokens.size() > 1) {
        if (tokens[1] == "units" && tokens.size() >= 4) {
            // Parse find units command
            // Example: find units books title "Harry Potter" year desc 5
            std::string type = tokens.size() > 2 ? tokens[2] : "";
            std::string option = tokens.size() > 3 ? tokens[3] : "";
            std::string value = tokens.size() > 4 ? tokens[4] : "";
            std::string sortKey = tokens.size() > 5 ? tokens[5] : "";
            std::string sortOrder = tokens.size() > 6 ? tokens[6] : "";
            int topCount = tokens.size() > 7 ? std::stoi(tokens[7]) : -1;
            
            system.findUnits(type, option, value, sortKey, sortOrder, topCount);
        }
        else if (tokens[1] == "users" && tokens.size() >= 4) {
            // Parse find users command
            std::string option = tokens[2];
            std::string value = tokens[3];
            system.findReaders(option, value);
        }
        else {
            std::cout << "Invalid find command format.\n";
            std::cout << "Use 'find units [type] [option] [value] [sortKey] [sortOrder] [topCount]' or 'find users [option] [value]'\n";
        }
    }
    else if (command == "add") {
        if (tokens.size() > 1 && tokens[1] == "unit") {
            // Parse unit type
            if (tokens.size() > 2) {
                Type unitType;
                if (tokens[2] == "book") {
                    unitType = Type::BOOK;
                } else if (tokens[2] == "series") {
                    unitType = Type::SERIES;
                } else if (tokens[2] == "periodicals") {
                    unitType = Type::PERIODICALS;
                } else {
                    std::cout << "Invalid unit type. Must be 'book', 'series', or 'periodicals'\n";
                    return true;
                }
                system.addUnit(unitType);
            } else {
                std::cout << "Usage: add unit <type>\n";
                std::cout << "Where type is one of: book, series, periodicals\n";
            }
        }
        else if (tokens.size() >= 2 && tokens[1] == "user") {
            bool isAdmin = false;
            if (tokens.size() >= 3 && tokens[2] == "admin") {
                isAdmin = true;
            }
            system.addUser(isAdmin);
        }
        else {
            std::cout << "Usage: add <unit|user> ...\n";
        }
    }
    else if (command == "remove") {
        if (tokens.size() > 2 && tokens[1] == "unit") {
            try {
                unsigned int id = std::stoul(tokens[2]);
                system.removeUnit(id);
            }
            catch (std::exception& e) {
                std::cerr << "Invalid ID. Must be a number.\n";
            }
        }
        else if (tokens.size() > 2 && tokens[1] == "user") {
            system.removeUser(tokens[2]);
        }
        else {
            std::cout << "Usage: remove <unit|user> <id|username>\n";
        }
    }
    else if (command == "change") {
        if (tokens.size() > 2 && tokens[1] == "unit") {
            try {
                unsigned int id = std::stoul(tokens[2]);
                system.changeUnit(id);
            }
            catch (std::exception& e) {
                std::cerr << "Invalid ID. Must be a number.\n";
            }
        }
        else if (tokens.size() > 3 && tokens[1] == "user") {
            std::string targetUser = tokens.size() > 4 ? tokens[4] : "";
            system.changeUser(tokens[2], tokens[3], targetUser);
        }
        else {
            std::cout << "Usage: change unit <id> or change user <username> <newpassword> [targetuser]\n";
        }
    }
    else if (command == "take" && tokens.size() > 2) {
        try {
            unsigned int id = std::stoul(tokens[2]);
            system.take(tokens[1], id);
        }
        catch (std::exception& e) {
            std::cerr << "Invalid ID. Must be a number.\n";
        }
    }
    else if (command == "return" && tokens.size() > 1) {
        try {
            unsigned int id = std::stoul(tokens[1]);
            system.returnUnit(id);
        }
        catch (std::exception& e) {
            std::cerr << "Invalid ID. Must be a number.\n";
        }
    }
    else {
        std::cout << "Unknown command. Type 'help' for a list of commands.\n";
    }

    return true;
}

int main() {
    printMessage();

    try {
        LibrarySystem system("users.dat", "units.dat");
        bool continueRunning = true;
        std::string input;

        // Main command loop
        while (continueRunning) {
            std::cout << "\n> ";
            std::getline(std::cin, input);
            
            std::vector<std::string> tokens = tokenize(input);
            continueRunning = processCommand(tokens, system);
        }

        //// Save the system state before exiting
        //std::ofstream usersFile("users.dat", std::ios::binary);
        //std::ofstream unitsFile("units.dat", std::ios::binary);
        //
        //if (!usersFile.is_open() || !unitsFile.is_open()) {
        //    std::cerr << "Failed to open files for saving data.\n";
        //} else {
        //    system.serialize(usersFile, unitsFile);
        //}
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";

        return 1;
    }

    std::cout << "Thank you for using our Library System. Goodbye!\n";

    return 0;
}