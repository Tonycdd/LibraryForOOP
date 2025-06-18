#pragma once
#include <stdexcept>
#include <iostream>
#include <ctime>

class  Date
{
public:
	// голяма 4-ка -  не ни трябва в случая, но нека посочим, че ползваме стандратна 
	// реализация от компилатора

	Date();
	Date(int day, int month, int year);
	Date(const Date& other) = default;
	Date& operator=(const Date& other) = default;
	~Date() = default;

	// гетъри 
	inline unsigned int getDay()const {
		return this->day;
	}

	inline unsigned int getMonth()const {
		return this->month;
	}

	inline unsigned int getYear()const {
		return this->year;
	}

	// сетъри 
	void setDate(int, int, int);


	// методи
	void print(std::ostream& out = std::cout)const;
	const char* dayOfWeek()const;
	int differenceBetweenDates(const Date& other)const;
	int operator==(const Date& other) const;
	long long getTotalDays()const;
	
	bool operator<(const Date& other) const {
		if (year != other.year)
			return year < other.year;
		if (month != other.month)
			return month < other.month;
		return day < other.day;
	}
	
	bool operator>(const Date& rhs)const {
		if (year != rhs.getYear())
			return year > rhs.getYear();
		if (month != rhs.getMonth())
			return month> rhs.getMonth();
		return day > rhs.getDay();
	}
	
	void serialize(std::ostream& os)const;
	void deserialize(std::istream& is);
	bool validateDay(int, int);
	void addDays(int daysToAdd);
private:
	unsigned int day;
	unsigned int month;
	unsigned int year;
};



