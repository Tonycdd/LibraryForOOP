#include "Periodicals.h"
const unsigned int MAX = 10000;
size_t Periodicals::counter = 0;
Periodicals::Periodicals(const std::string& title,
    const std::string& publishers,
    const std::string& description,
    const Genre& genre, const Rate& rate,
    int month, int year,
    unsigned int issueNumber,
    const std::string& issn,
    const std::vector<Article>& articles)
    : LibraryUnit(title,publishers,description,1,month,year,rate,genre),
    issueNumber(issueNumber), ISSN(issn), articles(articles)
{
    if (this->issueNumber > MAX) {
        throw std::invalid_argument("Invalid issueNumber for that Periodical!");
    }
    Periodicals::counter++;
}

Periodicals::Periodicals(const Periodicals& other)
    :LibraryUnit(other), issueNumber(other.issueNumber), ISSN(other.ISSN), articles(other.articles)
{
    // няма какво друго да копираме
    Periodicals::counter++;
}

Periodicals& Periodicals::operator=(const Periodicals& other)
{
    if (this == &other) { return *this; };
    // два валидни обекта, отново искаме strong exception safety
    Periodicals temp(other); // правим му копие
    *this = std::move(temp); // move operator=
    return *this;
}

Periodicals::Periodicals(Periodicals&& other) noexcept
    :LibraryUnit(std::move(other)), issueNumber(other.issueNumber), ISSN(std::move(other.ISSN)), articles(std::move(other.articles))
{
    // нямаме какво друго да правим тук
    Periodicals::counter++;
}

Periodicals& Periodicals::operator=(Periodicals&& other) noexcept
{
    // два валидни обекта
    if (this != &other) {
        LibraryUnit::operator=(std::move(other)); // не хвърля грешка
        issueNumber = other.issueNumber;
        ISSN = std::move(other.ISSN); //  move оператор= за  стрингове
        articles = std::move(other.articles); // move operator= за вектора
        
        other.issueNumber = 0; //  не е задължително 
    }
    return *this;
}

Periodicals::~Periodicals() noexcept
{
    if (Periodicals::counter > 0) {
        Periodicals::counter--;
    }
}

void Periodicals::display(std::ostream& os) const
{
    LibraryUnit::printBase(os);
    Periodicals::printPeriodicals(os);
}

bool Periodicals::setNewISSN(const std::string& newISSN)
{
    if (newISSN.empty()) {
        // Позволяваме празен ISSN (незадължителен)
        if (ISSN.empty()) {
            // няма промяна
            return true;
        }

        ISSN.clear();
        return true;
    }

    if (newISSN == ISSN) {
        // няма промяна
        return true;
    }

    ISSN = newISSN;
    return true;
}

bool Periodicals::setNewIssueNumber(unsigned int newIssueNumber)
{
    if (newIssueNumber == 0) {
        std::cout << "Issue number must be positive!\n";
        return false;
    }

    if (newIssueNumber == issueNumber) {
        // Няма промяна
        return true;
    }

    issueNumber = newIssueNumber;
    return true;
}

bool Periodicals::setNewArticles(const std::vector<Article>& newArticles)
{
    if (newArticles.empty()) {
        std::cout << "Caanot be empty!";
        return false;
    }
    else if (newArticles == articles) {
        return true;
    }
    for (size_t i = 0; i < newArticles.size(); i++)
    {
        if (newArticles[i].author.empty() || newArticles[i].title.empty() || newArticles[i].keyWords.empty()) {
            std::cerr << "Each article must have author, tilte and key words!";
            return false;
        }
    }

    articles = newArticles;
    return true;
}

void Periodicals::addNewArticle(const Article& article)
{
    for (size_t i = 0; i < articles.size(); i++)
    {
        if (articles[i] == article) {
            std::cout << "This article is already in our collection!";
            return;
        }
    }
    articles.push_back(article);
}

void Periodicals::serialize(std::ostream& os) const
{
    Type t = getType();
    os.write(reinterpret_cast<const char*>(&t), sizeof(t));
    if (os.fail()) {
        throw std::ios_base::failure("Problem writing type!");
    }
    LibraryUnit::serializeBaseUnit(os); // използваме общите неща 
    //сериализираме нещата специфични за периодично издание
    Periodicals::serializePeriodicalsUnit(os);
}

void Periodicals::deserialize(std::istream& is)
{

    // аналогично както при Book
    
    if (!is.good()) {
        throw std::invalid_argument("Error with input stream before deserializing Periodicals!");
    }
    Periodicals temp;
    temp.deserializeBaseUnit(is);
    if (is.fail()) {
        throw std::ios_base::failure("Error reading base unit!");
    }
    temp.deserializePeriodicalsUnit(is);
    if (is.fail()) {
        throw std::ios_base::failure("Error reading periodical unit!");
    }
    *this = std::move(temp);
}

