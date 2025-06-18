#include "Administrator.h"
#include <regex>// отново да питаме дали е ок

Administrator::Administrator(const std::string& user,
    const std::string& pass,
    int day1, int month1, int year1,
    int day2, int month2, int year2,
    const std::string& email) :
    LibraryPerson(user, pass, day1, month1, year1, day2, month2, year2), email(email)
{
    if (this->email.empty())
    {
        throw std::invalid_argument("Email cannot be empty!");
    }
    else if (!Administrator::validateEmail(this->email)) {
        throw std::invalid_argument("Email is not in a valid form!");
    }
}

Administrator::Administrator(const std::string& user,
    const std::string& pass,
    const Date& registerDate,
    const Date& lastlogin,
    const std::string& email)
    : LibraryPerson(user, pass, registerDate.getDay(), registerDate.getMonth(), registerDate.getYear(),
        lastlogin.getDay(), lastlogin.getMonth(), lastlogin.getYear()), email(email)
{
    if (this->email.empty())
    {
        throw std::invalid_argument("Email cannot be empty!");
    }
    else if (!Administrator::validateEmail(this->email)) {
        throw std::invalid_argument("Email is not in a valid form!");
    }
}

Administrator::Administrator(const Administrator& other)
    :LibraryPerson(other), email(other.email)
{
    // нямаме какво повече да правим тук
}

Administrator& Administrator::operator=(const Administrator& other)

{
    if (this != &other) {
        //copy and swap
        Administrator cpy(other);
        *this = std::move(cpy);
    }
    return *this;
}

Administrator::Administrator(Administrator&& other) noexcept
    :LibraryPerson(std::move(other)), email(std::move(other.email))
{
    // няммае какво да правим тук повечe
}

Administrator& Administrator::operator=(Administrator&& other) noexcept
{
    if (this != &other) {
        LibraryPerson::operator=(std::move(other));
        email = std::move(other.email);
    }

    return *this;
}

// от интернет е!
bool Administrator::validateEmail(const std::string& email)
{
    size_t atPos = email.find('@');
    size_t dotPos = email.rfind('.');

    // Наличие на '@' и '.' и в правилен ред
    if (atPos == std::string::npos || dotPos == std::string::npos)
        return false;

    if (atPos == 0 || atPos == email.length() - 1)
        return false; // не може да започва/завършва с '@'

    if (dotPos < atPos + 2 || dotPos == email.length() - 1)
        return false; // '.' трябва да е след '@', и не в края

    // Локалната част: от начало до '@'
    for (size_t i = 0; i < atPos; ++i) {
        char c = email[i];
        if (!(std::isalnum(c) || c == '_' || c == '.' || c == '+' || c == '-'))
            return false;
    }

    // Домен частта: след '@' до края
    for (size_t i = atPos + 1; i < email.length(); ++i) {
        char c = email[i];
        if (!(std::isalnum(c) || c == '-' || c == '.'))
            return false;
    }

    return true;
}

Administrator::~Administrator() noexcept
{
    // не правим нищо тук
}

LibraryPerson* Administrator::clone() const
{
    return new Administrator(*this);
}

void Administrator::display(std::ostream& out) const
{
    LibraryPerson::display(out);
    out << "Email: " << email << "\n";
}

void Administrator::deserialize(std::istream& is)
{

    if (!is.good()) {
        throw std::invalid_argument("Invalid stream before reading from Administrator Unit!");
   }
    Administrator a;
    a.deserializeBase(is);
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading PersonUnit!");
    }
    a.deserializeAdministratorUnit(is);
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading Adminsistrator!");
    }
    *this = std::move(a);

}

void Administrator::serialize(std::ostream& out) const
{
    LibraryPerson::serializeBase(out);
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing PersonUnit!");
    }
    Administrator::serializeAdministratorUnit(out);
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing Administartor!");
    }
}

void Administrator::borrow(LibraryUnit* unit)
{
    //  не позволяваме на администраторите да взимат книги?
    // те нямат като потребителите история и текущо взети книги - така че не би трябвало да могат да взимат
    throw std::runtime_error("Administrators cannot take units!");
}

bool Administrator::returnUnit(int id)
{
    throw std::runtime_error("Admins cannot return units!\n");
}

Administrator* Administrator::createInteractively()
{
    std::string username, password, email;
    int d1, m1, y1; // registration date
    int d2, m2, y2; // last login date

    std::cout << "Enter username (or 'cancel' to abort): ";
    std::getline(std::cin, username);
    if (username == "cancel") return nullptr;

    std::cout << "Enter password (or 'cancel' to abort): ";
    std::getline(std::cin, password);
    if (password == "cancel") return nullptr;

    std::cout << "Enter email (or 'cancel' to abort): ";
    std::getline(std::cin, email);
    if (email == "cancel") return nullptr;

    std::cout << "Enter registration date (day month year): ";
    if (!(std::cin >> d1 >> m1 >> y1)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cerr << "Invalid registration date. Aborting.\n";
        return nullptr;
    }

    std::cout << "Enter last login date (day month year): ";
    if (!(std::cin >> d2 >> m2 >> y2)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cerr << "Invalid last login date. Aborting.\n";
        return nullptr;
    }

    std::cin.ignore(); 

    return new Administrator(username, password, d1, m1, y1, d2, m2, y2, email);
}

Administrator::Administrator() :  LibraryPerson(), email("No_email")
{
}

void Administrator::serializeAdministratorUnit(std::ostream& out) const
{
    // сериализация само на неговите части
    size_t size;
    size = email.size();

    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing down size in Admin!");
    }

    out.write(email.c_str(), size);
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing down email in Admin!");
    }
}

void Administrator::deserializeAdministratorUnit(std::istream& is)
{
    // четем само неговите части
    if (!is.good()) {
        throw std::invalid_argument("Invalid input stream before reading in Administrator!");
    }

    size_t size;
    std::string tempEmail;

    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading size for email!");
    }

    tempEmail.resize(size);
    if (size > 0) {
        is.read(&tempEmail[0], size);
    }
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading email in Admin!");
    }
    if (tempEmail.empty()) {
        throw std::invalid_argument("Email cannot be empty!");
    }
    this->email = std::move(tempEmail);
}


std::ostream& operator<<(std::ostream& out, const Administrator& other)
{
    other.display(out);
    return out;
}
