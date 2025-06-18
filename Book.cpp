#include "Book.h"
#include <iostream>

size_t Book::counter = 0;

Book::Book(const std::string& title
    , const std::string& publishers
    , const std::string& description
    , int day, int month, int year
    , const Rate& rate, const Genre& genre
    , const std::string& author
    , const std::string& isbn
    , const std::vector<std::string>& v)
    : LibraryUnit(title, publishers, description, day, month, year, rate, genre),
    ISBN(isbn),keyWords(v)
{
    if (author.empty()) {
        throw std::invalid_argument("Book must have author!");
    }
    this->author = author; // operator= of string се вика тук, тъй като вече е минал инициализация
    Book::counter++;
}

Book::Book(const Book& other) : 
    LibraryUnit(other), author(other.author),ISBN(other.ISBN),keyWords(other.keyWords)
{
    // тепърва създаваме нашата, оther е валидна
    // ако някое не успее, ще се хвърли грешка, което е ок и нямаме лийк
    Book::counter++;
}

Book& Book::operator=(const Book& other)
{
    if (this != &other) {
       // искаме да е  strong exception - т.е правим първо копие
        Book temp(other); // копираме първо всичко от other,ако някое гръмне, не сме променили нашият обект

        // ако сме тук, вс е ок
        // възползваме се от move
        *this = std::move(temp);
    }
    return *this;
}

Book::Book(Book&& other) noexcept
    :LibraryUnit(std::move(other)), author(std::move(other.author)), ISBN(std::move(other.ISBN)), keyWords(std::move(other.keyWords))
{
    // преместваме останалите неща
    Book::counter++;
}

Book& Book::operator=(Book&& other) noexcept
{
    if (this != &other) {
        // викаме move= от базовия клас
        LibraryUnit::operator=(std::move(other)); // noexcept guarantee
        author = std::move(other.author); // move=  за  string
        ISBN = std::move(other.ISBN); // move=  за  string
        keyWords = std::move(other.keyWords); // move=  за vector
    }
    return *this;
}

Book::~Book() noexcept
{
    if (Book::counter > 0) {
        Book::counter--;
    }
    // няма какво да правим тук
}

bool Book::setNewAuthor(const std::string& newAth)
{
    if (newAth.empty()) {
        std::cout << "New author cannot be empty!\n";
        return false;
    }

    if (newAth== author) {
        // няма промяна
        return true;
    }

    author = newAth; // може да хвърли но е ок
    return true;
}

bool Book::setNewKeyWords(const std::vector<std::string>& newKeyWords)
{
    if (newKeyWords.empty()) {
        std::cout << "Cannot have zero keyWords!\n";
        return false;
    }
    if (newKeyWords == keyWords) {
        return true;
    }

    keyWords = newKeyWords; // вектор operator=
    return true;
}

bool Book::setNewISBN(const std::string& newISBN)
{
    
        if (newISBN.empty()) {
            // Позволяваме празен ISSN (незадължителен)
            if (ISBN.empty()) {
                // няма промяна
                return true;
            }

            ISBN.clear();
            return true;
        }

        // Можеш да добавиш допълнителна проверка за валиден формат на ISSN
        if (newISBN == ISBN) {
            // няма промяна
            return true;
        }

        ISBN = newISBN;
        return true;
    
}

void Book::display(std::ostream& out) const
{
    LibraryUnit::printBase(out); // така викаме имплементацията от базовия клас
    Book::printBook(out);
}

void Book::serialize(std::ostream& os) const
{
    // аналогично не правим тук проверката дали е отворен правилно
    // очаква се това да е сторено - да е в правилен формат за работа и тн

    // тук само пишем логиката за писане
    // първо типа, многоо важна стъпка, за да можем после да знаем във фабриката типа на новия обект
    Type t = this->getType(); //  това не е нужно, тъй като си знаем че е книга
    os.write(reinterpret_cast<const char*>(&t), sizeof(t));
    if (os.fail()) {
        throw std::ios_base::failure("Error with writing type");
    }
    LibraryUnit::serializeBaseUnit(os); // инициализираме базовата част
    if (os.fail()) {
        throw std::ios_base::failure("Error writing unit!");
    }
    // собствените полета на book
    Book::serializeBookUnit(os);
    if (os.fail()) {
        throw std::ios_base::failure("Error writing BookUnit!");
    }
}