Periodicals* Periodicals::createInteractively() {
    std::string title, publishers, description, issn;
    int genreValue, rateValue;
    int month, year;
    unsigned int issueNumber;
    std::vector<Article> articles;

    std::cout << "Enter title (or 'cancel' to abort): ";
    std::getline(std::cin, title);
    if (title == "cancel") return nullptr;

    std::cout << "Enter publishers (or 'cancel' to abort): ";
    std::getline(std::cin, publishers);
    if (publishers == "cancel") return nullptr;

    std::cout << "Enter description (or 'cancel' to abort): ";
    std::getline(std::cin, description);
    if (description == "cancel") return nullptr;

    std::cout << "Enter genre as number (e.g., 0: Fiction, 1: Nonfiction, etc.): ";
    if (!(std::cin >> genreValue)) {
        std::cin.clear(); std::cin.ignore(1000, '\n');
        std::cout << "Invalid genre. Aborting.\n";
        return nullptr;
    }
    std::cin.ignore();

    std::cout << "Enter rating (1–5): ";
    if (!(std::cin >> rateValue) || rateValue < 1 || rateValue > 5) {
        std::cin.clear(); std::cin.ignore(1000, '\n');
        std::cout << "Invalid rating. Aborting.\n";
        return nullptr;
    }
    std::cin.ignore();

    std::cout << "Enter publication month and year (e.g., 5 2024): ";
    if (!(std::cin >> month >> year)) {
        std::cin.clear(); std::cin.ignore(1000, '\n');
        std::cout << "Invalid date. Aborting.\n";
        return nullptr;
    }
    std::cin.ignore();

    std::cout << "Enter issue number: ";
    if (!(std::cin >> issueNumber)) {
        std::cin.clear(); std::cin.ignore(1000, '\n');
        std::cout << "Invalid issue number. Aborting.\n";
        return nullptr;
    }
    std::cin.ignore();

    std::cout << "Enter ISSN (optional, or 'cancel' to abort): ";
    std::getline(std::cin, issn);
    if (issn == "cancel") return nullptr;

    // Add articles interactively
    std::string response;
    std::cout << "Do you want to add articles? (yes/no): ";
    std::getline(std::cin, response);
    if (response == "cancel") return nullptr;

    while (response == "yes") {
        std::string articleTitle, articleAuthor;
        std::vector<std::string> keywords;
        std::string keyword;

        std::cout << "Enter article title (or 'cancel' to abort): ";
        std::getline(std::cin, articleTitle);
        if (articleTitle == "cancel") return nullptr;

        std::cout << "Enter article author (or 'cancel' to abort): ";
        std::getline(std::cin, articleAuthor);
        if (articleAuthor == "cancel") return nullptr;

        std::cout << "Enter keywords for this article (type 'done' to finish, or 'cancel' to abort):\n";
        while (true) {
            std::cout << "Keyword: ";
            std::getline(std::cin, keyword);
            if (keyword == "done") break;
            if (keyword == "cancel") return nullptr;
            keywords.push_back(keyword);
        }

        articles.emplace_back(articleTitle, articleAuthor, keywords);

        std::cout << "Add another article? (yes/no): ";
        std::getline(std::cin, response);
        if (response == "cancel") return nullptr;
    }

    return new Periodicals(
        title,
        publishers,
        description,
        static_cast<Genre>(genreValue),
        Rate(rateValue),
        month,
        year,
        issueNumber,
        issn,
        articles
    );
}
bool Periodicals::change()
{
    if (!LibraryUnit::change()) {
        std::cerr << "Something happened changing base part!\n";
        return false;
    }

    if (!changePeriodicalsPart()) {
        std::cerr << "Error changing periodicals part!\n";
        return false;
    }
    return true;
}
Periodicals::Periodicals() : LibraryUnit(), issueNumber(1), ISSN("NO_ISN"),
                            articles()
{
    counter++;
}

