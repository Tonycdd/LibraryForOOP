#pragma once
#include <string>
#include <exception>
#include <stdexcept>
#include "Date.h"
#include <iostream>
#include "LibraryUnit.h"
// добавяме един базов клас, който отново подобно на единиците, искаме да бъде абстрактен
// той само задава поведението и няма да бъде инстанциран
// в бъдеще е по-лесно добавяне на нови роли за хора в библиотеката, като единствено ще пипаме 
// малки части тук

enum TypeOfReader {
	UNKOWN = 1,
	READER = 2,
	ADMINISTRATOR = 3
};

class LibraryPerson
{
public:
	// голяма 6-тица
	LibraryPerson(const std::string& name, const std::string& password, int day1, int month1, int year1, int day2, int month2, int year2);
	LibraryPerson(const LibraryPerson& other);
	LibraryPerson& operator=(const LibraryPerson& other);
	LibraryPerson(LibraryPerson&& other) noexcept;
	LibraryPerson& operator=(LibraryPerson&& other) noexcept;
	virtual ~LibraryPerson() noexcept;
	
	inline const std::string& getUsername()const { return username; }; 
	// не е добра идея да даваме достъп до паролата, така че не предоставяме гетър
	
	inline Date getRegisterDate()const { return registerDate; }; // по копие, не е тежко
	inline Date getLastLoginDate()const { return lastLoginDate; }; // по копие, не е тежко

	//setters
	bool setPassword(const std::string& newPas);
	bool setNewUsername(const std::string& newUsername);

	virtual LibraryPerson* clone() const = 0 ; // чисто виртуална функция

	// две функции, които просто валидират името и паролата
	// тук те просто гледат те да са с правилен формат
	// в самата система ще проверяваме за дублирания на имена, тук не

	static bool validatePassword(const std::string& pass) noexcept;
	static bool validateUsername(const std::string& user) noexcept;

	virtual void display(std::ostream& out = std::cout)const = 0; 
	
	// двоични файлове
	virtual void serialize(std::ostream& out)const = 0; 
	virtual void deserialize(std::istream& is) = 0;
	virtual TypeOfReader getType() const = 0;
	
	virtual void borrow(LibraryUnit* unit) = 0;
	virtual bool returnUnit(int id) = 0;

protected:
	LibraryPerson();
protected:
	// за фабриката 
	LibraryPerson(std::istream& is) {
		deserializeBase(is);
	}

	void deserializeBase(std::istream& is);
	void serializeBase(std::ostream& out)const;

	// protected e!
	inline const std::string& getPassword()const { return password; };

	friend class LibrarySystem; // трябва ни на едно място за серилизацията в системата

protected:
	std::string username; // потребителско име
	std::string password; // парола, ще искаме да има някакви изисквания за сигурност 
	Date registerDate; // дата на регистрация
	Date lastLoginDate; // последно влизане
};

std::ostream& operator<<(std::ostream& out, const LibraryPerson& person);