#include <iostream>
#include "LibrarySystem.h"

void printHelpMessage() {
	std::cout << "(add unit) - only admin!\n";
	std::cout << "(add user) - only admin!\n";
	std::cout << "(remove unit) - only admin!\n";
	std::cout << "(remove user) - only admin!\n";
	std::cout << "(change unit) - only admin!\n";
	std::cout << "(find users) - only admin!\n";
}

// тук като подобрение можем да сложим изискванията на всяка от фукнциите - но това е по-скоро козметична забележка!
void printMessage() {
	
	std::cout << "Welcome to our LibrarySystem!\n";
	std::cout << "\n";
	std::cout << "Here is a list of commands, that you can perform, based on different rights (some commands are only for admins) :\n";
	std::cout << "1. (help) - different rights\n";
	std::cout << "2. (login) - after use you should input valid username and password for the system! \n";
	std::cout << "3. (logout) - current logged user is logging out from the system (BUT NOT REMOVED!)!\n";
	std::cout << "4. (exit) - end of the program!\n";
	std::cout << "5. (books all) - pritns info about books in our system!\n";
	std::cout << "6. (periodicals all) - prints info about periodicals in our system!\n";
	std::cout << "7. (series all) - prints info about series in our system!\n";
	std::cout << "8. (all) - prints info about all of the units in our system!\n";
	std::cout << "9. (info)  with ISBN/ISSN number - prints detailed info about unit with current ISBN/ISSN!\n";
	std::cout << "10. (find units) - it searches a specific unit in our system and prints it out!\n";
	std::cout << "11. (find users) - it searches a specific user in our system and prints it out!\n";
	std::cout << "12. (find units) - it searches a specific unit in our system!\n";
	std::cout << "13. (add unit) - interactively  creates and adds it to our system!\n";
	std::cout << "14. (add user) - interactively  creates and adds it to our system!\n";
	std::cout << "15. (remove unit) with id - removes that unit from the system!\n";
	std::cout << "16. (remove user) with id - removes that user from the system!\n";
	std::cout << "17. (change unit) with id - changes interactively that unit !\n";
	std::cout << "18. (change user) - can change his password!\n";
	std::cout << "\n";
}

int main() {
	
	printMessage();

	try
	{
		LibrarySystem system("users.dat", "units.dat");
		
		LibraryUnit* v1 = new Series("Title", "Publishers", "Description", 1, 1, 2000, Rate::Excellent, Genre::Academic,"Author", 3);
		LibraryUnit* v2 = new Periodicals("Title", "Publishers", "Description", Genre::Academic, Rate::Average,1,2003, 3,"ISN341");
		LibraryUnit* v3 = new Book("BOok", "Book", "Book", 1, 1, 2000, Rate::Excellent, Genre::Academic,"Tony","ISB@E!123123");
		v1->display();
		v2->display();
		v3->display();


		delete v2;
		delete v3;
		delete v1;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}