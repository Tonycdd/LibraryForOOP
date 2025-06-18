#include "LibraryUnit.h"
#include <iostream>
#include <stdexcept>
#include <exception>

std::string toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result) c = std::tolower(c);
    return result;
}

Genre parseGenreFromString(const std::string& input) {
    std::string s = toLower(input);

    if (s == "fiction") return Genre::Fiction;
    if (s == "nonfiction") return Genre::NonFiction;
    if (s == "sci-fi" || s == "scifi") return Genre::SciFi;
    if (s == "fantasy") return Genre::Fantasy;
    if (s == "mystery") return Genre::Mystery;
    if (s == "thriller") return Genre::Thriller;
    if (s == "romance") return Genre::Romance;
    if (s == "historical") return Genre::Historical;
    if (s == "academic") return Genre::Academic;
    if (s == "textbook") return Genre::Textbook;
    if (s == "news") return Genre::News;
    if (s == "science") return Genre::Science;
    if (s == "sports") return Genre::Sports;
    if (s == "technology") return Genre::Technology;
    if (s == "other") return Genre::Other;

    throw std::invalid_argument("Unknown genre: " + input);
}

std::string genreToString(Genre genre) {
    switch (genre) {
    case Genre::Fiction: return "Fiction";
    case Genre::NonFiction: return "NonFiction";
    case Genre::SciFi: return "SciFi";
    case Genre::Fantasy: return "Fantasy";
    case Genre::Mystery: return "Mystery";
    case Genre::Thriller: return "Thriller";
    case Genre::Romance: return "Romance";
    case Genre::Historical: return "Historical";
    case Genre::Academic: return "Academic";
    case Genre::Textbook: return "Textbook";
    case Genre::News: return "News";
    case Genre::Science: return "Science";
    case Genre::Sports: return "Sports";
    case Genre::Technology: return "Technology";
    case Genre::Other: return "Other";
    default: return "Unknown";
    }
}

Rate parseRateFromString(const std::string& input) {
    std::string s = toLower(input);

    if (s == "verypoor" || s == "1") return Rate::VeryPoor;
    if (s == "poor" || s == "2") return Rate::Poor;
    if (s == "average" || s == "3") return Rate::Average;
    if (s == "good" || s == "4") return Rate::Good;
    if (s == "verygood" || s == "5") return Rate::VeryGood;
    if (s == "excellent" || s == "6") return Rate::Excellent;

    throw std::invalid_argument("Unknown rate: " + input);
}

std::string rateToString(Rate rate) {
    switch (rate) {
    case Rate::VeryPoor: return "VeryPoor";
    case Rate::Poor: return "Poor";
    case Rate::Average: return "Average";
    case Rate::Good: return "Good";
    case Rate::VeryGood: return "VeryGood";
    case Rate::Excellent: return "Excellent";
    default: return "Unknown";
    }
}


int LibraryUnit::uniqueNumber = 0; // инициализираме началото на номерата

LibraryUnit::LibraryUnit() : title("No_title"), publishers("No_publishers"),
    description("No_description"),date(),rate(Rate::VeryPoor),genre(Genre::Other),totalCopies(1),takenCopies(0), id(-1)
{
    // някакви невалидни стойности, не го ползваме безразборно
}

LibraryUnit::LibraryUnit(const std::string& title,
    const std::string& publishers,
    const std::string& description, 
    int day, int month, int year,
    const Rate& rate,
    const Genre& genre)
    :date(day, month, year) // тук се опитваме първо да създадем датата
{   
    // тук оставяме място за още валидации, които можем да направим и съответно съобщения за грешни данни
    // трябва да създадем валиден обект, като се грижим да посигурим тук някакъв инвариант
    if (title.empty()) {
        throw std::invalid_argument("Title cannot be empty!");
    }
    else if (publishers.empty()) {
        throw std::invalid_argument("Publishers cannot be empty!");
    }
    else if (description.empty()) {
        throw std::invalid_argument("Unit must have a non-empty description");
    }

    else if (rate > 6) {
        throw std::invalid_argument("Rate cannot be bigger than 5!");
    }

    this->id = LibraryUnit::generateNextNumber();
    this->rate = rate;
    this->genre = genre;
    this->title = title; // тук вече викаме operator= за стринг
    this->publishers = publishers;
    this->description = description;
    totalCopies = 1;
    takenCopies = 0;
}

