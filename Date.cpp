#define _CRT_SECURE_NO_WARNINGS
#include "Date.h"
#include <ctime> 
#include <iostream> 
#include <stdexcept> 

const int days[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

Date::Date() : day(0),month(0),year(0)
{
	std::time_t t = std::time(nullptr);     // Взима текущото време в секунди
	std::tm* now = std::localtime(&t);      // Превръща го в локално време (година, месец и т.н.)

	int day = now->tm_mday;
	int month = now->tm_mon + 1;            // tm_mon е между 0 и 11 => +1
	int year = now->tm_year + 1900;         // tm_year е броя години след 1900
	this->day = day;
	this->month = month;
	this->year = year;
}

Date::Date(int day, int month, int year)
	:Date()
{
	// трябва да решим въпроса с високосните години
	// валидираме месеца спрямо деня
	if (day < 0 || !validateDay(day, month)) {
		throw std::invalid_argument("Invalid day!");
	}
	else if (month < 0 || month > 12)
	{
		throw std::invalid_argument("Invalid month!");
	}
	else if (year < 1900) {
		throw std::invalid_argument("Invalid date!");
	}

	// иначе данните са ок
	// тук можем да ползваме и сетДате
	this->day = day;
	this->month = month;
	this->year = year;
}

void Date::setDate(int day, int month, int year) {
	
	// същите валидации
	if (day < 0 || !validateDay(day, month) || year < 1900) {
		throw std::invalid_argument("Invalid arguments!");
	}
	// ок е го сетваме
	this->day = day;
	this->month = month;
	this->year = year;
}

void Date::print(std::ostream& out) const
{
	out << "Date\n";
	out << (this->day < 10 ? "0" : "") << this->day << "\\" << (this->month < 10 ? "0" : "") << this->month << "\\" << this->year << "\n";
}

const char* Date::dayOfWeek() const
{
	//  Това нещо го видях от интернет как работи
	// затова ми трябваше библиотекта ctime
	tm time = {}; 
	time.tm_year = this->year - 1900;
	time.tm_mon = this->month - 1;
	time.tm_mday = this->day;

	mktime(&time);
	const char* days[] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
	return days[time.tm_wday];
}

int Date::differenceBetweenDates(const Date& other) const
{
	int totalDaysCurrent = this->getTotalDays();
	int totalDaysOther = other.getTotalDays();

	int final = totalDaysCurrent - totalDaysOther;
	return final;

}

int Date::operator==(const Date& other) const
{
	// първо годините, месеците, най-накрая са дните
	// стандартно сравнение 

	if (this->year != other.year)
	{
		if (this->year > other.year) { return 1;};
		return -1;
	}
	else if (this->month != other.month) {
		if (this->month > other.month) { return 1; };
		return -1;
	}
	else if (this->day != other.day) {
		if (this->day > other.day) { return 1; };
		return -1;
	}
	return 0; 
}

bool Date::validateDay(int day, int month) {
	switch (month)
	{
	case 1:
		return day <= 31;
	case 2:
		return day <= 28;
	case 3:
		return day <= 31;
	case 4:
		return day <= 30;
	case 5:
		return day <= 31;
	case 6:
		return day <= 30;
	case 7:
		return day <= 31;
	case 8:
		return day <= 31;
	case 9:
		return day <= 30;
	case 10:
		return day <= 31;
	case 11:
		return day <= 30;
	case 12:
		return day <= 31;
	default:
		return false;
	}
}

long long Date::getTotalDays() const
{
	int days = this->day;

	// добавяме месеците 
	// -1 тъй като текущия ден е от някой месец и затова него не го отчитаме пак
	for (int i = 0; i < this->month - 1; i++)
	{
		// :: okazva globalen skope
		days += ::days[i];
	}

	// добавяме от годините
	// не смятаме високосни години
	days += this->year * 365;
	return days;
}

void Date::serialize(std::ostream& os) const
{
	if (!os) {
		throw std::ios_base::failure("Error before writing data!");
	}
	os.write(reinterpret_cast<const char*>(&day), sizeof(day));
	if (os.fail()) {
		throw std::ios_base::failure("Error with writing day!");
	}
	
	os.write(reinterpret_cast<const char*>(&month), sizeof(month));
	if (os.fail()) {
		throw std::ios_base::failure("Error with writing month!");
	}

	os.write(reinterpret_cast<const char*>(&year), sizeof(year));
	if (os.fail()) {
		throw std::ios_base::failure("Error with writing year!");
	}
}

void Date::deserialize(std::istream& is) {
	if (!is) throw std::ios_base::failure("Invalid stream before deserializing data!");

	is.read(reinterpret_cast<char*>(&day), sizeof(day));
	if (is.fail()) {
		throw std::ios_base::failure("Error with reading day!");
	}
	is.read(reinterpret_cast<char*>(&month), sizeof(month));
	if (is.fail()) {
		throw std::ios_base::failure("Error with reading month!");
	}
	is.read(reinterpret_cast<char*>(&year), sizeof(year));
	if (is.fail()) {
		throw std::ios_base::failure("Error with reading year!");
	}
}

void Date::addDays(int daysToAdd) {
	// интернет е взето изцяло!
	if (daysToAdd < 0)
		throw std::invalid_argument("Cannot add negative number of days.");

	std::tm timeStruct = {};
	timeStruct.tm_mday = this->day;
	timeStruct.tm_mon = this->month - 1; // std::tm uses months [0, 11]
	timeStruct.tm_year = this->year - 1900; // std::tm uses years since 1900

	time_t rawTime = mktime(&timeStruct); // преобразуваме в time_t
	if (rawTime == -1)
		throw std::runtime_error("Invalid date during conversion.");

	rawTime += daysToAdd * 24 * 60 * 60; // добавяме дни в секунди

	std::tm* newTime = localtime(&rawTime); // обратно към структура

	this->day = newTime->tm_mday;
	this->month = newTime->tm_mon + 1;
	this->year = newTime->tm_year + 1900;
}