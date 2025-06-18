#pragma once
#include "LibraryPerson.h"
#include "Book.h"
#include "Periodicals.h"
#include "Series.h"

// една допълнителна структура, която да пази указател от базовия клас  и да помни кога е заета и 
// върната книгата
struct LoanInfo {
	// когато unit умира, трябва да се занули, за да няма висящи указатели
	// не се грижи за unit, а само пази указател към него
	const LibraryUnit* unit; // указател към единица от библиотеката
	Date borrowDate; // дата на взимане
	Date returnDate; // дата на връщането
	bool returned; // дали е върната била
	
	LoanInfo() = default;
	LoanInfo(const LibraryUnit* unit, const Date& borrow, const Date& due,bool isReturned = false) :
		unit(unit), borrowDate(borrow), returnDate(due), returned(false) {};

	inline void changeReturned() {
		returned = !returned; // обръща състоянието
	}

	void serialize(std::ostream& out)const {
		// unit  вече е сериализиран, така че не го пишем отново!!!
		borrowDate.serialize(out);
		if (!out.good()) {
			throw std::ios_base::failure("Error with writing borrowDate in LoanInfo!");
		}
		returnDate.serialize(out);
		if (!out.good()) {
			throw std::ios_base::failure("Error with writing returnDate in LoanInfo!");
		}
		out.write(reinterpret_cast<const char*>(&returned), sizeof(returned));
		if (!out.good()) {
			throw std::ios_base::failure("Error with writing returned state in LoanInfo!");
		}
	}

	void deserialize(std::istream& is) {
		if (!is.good()) {
			throw std::invalid_argument("Error with stream before reading in LoanInfo!");
		}
		borrowDate.deserialize(is);
		if (!is.good()) {
			throw std::ios_base::failure("Error with reading borrowDate in LoanInfo!");
		}
		returnDate.deserialize(is);
		if (!is.good()) {
			throw std::ios_base::failure("Error with reading returnDate in LoanInfo!");
		}
		is.read(reinterpret_cast<char*>(&returned), sizeof(returned));
		if (!is.good()) {
			throw std::ios_base::failure("Error with reading returned state in LoanInfo!");
		}
	}

};

class Reader: public LibraryPerson
{
public:
	// аналогично правим голяма 6-тица
	// 
	//за фабриката
	Reader(std::istream& is):LibraryPerson(is)
	{
		deserializeReaderUnit(is);
	}

	Reader(const std::string& name, const std::string& pass, int day1, int month1, int year1,
		int day2, int month2, int year2, const std::vector<LibraryUnit*>& v = {});
	Reader(const Reader& other);
	Reader& operator=(const Reader& other);
	Reader(Reader&& other)noexcept;
	Reader& operator=(Reader&& other)noexcept;
	virtual ~Reader() noexcept override;

	// имплементираме си вируталните функции
	virtual LibraryPerson* clone()const override;
	virtual void display(std::ostream& out = std::cout)const override;

	// гетъри
	inline const std::vector<LibraryUnit*>& getUnits()const { return units; };
	inline std::vector<LibraryUnit*>& getUnits() { return units; }; // трябва ми за промяната в системата, не е добра практика обаче
	inline const std::vector<LoanInfo>& getHistory()const { return history; };

	// свързани с логиката за добавяне на нови единици и тяхното взимане и връщане
	void addNewUnit(LibraryUnit* unit);
	virtual void borrow(LibraryUnit* unitToBorrow)override;
	virtual bool returnUnit(int id)override;

	// сериализация, десериализация
	virtual void serialize(std::ostream& out)const override;
	virtual void deserialize(std::istream& is)override;
	inline virtual TypeOfReader getType()const override { return TypeOfReader::READER; };

	// специални методи за търесне само за потребителите, които са читатели
	bool hasOverdueBooks(const Date& today)const;
	unsigned int borrowedLastMonth(const Date& today)const;
	int monthsSinceLastBorrow(const Date& today)const;
	bool hasBorrowedUnit(unsigned int unitId)const ; // търси в историята - тези, които са били заети някога от този потребител

	// статичен метод за интерактивен режим за фабриката
	static Reader* createInteractively();

protected:
	Reader();

protected:
	// важно за наследници
	// засега не са от полза
	void serializeReaderUnit(std::ostream& out)const;
	void deserializeReaderUnit(std::istream& is);

private:
	std::vector<LibraryUnit*> units; // полиморфичен контейнър, пазещ какви единици е взел този читател
	std::vector<LoanInfo> history; // всеки от units ще бъде свързан с history
};

std::ostream& operator<<(std::ostream& out, const Reader& other);