LibraryUnit::LibraryUnit(const LibraryUnit& other)
    :title(other.title),publishers(other.publishers),
    description(other.description),date(other.date),
    genre(other.genre),rate(other.rate),totalCopies(other.totalCopies), takenCopies(other.takenCopies)
{
    // other е валиден обект, ние тепърва конструираме нашият
    id = LibraryUnit::generateNextNumber();
}

LibraryUnit& LibraryUnit::operator=(const LibraryUnit& other)
{
    if (this != &other) {
        // Правим дълбоко копиране през временни променливи
        try {
            std::string tempTitle = other.title;
            std::string tempPublishers = other.publishers;
            std::string tempDescription = other.description;

            // за атомарните типове не е нужно да го правим така това
            Date tempDate = other.date;
            Genre tempGenre = other.genre;
            Rate tempRate = other.rate;
            bool tempTotal = other.totalCopies;
            bool tempTaken = other.takenCopies;

            // Само ако всички операции са успешни, променяме членовете
            title = std::move(tempTitle);
            publishers = std::move(tempPublishers);
            description = std::move(tempDescription);
            date = tempDate;
            genre = tempGenre;
            rate = tempRate;
            totalCopies = tempTotal;
            takenCopies = tempTaken;
        }
        catch (...) {
            // Ако нещо се счупи, връщаме обекта в първоначалното му състояние
            // Този блок гарантира, че обектът остава непроменен при грешка
            throw; // Препредаваме exception-а нагоре
        }
    }
    return *this;
}

LibraryUnit::LibraryUnit(LibraryUnit&& other) noexcept
    :title(std::move(other.title)), publishers(std::move(other.publishers))
    , description(std::move(other.description)),
    totalCopies(other.totalCopies),takenCopies(other.takenCopies), id(other.id)
{
    // муув конструктор, не очакваме да хвърля грешка
    // просто преместваме нещата, като other е валиден обект, а нашият тепърва го създаваме
    std::swap(date, other.date);
    std::swap(genre, other.genre);
    std::swap(rate, other.rate);
    other.id = -1; // не го забравяме
}

LibraryUnit& LibraryUnit::operator=(LibraryUnit&& other) noexcept
{
    if (this != &other) {
        // не унищожаваме нищо, може по някаква причина външният свят да удължи живота на other
        // естествено, той ще си извика деструктор за ресурсите, така че сме ок
        std::swap(title, other.title);
        std::swap(publishers, other.publishers);
        std::swap(description, other.description);
        std::swap(date, other.date);
        std::swap(rate, other.rate);
        std::swap(genre, other.genre);
        std::swap(totalCopies, other.totalCopies);
        std::swap(takenCopies, other.takenCopies);

        other.id = -1;
    }
    return *this;
}

bool LibraryUnit::setNewTitle(const std::string& newTitle) {
    
    if (newTitle.empty()) {
        std::cout << "NewTitle cannot be empty!\n";
        return false;
    }
    
    if (newTitle == title) {
        // няма промяна
        return true;
    }

    title = newTitle; // може да хвърли но е ок
    return true;
}

bool LibraryUnit::setNewPublishers(const std::string& newPub)
{
    if (newPub.empty()) {
        std::cout << "New publishers cannot be empty!\n";
        return false;
    }

    if (newPub == publishers) {
        // няма промяна
        return true;
    }

    publishers = newPub; // може да хвърли но е ок
    return true;
}

bool LibraryUnit::setNewDescription(const std::string& desc)
{
    if (desc.empty()) {
        std::cout << "New description cannot be empty!\n";
        return false;
    }

    if (desc == description) {
        // няма промяна
        return true;
    }

    description = desc; // може да хвърли но е ок
    return true;
}

