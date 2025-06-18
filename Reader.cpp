#include "Reader.h"
#include "Factory.h"

Reader::Reader(const std::string& name,
    const std::string& pass,
    int day1, int month1, int year1,
    int day2, int month2, int year2,
    const std::vector<LibraryUnit*>& v)
    :LibraryPerson(name, pass, day1, month1, year1, day2, month2, year2)
{
    units = v;
}

Reader::Reader(const Reader& other)
    :LibraryPerson(other), units(other.units), // (shallow copy)
    history(other.history) 
{

}

Reader& Reader::operator=(const Reader& other)
{
    // copy and swap
    if (this != &other) {
        Reader cpy(other);
        *this = std::move(cpy); // move operator= 
    }
    return *this;
}

Reader::Reader(Reader&& other) noexcept
    :LibraryPerson(std::move(other)), units(std::move(other.units))
{
   // няма какво повече да се прави тук
}

Reader& Reader::operator=(Reader&& other) noexcept
{
    if (this != &other) {
        LibraryPerson::operator=(std::move(other));
        units = std::move(other.units);

    }
    return *this;
}

Reader::~Reader() noexcept
{
    // не притежава ресурси
    units.clear();
    history.clear();
}

LibraryPerson* Reader::clone() const
{
    return new Reader(*this);
}

void Reader::display(std::ostream& out) const
{
    LibraryPerson::display(out);
    for (size_t i = 0; i < units.size(); i++)
    {
        units[i]->display(out);
    }
}

void Reader::addNewUnit(LibraryUnit* unit)
{
    if (!unit) {
        std::cout << "Unit is nullptr!" << "\n";
        return; // не добавяме
    }

    for (size_t i = 0; i < units.size(); i++)
    {
        if (*(units[i]) == *unit) {
            std::cout << "This unit was already added!";
            return;
        }
    }
    units.push_back(unit);
}

void Reader::serialize(std::ostream& out) const
{
    TypeOfReader t = getType(); // READER
    out.write(reinterpret_cast<const char*>(&t), sizeof(t));
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing type {READER}!");
    }

    LibraryPerson::serializeBase(out);
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing base part of reader!");
    }
    Reader::serializeReaderUnit(out);
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing Reader part!");
    }
}

void Reader::deserialize(std::istream& is)
{
    // аналогично искаме strong excpetion

    if (!is.good()) {
        throw std::invalid_argument("Invalid stream before reading in Reader!");
    }
    Reader r;
    r.deserializeBase(is);
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading base part of Reader!");
    }
    r.deserializeReaderUnit(is);
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading Reader part!");
    }
    *this = std::move(r);
}

bool Reader::hasOverdueBooks(const Date& today) const {
    for (size_t i = 0; i < history.size(); i++) {
        if (!history[i].returned && history[i].returnDate < today) {
            return true;
        }
    }
    return false;
}

unsigned int Reader::borrowedLastMonth(const Date& today) const
{
    unsigned int count = 0;

    int currentMonth = today.getMonth();
    int currentYear = today.getYear();

    int lastMonth, yearOfLastMonth;

    if (currentMonth == 1) {
        lastMonth = 12;
        yearOfLastMonth = currentYear - 1;
    }
    else {
        lastMonth = currentMonth - 1;
        yearOfLastMonth = currentYear;
    }

    for (size_t i = 0; i < history.size(); i++)
    {
        int borrowMonth = history[i].borrowDate.getMonth();
        int borrowYear = history[i].borrowDate.getYear();

        if (borrowMonth == lastMonth && borrowYear == yearOfLastMonth) {
            count++;
        }
    }

    return count;
}

int Reader::monthsSinceLastBorrow(const Date& today) const {
    if (history.empty()) {
        return -1; // няма заемания
    }

    Date lastBorrowDate = history[0].borrowDate;
    for (size_t i = 0; i < history.size(); i++) {
        if (history[i].borrowDate > lastBorrowDate)
        {
            lastBorrowDate = history[i].borrowDate;
        }
    }

    int yearDiff = today.getYear() - lastBorrowDate.getYear();
    int monthDiff = today.getMonth() - lastBorrowDate.getMonth();

    return yearDiff * 12 + monthDiff;
}


bool Reader::hasBorrowedUnit(unsigned int unitId)const
{
    for (size_t i = 0; i < history.size(); i++)
    {
        if (history[i].unit && history[i].unit->getId() == unitId) {
            return true;
        }
    }
    return false;
}

