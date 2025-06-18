#pragma once
#include"Administrator.h"
#include "Book.h"
#include"Periodicals.h"
#include "Reader.h"
#include"Series.h"
#include <fstream> // двоични файлове ще работим с тях
#include "Factory.h"

// това е структура, която ще пази всичките налични книги и техните позиции във файла съответно
// както и дали са свободни в този момент! 

struct UniqueIDAndFilePositions
{
	unsigned int uniqueNumber;
	size_t pos;
	bool isFree;

	UniqueIDAndFilePositions() = default;
	// не правим никакви валидации тук, тъй като това не е работа за момента
	// това ще стане чак когато се опитаме да създадем обекта, той ще се погрижи за тази работа
	UniqueIDAndFilePositions(unsigned int q, size_t pos, bool is) : uniqueNumber(q), pos(pos), isFree(is) {};

};

// структура, която пази информация за името,паролата на даден юзър, както и неговата позиция във двоичния файл

struct MetaInfoAboutUsers {
	std::string username;
	std::string password;
	size_t pos;

	MetaInfoAboutUsers() = default;
	// не правим никакви валидации тук, тъй като това не е работа за момента
	// това ще стане чак когато се опитаме да създадем обекта, той ще се погрижи за тази работа
	MetaInfoAboutUsers(const std::string& user, const std::string& pass, size_t pos) : username(user), password(pass), pos(pos) {};
};

class LibrarySystem
{
public:
	// засега не ни трябва
	LibrarySystem() = delete;

	LibrarySystem(const std::string& file1,const std::string& file2);
	LibrarySystem(const LibrarySystem& other) = delete;
	LibrarySystem& operator= (const LibrarySystem& other) = delete;
	LibrarySystem(LibrarySystem&& other)noexcept;
	LibrarySystem& operator=(LibrarySystem&& other)noexcept;

	~LibrarySystem() noexcept;

	// гетъри за полетата
	inline const std::vector<LibraryUnit*>& getUnits()const { return units; };
	inline const std::vector<LibraryPerson*>& getUsers()const { return users; };
	
	// методи за системата!
	bool login(const std::string& username, const std::string& password); // добавя нов юзър
	bool logout();

	bool exit(); // to implement!

	//  методи за извеждане на информация за книгите
	// използват  printBase part само, а вече допълнитените данни, само ако се изполва list info с  ISNN or ISBN

	void booksAll(std::ostream& out = std::cout) const;
	void periodicalsAll(std::ostream& out = std::cout) const;
	void seriesAll(std::ostream& out = std::cout)const;
	void all(std::ostream& out = std::cout)const;
	// тук вече използваме display - спрямо конкретния тип
	void listInfo(const std::string& ISNN_OR_ISBN) const;

	// методи за търесете
	// търсене на издания
	void findUnits(const std::string& type,
		const std::string& option,
		const std::string& value,
		const std::string& sortKey,  // празен низ ако няма сортиране
		const std::string& sortOrder, // "asc", "desc", или празен
		int topCount)const;  // -1 ако няма top
	// търсене на потребители
	void findReaders(const std::string& option, const std::string& value);

	// методи за писане и четене
	bool serialize(std::ostream& file1,std::ostream& file2)const;
	bool deserialize(std::istream& file1 ,std::istream& file2); // ок е да е по-бавно в началото на системата, но се извършва веднъж

	// методи за промяна
	/*
	Когато потребителят въведе books add, програмата влиза в интерактивен режим:
	Въвежда се стъпка по стъпка всяко поле.
	След всяка стъпка се проверява дали потребителят е въвел cancel, за да прекрати операцията.
	Ако всички данни са коректни → обектът се създава и добавя към системата.
	*/
	void addUnit(const Type& type);
	void removeUnit(unsigned int id);
	bool changeUnit(unsigned int id);

	// за потребители
	// не го добавяме още в системата, а само информация за него, чак при логин ще го добавим целия
	void addUser(const std::string& name, const std::string& password, bool isAdmin = false); 
	void removeUser(const std::string& name);
	bool changeUser(const std::string&,const std::string&, const std::string&);

	bool userFind(const std::string& option, const std::string& optionString)const;

	// take and return 
	bool take(const std::string& user, unsigned int id);
	bool returnUnit(unsigned int id);

private:
	// това са полиморфични контейнери, които позволяват да работим вече със самите обекти
	// тоест това са вече заредените обекти 

	std::vector<LibraryUnit*> units;
	std::vector<LibraryPerson*> users;
	
	// отделно е посочено, че не зареждаме всичките данни, но можем да пазим по някакъв начин техните позиции
	std::vector<UniqueIDAndFilePositions> infoUnits;
	std::vector<MetaInfoAboutUsers> infoUsers;

	LibraryPerson* currentPerson = nullptr;

	bool canAddNewUser(const std::string& name, const std::string& password)const;
	LibraryPerson* findUserByUsername(const std::string&)const;
	LibraryUnit* findUnitById(unsigned int id)const;
	// по-добре е да пазим само имената, а не самите файлове!
	std::string fileUsers;
	std::string fileUnits;
};