bool LibraryUnit::setNewDate(const Date& newDate)
{
    try
    {
        date = newDate;
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool LibraryUnit::setNewRate(const Rate& newRate)
{
    try
    {
        rate = newRate;
    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool LibraryUnit::setNewGenre(const Genre& newGenre)
{
    try
    {
        genre = newGenre;
    }
    catch (...)
    {
        return false;
    }
    return true;
}

void LibraryUnit::setTotalCopies(unsigned newTotal)
{ 
    totalCopies = newTotal;
    if (takenCopies > totalCopies)
    {
        takenCopies = totalCopies;  // За да не излизаме извън граница
    } 
}

void LibraryUnit::printBase(std::ostream& os)const {
    os << "Title: " << title << "\n";
    os << "Publishers: " << publishers<< "\n";
    os << "Description: " << description << "\n";
    date.print(os); // може да го направим да е operator<<
    os << "Rate: " << rateToString(this->rate) << "\n";
    os << "Genre: " << genreToString(this->genre) << "\n";
    os << "Availability: " << std::boolalpha << this->isAvailable() << "\n";
    os << "ID: " << id << "\n";
}

bool LibraryUnit::change() {
    std::string input;
    Date newDate;
    Rate newRate;
    Genre newGenre;

    // Заглавие
    while (true) {
        std::cout << "Input new title or 'cancel' : ";
        std::getline(std::cin, input);
        if (input == "cancel") return true;    // прекъсваме с вече записаните промени
        
        try
        {
            if (!setNewTitle(input)) {
                std::cerr << "Invalid title!";
                break;
            }
            std::cout << "Successfully changed to new title!" << "\n";
            break;

        }
        catch (...)
        {
            std::cerr << "Someting happened! \n";
            return false;
        }
    }

    // Издателство
    while (true) {
        std::cout << "Input ne publishiers or 'cancel' : ";
        std::getline(std::cin, input);
        if (input == "cancel") return true;
        try
        {
            if (!setNewPublishers(input)) {
                std::cerr << "Cannot change with input! (Invalid one!)" << "\n";
                break;
            }
            std::cout << "Successfully changed to: " << input << "\n";
            break;
        }
        catch (...)
        {
            std::cerr << "Something went wrong with changing pusblishers!" << "\n";
            return false;
        }
    }

    // Описание
    while (true) {
        std::cout << "Input new description or 'cancel' : ";
        std::getline(std::cin, input);
        if (input == "cancel") return true;
        try
        {
            if (!setNewDescription(input)) {
                std::cerr << "Cannot change with input! (Invalid one!)" << "\n";
                break;
            }
            std::cout << "Successfully changed to: " << input << "\n";
            break;

        }
        catch (...)
        {
            std::cerr << "Something went wrong with changing description!" << "\n";
            return false;
        }
    }

    // Дата 
    while (true) {
        int x, y, z;
        std::cout << "Enter 'cancel' or 'continue' to change date: \n";
        if (input == "cancel") return true;
        else if (input == "continue") {
            std::cout << "Input new date (format: YYYY-MM-DD) or 'cancel': ";
            std::cin >> x >> y >> z;
            try {
                newDate = Date(x,y,z); 
                if (!setNewDate(newDate)) 
                {
                    std::cerr << "Invalid date!" << "\n";
                    break;
                }
                std::cout << "Changed succesfully with new date!" << "\n";
                break;
            }

            catch (...) {
                std::cerr << "Something went wrong with date!\n";
                return false;
            }
        }
    }

    // Рейтинг 
    while (true) {
        std::cout << "Enter new rate (1-6) or 'cancel': ";
        std::getline(std::cin, input);
        if (input == "cancel") return true;
        try {
            int r = std::stoi(input);
            if (r < 1 || r > 6) throw std::out_of_range("rating");
            if (!setNewRate((Rate)r)) 
            {
                std::cerr << "Invalid rate!" << "\n";
                break;
            }
            std::cout << "Successfully changed to new Rate!" << "\n";
            break;
        }
        catch (...) {
            std::cout << "Something went wrong!\n";
            return false;
        }
    }

    // Жанр 
    while (true) {
        std::cout << "Input new genre or'cancel' : ";
        std::getline(std::cin, input);
        if (input == "cancel") return true;

        try {
            newGenre = parseGenreFromString(input);
            if (!setNewGenre(newGenre)) 
            {
                std::cerr << "Couldnt change genre -> invalid input!";
                break;
            };
            std::cout << "Succesfully changed to new Genre!" << "\n";
            break;
        }

        catch (...) {
            std::cout << "Someting went wrong! \n";
            return false;
        }
    }

    std::cout << "All you wanted been change! \n";
    return true;
}

bool LibraryUnit::equals(const LibraryUnit& other)const {
    return uniqueNumber == other.uniqueNumber;
}

bool LibraryUnit::incrementTakenCopies()
{
    if (!isAvailable()) {
        return false;
    }
    ++takenCopies;
 
}

bool LibraryUnit::decrementTakenCopies()
{

    if (takenCopies == 0) {
        return false;
    }
        --takenCopies;
}

void LibraryUnit::serializeBaseUnit(std::ostream& os) const
{
    // очакваме os да е отворен правилно в бинарен режим
    // не правим тук проверки за това работата е само да пишем и да гарантираме валидност на всяка операция от писането
    // серелизация на базовата част

    // ТЕЗИ ДОСАДНИ ПРОВЕРКИ МОГАТ ДА СЕ ИЗВЕДАТ ВЪВ ФУКНКЦИЯ

    size_t len;
    len = title.size();
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (os.fail()) {
        throw std::ios_base::failure("Error with writing size of title!");
    }
    // изоплзваме  c_str от стринг която връща c-style string - ок е
    os.write(title.c_str(), len);
    if (os.fail()) {
        throw std::ios_base::failure("Error with writing title!");
    }

    len = publishers.size();
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (os.fail()) {
        throw std::ios_base::failure("Error with writing size of publisher!");
    }
    os.write(publishers.c_str(), len);
    if (os.fail()) {
        throw std::ios_base::failure("Error with writing publishers!");
    }

    len = description.size();
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (os.fail()) {
        throw std::ios_base::failure("Error with writing size of description!");
    }
    os.write(description.c_str(), len);
    if (os.fail()) {
        throw std::ios_base::failure("Error with writing description!");
    }

    date.serialize(os); // имплементацията е в Дате

    os.write(reinterpret_cast<const char*>(&rate), sizeof(rate));
    if (!os) throw std::ios_base::failure("Error with writing rate");

    os.write(reinterpret_cast<const char*>(&genre), sizeof(genre));
    if (!os) throw std::ios_base::failure("Error with writing genre");

    os.write(reinterpret_cast<const char*>(&id), sizeof(id));
    if (!os) throw std::ios_base::failure("Error with writing id");

    os.write(reinterpret_cast<const char*>(&totalCopies), sizeof(totalCopies));
    if (!os) throw std::ios_base::failure("Error with writing isAvailable");

    os.write(reinterpret_cast<const char*>(&takenCopies), sizeof(takenCopies));
    if (!os) throw std::ios_base::failure("Error with writing isAvailable");

}
Type LibraryUnit::getType() const{
    return Type::UNKNOWN;
}

void LibraryUnit::deserializeBaseUnit(std::istream& is)
{
    // десерелизация на общите полета, има значение как сме ги серилизирари,
    // съобразяваме се с тези изисквания
    

    // ВАЖНО! тук не четем типа, той ще се вземе впредвид от ФАБРИКАТА
    // тук четем останалите неща
    if (!is.good()) {
        throw std::invalid_argument("Invalid stream input durint deserializeBaseUnit function!");
    }

    // Временни променливи
    std::string tempTitle, tempPublishers, tempDescription;
    Date tempDate;
    Rate tempRate;
    Genre tempGenre;
    int tempId;
    unsigned int tempTotal, tempTaken;

    // за да осигурим strong safety,
    // тъй като класът е абстрактен, трябва да си ги валидираме едно по едно, ако не беше можеше да създадем временен обект и да използваме муув
    size_t size;

    // title
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (is.fail()) {
        throw std::ios_base::failure("Error reading size from base unit!");
    }

    tempTitle.resize(size);

    // тази проверка е само ако има нещо записано  
    // иначе &title[0] -> не е дефинирано!!!!!!
    // а го правим така тъй като std::string.data() -> връща const char*, а ние искаме да прочетем вътре char*
    if (size > 0) {
        is.read(&tempTitle[0], size); 
        if (is.fail()) {
            throw std::ios_base::failure("Error reading title!");
        }
    }

    //publisher
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (is.fail()) {
        throw std::ios_base::failure("Error reading size from base unit!");
    }
   
    tempPublishers.resize(size);
    // тази проверка е само ако има нещо записано  
    // иначе &title[0] -> не е дефинирано!!!!!!
    // а го правим така тъй като std::string.data() -> връща const char*, а ние искаме да прочетем вътре char*
    if (size > 0) {
        is.read(&tempPublishers[0], size);
        if (is.fail()) {
            throw std::ios_base::failure("Error reading title!");
        }
    }

    // description
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (is.fail()) {
        throw std::ios_base::failure("Error reading size from base unit!");
    }

    tempDescription.resize(size);
    // тази проверка е само ако има нещо записано  
    // иначе &title[0] -> не е дефинирано!!!!!!
    // а го правим така тъй като std::string.data() -> връща const char*, а ние искаме да прочетем вътре char*
    if (size > 0) {
        is.read(&tempDescription[0], size);
        if (is.fail()) {
            throw std::ios_base::failure("Error reading description!");
        }
    }

    // data
    tempDate.deserialize(is);
    if (is.fail()) {
        throw std::ios_base::failure("Error reading date!");
    }
    // rate
    uint8_t r;
    is.read(reinterpret_cast<char*>(&r), sizeof(r));
    if (is.fail()) {
        throw std::ios_base::failure("Error reading rate!");
    }
    // ok е статичният каст, тъй като са съвместими и знаем точно какво стои зад него
    tempRate = static_cast<Rate>(r);

    // genre
    uint16_t g;
    is.read(reinterpret_cast<char*>(&g), sizeof(g));
    if (is.fail()) {
        throw std::ios_base::failure("Error reading genre!");
    }
    tempGenre = static_cast<Genre>(g);
    
    // id
    is.read(reinterpret_cast<char*>(&tempId), sizeof(tempId));
    if (is.fail()) {
        throw std::ios_base::failure("Error reading id!");
    }

    // availability
    is.read(reinterpret_cast<char*>(&tempTotal), sizeof(tempTotal));
    if (is.fail()) {
        throw std::ios_base::failure("Error reading total copies!");
    }

    is.read(reinterpret_cast<char*>(&tempTaken), sizeof(tempTaken));
    if (is.fail()) {
        throw std::ios_base::failure("Error reading taken copies!");
    }

    // тук как да валидираме, че сме прочели правилно нещата
    // не са най-добрите валидации, но е по-добре от нищо
    if (tempTitle.empty()) {
        throw std::invalid_argument("Title cannot be empty!");
    }

    if (!tempDate.validateDay(tempDate.getDay(), tempDate.getMonth())) {
        throw std::invalid_argument("Date is not valid!");
    }

    if (r > static_cast<uint8_t>(Rate::Excellent)) {
        throw std::invalid_argument("Invalid rate value!");
    }

    if (g > static_cast<uint16_t>(Genre::Other)) {
        throw std::invalid_argument("Invalid genre value!");
    }

    if (tempId < 0) {
        throw std::invalid_argument("ID must be non-negative!");
    }
    // сега вече можем да ги местим
    title = std::move(tempTitle);
    publishers = std::move(tempPublishers);
    description = std::move(tempDescription);
    date = std::move(tempDate);

    rate = tempRate;
    genre = tempGenre;
    id = tempId;
    totalCopies = tempTotal;
    takenCopies = tempTaken;
}

std::ostream& operator<<(std::ostream& os, const LibraryUnit& lib)
{
    lib.display(os);
    return os;
}

bool operator==(const LibraryUnit& lhs, const LibraryUnit& rhs)
{
    return lhs.equals(rhs);
}