void Book::deserialize(std::istream& is)
{
    if (!is.good()) {
        throw std::invalid_argument("Invalid input stream before deserializing Book!");
    }
    
    // book temp
    // temp.deserialize..
    // temp.deseerialize
    // *this = std::move(temp);
    
    // за да осигурим strong exception

    Book temp;
    
    temp.deserializeBaseUnit(is);
    if (is.fail()) {
        throw std::ios_base::failure("Error reading base unit!");
    }

    temp.deserializeBookUnit(is);
    if (is.fail()) {
        throw std::ios_base::failure("Error reading book unit!");
    }

    *this = std::move(temp);

}


void Book::addKeyWords(const std::string& key)
{
    for (size_t i = 0; i < keyWords.size(); i++)
    {
        if (keyWords[i] == key) {
            std::cout << "This key word has already been added to that book!";
            return;
        }
    }
    keyWords.push_back(key);
}

Book* Book::createInteractively()
{
    std::string title, publishers, description, author, isbn;
    int day, month, year;
    int rateValue, genreValue;
    std::vector<std::string> tags;

    std::cout << "Enter title (or 'cancel' to abort): ";
    std::getline(std::cin, title);
    if (title == "cancel") return nullptr;

    std::cout << "Enter publishers (or 'cancel' to abort): ";
    std::getline(std::cin, publishers);
    if (publishers == "cancel") return nullptr;

    std::cout << "Enter description (or 'cancel' to abort): ";
    std::getline(std::cin, description);
    if (description == "cancel") return nullptr;

    std::cout << "Enter author (or 'cancel' to abort): ";
    std::getline(std::cin, author);
    if (author == "cancel") return nullptr;

    std::cout << "Enter ISBN (optional, or 'cancel' to abort): ";
    std::getline(std::cin, isbn);
    if (isbn == "cancel") return nullptr;

    std::cout << "Enter publication date (day, month, year): ";
    if (!(std::cin >> day >> month >> year)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Invalid date input. Aborting.\n";
        return nullptr;
    }
    std::cin.ignore(); // clear newline

    std::cout << "Enter rating (1–6): ";
    if (!(std::cin >> rateValue) || rateValue < 1 || rateValue > 6) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Invalid rating. Aborting.\n";
        return nullptr;
    }
    std::cin.ignore();

    std::cout << "Enter genre as number (e.g., 0: Fiction, 1: Nonfiction, etc.): ";
    if (!(std::cin >> genreValue)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Invalid genre. Aborting.\n";
        return nullptr;
    }
    std::cin.ignore();

    std::string tag;
    std::cout << "Enter tags one by one (type 'done' to finish, or 'cancel' to abort):\n";
    while (true) {
        std::cout << "Tag: ";
        std::getline(std::cin, tag);
        if (tag == "done") break;
        if (tag == "cancel") return nullptr;
        tags.push_back(tag);
    }

    return new Book(title, publishers, description, day, month, year,
        static_cast<Rate>(rateValue), static_cast<Genre>(genreValue),
        author, isbn, tags);
}

bool Book::change()
{
    if (!LibraryUnit::change()) {
        std::cerr << "Something happened changing base part!\n";
        return false;
    }
    if (!changeBookPart()) {
        std::cerr << "Error changing book part!\n";
        return false;
    }
    return true;
}

Book::Book() : LibraryUnit(),author("No_author"),
    keyWords(), ISBN("No_ISBN")
{
    counter++;
}

bool Book::changeBookPart()
{
    std::string input;

    // Автор
    while (true) {
        std::cout << "Input new author or 'cancel': ";
        std::getline(std::cin, input);
        if (input == "cancel") return true;

        try {
            if (!setNewAuthor(input)) {
                std::cerr << "Invalid or unchanged author!\n";
                continue;
            }
            std::cout << "Successfully changed author!\n";
            break;
        }
        catch (...) {
            std::cerr << "Unexpected error while changing author!\n";
            return false;
        }
    }

    // Ключомите думии
    while (true) {
        std::cout << "Input number of new keywords or 'cancel': ";
        std::getline(std::cin, input);
        if (input == "cancel") return true;

        try {
            int num = std::stoi(input);
            if (num <= 0) {
                std::cerr << "Keyword count must be positive!\n";
                continue;
            }

            std::vector<std::string> newKeywords;
            for (int i = 0; i < num; ++i) {
                std::string kw;
                std::cout << "Enter keyword #" << (i + 1) << ": ";
                std::getline(std::cin, kw);
                if (kw.empty()) {
                    std::cerr << "Keyword cannot be empty!\n";
                    --i; // повтаряме
                    continue;
                }
                newKeywords.push_back(kw);
            }

            if (!setNewKeyWords(newKeywords)) {
                std::cerr << "Invalid or unchanged keywords!\n";
                continue;
            }

            std::cout << "Successfully changed keywords!\n";
            break;
        }
        catch (...) {
            std::cerr << "Error reading keywords count!\n";
            return false;
        }
    }

    // ISBN
    while (true) {
        std::cout << "Input new ISBN or 'cancel': ";
        std::getline(std::cin, input);
        if (input == "cancel") return true;

        try {
            if (!setNewISBN(input)) {
                std::cerr << "Invalid or unchanged ISBN!\n";
                continue;
            }
            std::cout << "Successfully changed ISBN!\n";
            break;
        }
        catch (...) {
            std::cerr << "Unexpected error while changing ISBN!\n";
            return false;
        }
    }

    std::cout << "Successfully changed book part!\n";
    return true;
}

