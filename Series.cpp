#include "Series.h"

size_t Series::counter = 0;

void Series::serialize(std::ostream& os) const
{
	Type t = getType();
	os.write(reinterpret_cast<const char*>(&t), sizeof(t));
	if (os.fail()) {
		throw std::ios_base::failure("Error writing type of Series!");
	}

	LibraryUnit::serializeBaseUnit(os);
	if (os.fail()) {
		throw std::ios_base::failure("Error writing base unit!");
	}
	Book::serializeBaseUnit(os);
	if (os.fail()) {
		throw std::ios_base::failure("Error writing book unit!");
	}
	Periodicals::serializePeriodicalsUnit(os);
	if (os.fail()) {
		throw std::ios_base::failure("Error writing periodicals unit!");
	}
	//  може да извикаме serializeSeriesUnit(), ако държим и в серията данни 
}

void Series::deserialize(std::istream& is)
{
	// по компонентии!!!
	if (!is.good()) {
		throw std::invalid_argument("Error with input stream before deserializing Series!");
	}
	Series s;

	s.deserializeBaseUnit(is);
	if (!is.good()) {
		throw std::ios_base::failure("Error with deserializing Base Unit!");
	}
	s.deserializeBookUnit(is);
	if (!is.good()) {
		throw std::ios_base::failure("Error with deserializing Book!");
	}
	s.deserializePeriodicalsUnit(is);
	if (!is.good()) {
		throw std::ios_base::failure("Error with deserializing Periodicals!");
	}
	*this = std::move(s);
}

Series* Series::createInteractively() {
    std::string title, publishers, description;
    std::string author, isbn, issn;
    int genreValue, rateValue;
    int day, month, year;
    unsigned int issueNumber;
    std::vector<std::string> keywords;
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

    std::cout << "Enter publication date (day month year): ";
    if (!(std::cin >> day >> month >> year)) {
        std::cin.clear(); std::cin.ignore(1000, '\n');
        std::cout << "Invalid date. Aborting.\n";
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

    std::cout << "Enter genre as number (e.g., 0: Fiction, 1: Nonfiction, etc.): ";
    if (!(std::cin >> genreValue)) {
        std::cin.clear(); std::cin.ignore(1000, '\n');
        std::cout << "Invalid genre. Aborting.\n";
        return nullptr;
    }
    std::cin.ignore();

    std::cout << "Enter author (or 'cancel' to abort): ";
    std::getline(std::cin, author);
    if (author == "cancel") return nullptr;

    std::cout << "Enter ISBN (optional, or 'cancel' to abort): ";
    std::getline(std::cin, isbn);
    if (isbn == "cancel") return nullptr;

    std::cout << "Enter keywords for the book (type 'done' to finish, or 'cancel' to abort):\n";
    while (true) {
        std::string keyword;
        std::cout << "Keyword: ";
        std::getline(std::cin, keyword);
        if (keyword == "done") break;
        if (keyword == "cancel") return nullptr;
        keywords.push_back(keyword);
    }

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

    std::string response;
    std::cout << "Do you want to add articles? (yes/no): ";
    std::getline(std::cin, response);
    if (response == "cancel") return nullptr;

    while (response == "yes") {
        std::string articleTitle, articleAuthor;
        std::vector<std::string> articleKeywords;
        std::string keyword;

        std::cout << "Enter article title (or 'cancel' to abort): ";
        std::getline(std::cin, articleTitle);
        if (articleTitle == "cancel") return nullptr;

        std::cout << "Enter article author (or 'cancel' to abort): ";
        std::getline(std::cin, articleAuthor);
        if (articleAuthor == "cancel") return nullptr;

        std::cout << "Enter article keywords (type 'done' to finish, or 'cancel' to abort):\n";
        while (true) {
            std::cout << "Keyword: ";
            std::getline(std::cin, keyword);
            if (keyword == "done") break;
            if (keyword == "cancel") return nullptr;
            articleKeywords.push_back(keyword);
        }

        articles.emplace_back(articleTitle, articleAuthor, articleKeywords);

        std::cout << "Add another article? (yes/no): ";
        std::getline(std::cin, response);
        if (response == "cancel") return nullptr;
    }

    return new Series(
        title,
        publishers,
        description,
        day, month, year,
        Rate(rateValue),
        static_cast<Genre>(genreValue),
        author,
        issueNumber,
        isbn,
        keywords,
        issn,
        articles
    );
}

bool Series::change()
{
    if (!LibraryUnit::change()) {
        std::cerr << "Error chaning base part!\n";
        return false;
    }
    if (!Book::changeBookPart()) {
        std::cerr << "Error changing book part!\n";
        return false;
    }

    if (!Periodicals::changePeriodicalsPart()) {
        std::cerr << "Error changing periodicalsPart!\n";
        return false;
    }
    std::cout << "Succesffully changed series part!\n";
    return true;
}

Series::Series() : LibraryUnit(), Book(), Periodicals()
{
	counter++;
}

void Series::deserializeSeriesUnit(std::istream& is)
{

	// засега нищо, тъй като няма никакви данни, на в бъдещето ще е полезна!
}

void Series::serializeSeriesUnit(std::ostream& os) const
{
	// специфични данни за Серията
	// засега няма нищо.
	return;
}

void Series::printSeries(std::ostream& os) const
{
	// засега няма никой тук
	return;
}