Reader* Reader::createInteractively()
{
    std::string name, password;
    int d1, m1, y1, d2, m2, y2;

    std::cout << "Enter username (or 'cancel' to abort): ";
    std::getline(std::cin, name);
    if (name == "cancel") return nullptr;

    std::cout << "Enter password (or 'cancel' to abort): ";
    std::getline(std::cin, password);
    if (password == "cancel") return nullptr;

    std::cout << "Enter registration date (day month year): ";
    if (!(std::cin >> d1 >> m1 >> y1)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cerr << "Invalid registration date input. Aborting.\n";
        return nullptr;
    }

    std::cout << "Enter last borrow date (day month year): ";
    if (!(std::cin >> d2 >> m2 >> y2)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cerr << "Invalid borrow date input. Aborting.\n";
        return nullptr;
    }
    std::cin.ignore(); 

    return new Reader(name, password, d1, m1, y1, d2, m2, y2);
}

void Reader::borrow(LibraryUnit* unit) {
    if (!unit) {
        throw std::invalid_argument("Null pointer passed to borrow!.");
    }

    if (!unit->isAvailable()) {
        throw std::runtime_error("No available copies for this unit.");
    }

    // Увеличаваме броя на заетите копия
    unit->incrementTakenCopies(); 

    // Добавяме в текущите заети единици
    units.push_back(unit);

    // Взимаме текущата дата
    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);
    Date borrowDate(localTime.tm_mday, localTime.tm_mon + 1, localTime.tm_year + 1900);

    // Дата на връщане след 14 дни
    Date returnDate = borrowDate;
    returnDate.addDays(14);

    // Добавяме в историята
    history.emplace_back(unit, borrowDate, returnDate,false); // създава обекта
}

bool Reader::returnUnit(int id) {
    bool found = false;

    for (size_t i = 0; i < units.size(); ++i) {
        if (units[i]->getId() == id) {

            // търсим съответния запис в history
            for (size_t j = 0; j < history.size(); j++) {
                if (history[j].unit->getId() == id && !history[j].returned) {
                    history[j].returned = true; // маркираме като върната и свободна
                    break;
                }
            }

            if (!units[i]->decrementTakenCopies()) {
                std::cerr << "Something unexpeccted happend!\n";
                return false;
            };

            // премахваме от списъка с активни заеми
            std::swap(units[i], units.back());
            units.pop_back(); // не вика dtor, не притежаваме самите книги, а само ги ползваме

            found = true;
            break;
        }
    }
    return found;
}

Reader::Reader() :LibraryPerson(), units(), history()
{
}

void Reader::serializeReaderUnit(std::ostream& out) const
{
    // сериализираме само неговите части 
    
    // units -> които държи
    size_t size = units.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing size of units in ReaderUnit!");
    }
    size_t sizeOfUnits = size;
    for (size_t i = 0; i < sizeOfUnits; i++)
    {
        units[i]->serialize(out); // полиморфично поведение, трябва да се свърже правилно
    }

    // history
    size = history.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing size of history in ReaderUnit!");
    }
    for (size_t i = 0; i < size; i++)
    {
        history[i].serialize(out);
    }
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing history in ReaderUnit!");
    }
}

void Reader::deserializeReaderUnit(std::istream& is)
{
    if (!is.good()) {
        throw std::invalid_argument("Error with stream before reading in ReaderUnit");
    }

    // units
    size_t size;
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading size of units in ReaderUnit!");
    }


    // временно хранилище за units
    std::vector<LibraryUnit*> tempUnits;
    tempUnits.reserve(size);
    std::vector<LoanInfo> tempHistory;

    try
    {

        // тук трябва да знаем типа, за да създадем такъв обект
        // концепцията за Фабрика идва на среща

        // ние няма как да знаем какъв тип четем, преди да сме го достъпили

        for (size_t i = 0; i < size; ++i) {
            Type type;
            is.read(reinterpret_cast<char*>(&type), sizeof(type));
            if (!is.good()) {
                throw std::ios_base::failure("Error with file!");
            }
            LibraryUnit* curr = LibraryFactory::createUnitFromStream(is,type); // може да хвърли
            tempUnits.push_back(curr);
        }


        // history
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        if (!is.good()) throw std::ios_base::failure("Failed reading history size!");

        size_t historySize = size;
        if (historySize != tempUnits.size()) {
            throw std::invalid_argument("Missmatch between history and units!");
        }
        tempHistory.reserve(size);
        for (size_t i = 0; i < historySize; ++i) {
            LoanInfo li(tempUnits[i], Date(), Date()); // две временни, които ще се презапишат
            li.deserialize(is);
            if (!is.good()) throw std::ios_base::failure("Failed reading LoanInfo!");
            tempHistory.push_back(li);
        }
    }
    catch (...)
    {
        for (size_t i = 0; i < tempUnits.size(); i++)
        {
            delete tempUnits[i];
        }
        throw;
    }

    // вс е било успешно
    // изчистваме старите
    for (size_t i = 0; i < units.size(); i++)
    {
        delete units[i];
    }

    units = std::move(tempUnits);
    history = std::move(tempHistory);
}

std::ostream& operator<<(std::ostream& out, const Reader& other)
{
    other.display(out);
    return out;
}