void Book::serializeBookUnit(std::ostream& os) const
{
    size_t len;
    len = author.size();

    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (os.fail()) {
        throw std::ios_base::failure("Error writing authro size!");
    }
    os.write(author.c_str(), len);
    if (os.fail()) {
        throw std::ios_base::failure("Error writing author!");
    }


    // да си напишем колко е размерът
    len = keyWords.size();
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (os.fail()) {
        throw std::ios_base::failure("Error writing size of keyWords!");
    }
    size_t keySize = keyWords.size();
    for (size_t i = 0; i < keySize; i++)
    {
        len = keyWords[i].size();
        os.write(reinterpret_cast<const char*>(&len), sizeof(len));
        if (os.fail()) {
            throw std::ios_base::failure("Error writing keyWord size!");
        }
        os.write(keyWords[i].c_str(), len);
        if (os.fail()) {
            throw std::ios_base::failure("Error writing keyWord!");
        }
    }
    len = ISBN.size();
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (os.fail()) {
        throw std::ios_base::failure("Error writing ISBN size!");
    }
    os.write(ISBN.c_str(), len);
    if (os.fail()) {
        throw std::ios_base::failure("Error writing ISBN!");
    }
}

void Book::deserializeBookUnit(std::istream& is)
{
    if (!is.good()) {
        throw std::invalid_argument("Error with input stream!");
    }
    size_t size;
    
    std::string tempAuthor;
    std::vector<std::string> tempKeyWords;
    std::string tempISBN;


    // author 
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (is.fail()) {
        throw std::ios_base::failure("Error reading size of author!");
    }
    tempAuthor.resize(size);
    if (size > 0) {
        is.read(&tempAuthor[0], size);
        if (is.fail()) {
            throw std::ios_base::failure("Error reading author!");
        }
    }

    // keyWords
    size_t keyWordsSize;
    is.read(reinterpret_cast<char*>(&keyWordsSize), sizeof(keyWordsSize));
    if (is.fail()) {
        throw std::ios_base::failure("Error reading keyWordsSize!");
    }
    
    for (size_t i = 0; i < keyWordsSize; i++)
    {
        is.read(reinterpret_cast<char*>(&size), size);
        if (is.fail()) {
            throw std::ios_base::failure("Error reading current keyWords size!");
        }
        std::string keyWord(size, '\0');
        if (size > 0) {
            is.read(&keyWord[0], size);
            if (is.fail()) {
                throw std::ios_base::failure("Error reading current keyWord!");
            }
        }
        tempKeyWords.push_back(std::move(keyWord));
    }
    
    //ISBBN
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (is.fail()) {
        throw std::ios_base::failure("Error reading ISBN size!");
    }
    tempISBN.resize(size);
    if (size > 0) {
        is.read(&tempISBN[0], size);
        if (is.fail()) {
            throw std::ios_base::failure("Error reading ISBN!");
        }
    }
    
    // Валидация - някаква
    if (tempAuthor.empty()) {
        throw std::invalid_argument("Author cannot be empty.");
    }
    if (tempISBN.empty()) {
        throw std::invalid_argument("ISBN cannot be empty.");
    }

    // Всичко е успешно прочетено и валидирано -> прехвърляне
    author = std::move(tempAuthor);
    keyWords = std::move(tempKeyWords);
    ISBN = std::move(tempISBN);
    // има и по-добър начин с move operator=  и създаване на временен обект от данните които сме прочели

}

void Book::printBook(std::ostream& out) const
{
    out << "Author: " << author << "\n";
    out << "Key words: ";
    for (size_t i = 0; i < keyWords.size(); i++)
    {
        out << keyWords[i];
        if (i + 1 < keyWords.size()) {
            out << ", ";
        }
    }
    out << "\n";
    out << "ISBN: " << ISBN << "\n";
}

std::ostream& operator<<(std::ostream& out, const Book& book)
{
    book.display(out);
    return out;
}
