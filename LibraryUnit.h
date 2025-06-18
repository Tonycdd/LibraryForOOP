#pragma once
#include <string>
#include "Date.h"
#include <vector>
// отделен клас, представящ датите, имам работа за подобрение да работи с 
// високосни години, там има и ограничение за годините - 1900 не приемаме по-стари от тази година
// идеята на този клас е да представя общ интерфейс за всички единици в нашата библиотека
// мисъл за бъдещо наследяване 
// полиморфизъм и абстрактен клас 

// създаваме си помощни именовани структури за оценката и жанра

enum Type {
	UNKNOWN = 1,
	BOOK = 2,
	PERIODICALS = 3,
	SERIES = 4,
};

enum Rate : uint8_t
{
	VeryPoor = 1,
	Poor = 2,
	Average = 3,
	Good = 4,
	VeryGood = 5,
	Excellent = 6
};

enum Genre : uint16_t
{
	// Книжни жанрове
	Fiction = 0,
	NonFiction,
	SciFi,
	Fantasy,
	Mystery,
	Thriller,
	Romance,
	Historical,

	// Академични
	Academic,
	Textbook,

	// Периодични издания
	News,
	Science,
	Sports,
	Technology,

	// Допълнителни
	Other
};

class LibraryUnit
{
public:
	
	LibraryUnit(const std::string& title, const std::string& publishers, const std::string& description,
		int day, int month,int year,const Rate& rate, const Genre& genre);
	LibraryUnit(const LibraryUnit& other);

	// чисто виртуален оператор=, задължаващ наследниците да го имплементират
	LibraryUnit& operator=(const LibraryUnit& other);
	
	virtual LibraryUnit* clone() const = 0; // чисто виртуален метод за копиране, 
	// неговите наследници го имплементират

	// муув семантика
	LibraryUnit(LibraryUnit&& other) noexcept;
	LibraryUnit& operator=(LibraryUnit&& other) noexcept;

	virtual ~LibraryUnit() noexcept = default; //  виртуален деструктор за правилно извикване 
	// при унищожение на наследници, когато работим с указатели към базовия клас

	// не е обвързан пряко с представител на класа затова е static
	static int generateNextNumber() {
		return ++LibraryUnit::uniqueNumber;
	}

	// сетъри
	bool setNewTitle(const std::string& newTitle);
	bool setNewPublishers(const std::string& newPub);
	bool setNewDescription(const std::string& desc);
	bool setNewDate(const Date& newDate);
	bool setNewRate(const Rate& newRate);
	bool setNewGenre(const Genre& newGenre);
	void setTotalCopies(unsigned newTotal);

	// гетъри
	inline const std::string& getTitle()const { return title; }; 
	inline const std::string& getPublishers()const { return publishers; }; 
	inline const std::string& getDescription()const { return description; }; 
	inline Date getDate()const { return date; };// копие - не нарушава капуслация, не е тежък обект
	inline Rate getRate()const { return rate; };// копие, не нарушава капсулацията
	inline static int getUniqueNumber() { return LibraryUnit::uniqueNumber; }; // може и без този гетър, тъй като е статик
	inline Genre getGenre()const { return genre; };// връщаме копие 
	inline int getId()const { return id; };
	inline bool isAvailable()const { return takenCopies < totalCopies; };
	inline unsigned int getAvailableCopies()const { return totalCopies - takenCopies; };
	inline unsigned int getTotalCopies()const { return totalCopies; };
	
	// вирутален display
	virtual void display(std::ostream& os = std::cout) const = 0; // може да има имплементация ;)
	// може да го направим да бъде operator<< - да използва тази функция

	bool equals(const LibraryUnit& other)const;
	// промяна на наличността спярмо взетите копия на текущата инстанция
	bool incrementTakenCopies();
	bool decrementTakenCopies();

	// това ще отговаря за живите обекти и така ще връщаме колко е броят на всеки от тях
	virtual size_t getCount() const = 0;
	virtual void serialize(std::ostream& os)const = 0;
	virtual void deserialize(std::istream& is) = 0;
	virtual Type getType()const = 0;

	// за връщане на isnn и isbn
	virtual std::vector<std::string> getIdentifiers()const = 0;

	void printBase(std::ostream& out=std::cout) const;

	virtual std::vector<std::string> getKeyWordsForFind()const = 0;

	// за change - диалогов вариант - е тази функция
	virtual bool change();

protected:
	
	// това тук малко чупи валидността на обектите, но ние го използваме само в изключителен случай, като по този
	// си гарантираме в момента валидността на обектите

	LibraryUnit();
protected:

	// за фабриката
	LibraryUnit(std::istream& is) {
		deserializeBaseUnit(is);
	}

	void serializeBaseUnit(std::ostream& os)const;
	void deserializeBaseUnit(std::istream& is);

protected:
	// в случая е ок а е протектед
	std::string title;// заглавие
	std::string publishers;// издателство
	std::string description;// кратко описание
	Date date;// дата на издаване
	Rate rate;// рейтинг - стойност до 5, тоест може да я поберем в един 8 битов беззнаков интинджър
	Genre genre; // стига ни 2 байтов беззнаков инт за ефикасно представяне
	static int uniqueNumber;// за генериране на уникални номера
	unsigned int totalCopies;
	unsigned int takenCopies;
	int id; // уникалният номер в библиотеката
};

// ползваме вече готова имплементация, не ни трябва да бъде friend, тъй като вече имаме публична функция
std::ostream& operator<<(std::ostream& os, const LibraryUnit& lib);
bool operator==(const LibraryUnit& lhs, const LibraryUnit& rhs);