#pragma once
#include "Book.h"
#include "Periodicals.h"

// тук трябва да разбера първо малко повече за множественото наследяване и public virutal, 
// което сложих в Book и Periodicals


class Series:public Book,public Periodicals
{
public:

	// за фабриката
	Series(std::istream& is) : LibraryUnit(is), Book(is), Periodicals(is) {};

	// това е един от минусите, параметризираните ctor стават много дълги 
	// тъй като трябва да получант информация, с която да инициализират всичките си същности - в случая Book, Periodicals
	Series(const std::string& title,
		const std::string& publishers,
		const std::string& description,
		int day, int month, int year,
		const Rate& rate,
		const Genre& genre,
		const std::string& author,
		unsigned int issueNumber,
		const std::string& isbn = "",
		const std::vector<std::string>& v = {},
		const std::string& issn = "",
		const std::vector<Article>& articles = {})

		:LibraryUnit(title, publishers, description, day, month, year, rate, genre),
		Book(title, publishers, description, day, month, year, rate, genre, author, isbn, v),
		Periodicals(title, publishers, description, genre, rate, month, year, issueNumber, issn, articles)
	{
		Series::counter++;
	}

	Series(const Series& other) : LibraryUnit(other), Book(other), Periodicals(other) 
	{
		Series::counter++;
	}; // винаги първо вирт базов клас се вика
	
	Series& operator=(const Series& other) = default; //  дефолтно поведение
	
	Series(Series&& other)noexcept : LibraryUnit(std::move(other)),
		Book(std::move(other)), Periodicals(std::move(other)) 
	{
		Series::counter++;
	}; // викаме винаги първо виртуалния базов клас
	Series& operator=(Series&& other)noexcept = default; // дефолтно поведение
	
	virtual ~Series() noexcept override {
		if (Series::counter > 0) {
			Series::counter--;
		}
	}; //  серията не притежава свои ресурси, за които да се грижи
		
	//важно да ги overridnem
	inline virtual LibraryUnit* clone()const override {
		return new Series(*this);
	}

	virtual void display(std::ostream& os = std::cout)const override {
		LibraryUnit::printBase(os);
		Book::printBook(os);
		Periodicals::printPeriodicals(os);
	}
	
	inline virtual size_t getCount()const override { return counter; }
	inline static size_t getSeriesCount() { return counter; };
	virtual void serialize(std::ostream& os)const override;
	inline virtual Type getType()const override { return Type::SERIES; };
	virtual void deserialize(std::istream& is)override;
	inline virtual std::vector<std::string> getIdentifiers()const override {
		return { Book::getISBN(), Periodicals::getISSN() };
	}
	
	virtual std::vector<std::string> getKeyWordsForFind()const override { 
		

		std::vector<std::string> books;
		std::vector<std::string> periodicals;

		books = Book::getKeyWordsForFind();
		periodicals = Periodicals::getKeyWordsForFind();
		
		try
		{
			std::vector<std::string>a(books);
			for (size_t i = 0; i < periodicals.size(); i++)
			{
				a.push_back(periodicals[i]);
			}
			return a;
		}

		catch (...)
		{
			throw;
		}
	};

	static Series* createInteractively();

	virtual bool change()override;

protected:
	Series();
protected:
	// мислим за бъдещи наследници тук
	void deserializeSeriesUnit(std::istream& is);
	void serializeSeriesUnit(std::ostream& os)const;

	void printSeries(std::ostream& os = std::cout)const;
private:
	// нямаме нищо
	static size_t counter;
};