bool Periodicals::changePeriodicalsPart()
{
    std::string input;

    // issueNumber
    while (true) {
        std::cout << "Input new issue number or 'cancel': ";
        std::getline(std::cin, input);
        if (input == "cancel") return true;

        try {
            unsigned int num = std::stoul(input);
            if (!setNewIssueNumber(num)) {
                std::cerr << "Invalid issue number!\n";
                continue;
            }
            std::cout << "Successfully changed issue number!\n";
            break;
        }
        catch (...) {
            std::cerr << "Invalid input! Please enter a positive number.\n";
        }
    }

    // ISSN
    while (true) {
        std::cout << "Input new ISSN or 'cancel': ";
        std::getline(std::cin, input);
        if (input == "cancel") return true;

        try
        {
            if (!setNewISSN(input)) {
                std::cerr << "Invalid ISSN!\n";
                continue;
            }
            std::cout << "Successfully changed to new ISSN!\n";
            break;
        }
        catch (const std::exception&)
        {
            std::cerr << "Sometinh happend!\n";
            return false;
        }
    }

    while (true) {
        std::cout << "Input number of articles or 'cancel': ";
        std::getline(std::cin, input);
        if (input == "cancel") return true;

        try {
            int num = std::stoi(input);
            if (num <= 0) {
                std::cerr << "Number of articles must be positive!\n";
                continue;
            }

            std::vector<Article> newArticles;
            for (int i = 0; i < num; ++i) {
                Article art;
                std::cout << "Enter title for article #" << (i + 1) << ": ";
                std::getline(std::cin, art.title);
                if (art.title.empty()) {
                    std::cerr << "Title cannot be empty!\n";
                    --i; // repeat this article input
                    continue;
                }

                std::cout << "Enter author for article #" << (i + 1) << ": ";
                std::getline(std::cin, art.author);
                if (art.author.empty()) {
                    std::cerr << "Author cannot be empty!\n";
                    --i; // repeat this article input
                    continue;
                }

                std::cout << "Enter keywords for article #" << (i + 1) << " (comma separated): ";
                std::string keywordsLine;
                std::getline(std::cin, keywordsLine);

                art.keyWords.clear(); // Изчистваме списъка от ключови думи, за да добавим новите

                size_t pos = 0, found;
                while ((found = keywordsLine.find(',', pos)) != std::string::npos) {
                    // Търсим позицията на запетаята, започвайки от pos
                    std::string kw = keywordsLine.substr(pos, found - pos);
                    // Вземаме подниза от pos до намерената запетая (не включително)

                    if (!kw.empty()) art.keyWords.push_back(kw);
                    // Ако поднизът не е празен, добавяме го като ключова дума

                    pos = found + 1;
                    // Преместваме позицията след намерената запетая, за да търсим следващата
                }

                // След като няма повече запетаи, вземаме остатъка от низа след последната запетая
                std::string lastKw = keywordsLine.substr(pos);
                if (!lastKw.empty()) art.keyWords.push_back(lastKw);

                newArticles.push_back(std::move(art));
            }

            if (!setNewArticles(newArticles)) {
                std::cerr << "Invalid articles list!\n";
                break;
            }
            std::cout << "Successfully changed articles!\n";
            break;
        }
        catch (...) {
            std::cerr << "Invalid input! Please enter a positive number.\n";
            return false;
        }
    }

    std::cout << "Successfully changed periodical part!\n";
    return true;
}

void Periodicals::deserializePeriodicalsUnit(std::istream& is)
{
    // десериализация само на частите от Periodicals!
    // това ни трябва за да правим покомпонентно решение

    size_t size;
    std::vector<Article> tempArticles;
    std::string tempISSN;
    unsigned tempIssueNumber;

    if (!is.good()) {
        throw std::invalid_argument("Invalid stream input during deserializePeriodicalsUnit function!");
    }

    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (!is.good()) {
        throw std::ios_base::failure("Error while reading size of articles!");
    }
    tempArticles.reserve(size);
    for (size_t i = 0; i < size; i++)
    {
        Article a;
        a.deserialize(is);
        // муув ctor
        tempArticles.push_back(std::move(a)); // може да хвърли грешка
    }
    if (!is.good()) {
        throw std::ios_base::failure("Error while reading articles!");
    }
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (!is.good()) {
        throw std::ios_base::failure("Error while reading size of ISSN number!");
    }
    tempISSN.resize(size);
    if (size > 0) {
        is.read(&tempISSN[0], size);
    }
    if (!is.good()) {
        throw std::ios_base::failure("Error while reading ISSN number!");
    }
    is.read(reinterpret_cast<char*>(&tempIssueNumber), sizeof(tempIssueNumber));
    if (!is.good()) {
        throw std::ios_base::failure("Error while reading issueNumber!");
    }
    if (tempArticles.empty()) {
        throw std::invalid_argument("Article cannot be empty!");
    }

    // иначе са били успешно прочетени
    articles = std::move(tempArticles);
    ISSN = std::move(tempISSN);
    issueNumber = tempIssueNumber;
}

void Periodicals::serializePeriodicalsUnit(std::ostream& os) const
{
    size_t len;
    len = articles.size();
    os.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (os.fail()) {
        throw std::ios_base::failure("Error writing size of articles!");
    }
    size_t articlesSize = articles.size();
    for (size_t i = 0; i < articlesSize; i++)
    {
        articles[i].serialize(os);
    }
    if (os.fail()) {
        throw std::ios_base::failure("Error writing articles!");
    }
    len = ISSN.size();
    os.write(reinterpret_cast<const char*>(&len),sizeof(len));
    if (os.fail()) {
        throw std::ios_base::failure("Error with writing len of ISSN!");
    }
    os.write(ISSN.c_str(), len);
    if (os.fail()) {
        throw std::ios_base::failure("Error with writing ISSN!");
    }

    os.write(reinterpret_cast<const char*>(&issueNumber), sizeof(issueNumber));
    if (os.fail()) {
        throw std::ios_base::failure("Error with writing issueNumber!");
    }
}

void Periodicals::printPeriodicals(std::ostream& os) const
{
    os << "Issue number: " << issueNumber << "\n";
    os << "ISSN" << ISSN << "\n";
    os << "Articles: ";
    for (size_t i = 0; i < articles.size(); i++)
    {
        articles[i].print(os);
    }
    os << "\n";
}

std::ostream& operator<<(std::ostream& os, const Periodicals& periodical)
{
    periodical.display(os);
    return os;
}
