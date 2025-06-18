#include "LibraryPerson.h"

LibraryPerson::LibraryPerson(const std::string& name,
    const std::string& password,
    int day1, int month1, int year1,
    int day2, int month2, int year2)
    :username(name),password(password), registerDate(day1, month1, year1), lastLoginDate(day2, month2, year2)
{
    // ако имаме някакъв проблем с registerDate, lastLoginDate, нямаме проблем
    // сега тук потенциално всичко е минало инициализацията
    // валидираме нещата 
    if (!validateUsername(this->username)) {
        throw std::invalid_argument("Username is not in a valid form!");
    }
    else if (!validatePassword(this->password)) {
        throw std::invalid_argument("Password is not in a valid form!");
    }
}

LibraryPerson::LibraryPerson(const LibraryPerson& other)
    :username(other.username), password(other.password),registerDate(other.registerDate),lastLoginDate(other.lastLoginDate)
{
    // other  е валиден обект, а ние тепърва създаваме this
    // нямаме какво друго да правим тук, просто ги копираме, като викаме copy ctor на данните
}

LibraryPerson& LibraryPerson::operator=(const LibraryPerson& other)
{
    
    if (this != &other) {
        // правим дълбоко копиране, тъй като това е абстрактен клас не можем да ползваме ctor
        // може да има и по-елегантно решение, ние го правим с копиране поелементно, което не е най-доброто решение
        try
        {
        std::string copyUsername = other.username; // копиращ ctor на string
        std::string copyPassword = other.password;
        Date copyRegister = other.registerDate;
        Date copyLastLogin = other.lastLoginDate;

        // ако тук всичко е успяло, то сме ок
        // всеки клас си се грижи самостоятелно за своите данни, така че се възползваме от този факт
        username = std::move(copyUsername); // използваме move operator=  от стринг
        password = std::move(copyPassword);
        registerDate = copyRegister;
        lastLoginDate = copyLastLogin;
        }
        catch (...)
        {
            // ако някое от копиранията се счупи, не сме променили нашият обект
            throw;
        }

    }
    return *this;
}

LibraryPerson::LibraryPerson(LibraryPerson&& other) noexcept
    :username(std::move(other.username)), password(std::move(other.password))
{
    std::swap(this->registerDate, other.registerDate);
    std::swap(this->lastLoginDate, other.lastLoginDate);
}


LibraryPerson& LibraryPerson::operator=(LibraryPerson&& other) noexcept
{
    if (this != &other) {
        // не унищожаваме нищо, външният свят може да удължи живота на other
        std::swap(username, other.username);
        std::swap(password, other.password);
        std::swap(registerDate, other.registerDate);
        std::swap(lastLoginDate, other.lastLoginDate);
    }
    return *this;
}

LibraryPerson::~LibraryPerson() noexcept
{
    // нямаме работа тук
}

bool LibraryPerson::setPassword(const std::string& newPas)
{
    if (newPas.empty()) {
        std::cout << "NewTitle cannot be empty!\n";
        return false;
    }

    if (newPas == password) {
        // няма промяна
        return true;
    }

    try
    {
        password = newPas; // може да хвърли но е ок

    }
    catch (...)
    {
        return false;
    }
    return true;
}

bool LibraryPerson::setNewUsername(const std::string& newUsername)
{
    if (newUsername.empty()) {
        std::cout << "NewTitle cannot be empty!\n";
        return false;
    }

    if (newUsername == username) {
        // няма промяна
        return true;
    }

    try
    {
        username = newUsername; // може да хвърли но е ок

    }
    catch (...)
    {
        return false;
    }
    return true;
}

/*
Валидация за Парола :
Изисквания(стандарт за силна парола, напр.Google, GitHub, Microsoft) :

    Мин. 6 символа

    Поне 1 главна буква

    Поне 1 малка буква

    Поне 1 цифра

    Поне 1 специален символ(напр. !@#$ % ^&*()_ += -)
*/

bool LibraryPerson::validatePassword(const std::string& pass) noexcept
{
    if (pass.length() < 6)
        return false;

    bool hasLower = false;
    bool hasUpper = false;
    bool hasDigit = false;
    bool hasSpecial = false;

    for (char c : pass)
    {
        if (std::islower(c)) hasLower = true;
        else if (std::isupper(c)) hasUpper = true;
        else if (std::isdigit(c)) hasDigit = true;
        else hasSpecial = true; 
    }

    return hasLower && hasUpper && hasDigit && hasSpecial;
}

/*
Валидация за Username :
Изисквания(пример) :

    Между 4 и 20 символа

    Само букви, цифри, долна черта(_) и точка(.)

    Не започва или завършва с точка или долна черта

    Без последователни точки или долни черти(напр. .., __, ._ и т.н.)
*/

bool LibraryPerson::validateUsername(const std::string& name) noexcept
{
    // Проверка за дължина
    if (name.length() < 4 || name.length() > 20)
        return false;

    // Не трябва да започва или завършва с точка
    if (name.front() == '.' || name.back() == '.')
        return false;

    // Позволени символи: букви, цифри и точки
    for (size_t i = 0; i < name.length(); ++i) {
        char c = name[i];
        if (!(std::isalnum(c) || c == '.'))
            return false;

        // Проверка за две последователни точки
        if (i > 0 && name[i] == '.' && name[i - 1] == '.')
            return false;
    }

    return true;
}

void LibraryPerson::display(std::ostream& out) const
{
    out << "Username: " << username << "\n";
    out << "Password: ";
    // естествено крием паролата по някакъв начин
    for (size_t i = 0; i < password.size(); i++)
    {
        out << "*";
    }
    out << "\n";
    out << "Register date: ";
    registerDate.print();
    out << "Last login: ";
    lastLoginDate.print();
}

LibraryPerson::LibraryPerson() : username("No_username"), password("No_password"),
    registerDate(), lastLoginDate()
{
}

void LibraryPerson::serializeBase(std::ostream& out)const
{
    // очакваме os да е отворен правилно в бинарен режим
    // не правим тук проверки за това работата е само да пишем и да гарантираме валидност на всяка операция от писането
    // серелизация на базовата част

    // username
    size_t len;
    len = username.size();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (!out.good()) {
        throw std::ios_base::failure("Error writing size of username!");
    }
    out.write(username.c_str(), len);
    if (!out.good()) {
        throw std::ios_base::failure("Error writing username!");
    }

    // password
    len = password.size();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (!out.good()) {
        throw std::ios_base::failure("Error writing size of password!");
    }
    out.write(password.c_str(), len);
    if (!out.good()) {
        throw std::ios_base::failure("Error writing password!");
    }

    // registerDate
    registerDate.serialize(out);
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing registerDate!");
    }

    // lastLoginDate
    lastLoginDate.serialize(out);
    if (!out.good()) {
        throw std::ios_base::failure("Error with writing lastLoginDate!");
    }
}

void LibraryPerson::deserializeBase(std::istream& is)
{
    if (!is.good()) {
        throw std::invalid_argument("Invalid stream before deserializingBase of user in Library!");
    }

    // username
    std::string tempUsername, tempPassword;
    Date tempRegisterDate, tempLastLoginDate;

    size_t size;
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading size of username!");
    }
    tempUsername.resize(size);
    if (size > 0) {
        is.read(&tempUsername[0], size);
    }
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading username!");
    }

    // password
    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading size of password!");
    }
    tempPassword.resize(size);
    if (size > 0) {
        is.read(&tempPassword[0], size);
    }
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading password!");
    }

    // registerDate
    tempRegisterDate.deserialize(is);
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading registerDate");
    }

    // lastLoginDate
    tempLastLoginDate.deserialize(is);
    if (!is.good()) {
        throw std::ios_base::failure("Error with reading lastLoginDate!");
    }

    // някаква валидация
    if (tempUsername.empty()) {
        throw std::invalid_argument("Username cannot be emtpy!");
    }
    if (tempPassword.empty()) {
        throw std::invalid_argument("Username cannot be emtpy!");
    }
    if (!tempRegisterDate.validateDay(tempRegisterDate.getDay(), tempRegisterDate.getMonth())) {
        throw std::invalid_argument("Invalid registerDate!");
    }
    if (!tempLastLoginDate.validateDay(tempLastLoginDate.getDay(), tempLastLoginDate.getMonth())) {
        throw std::invalid_argument("Invalid lastLoginDate!");
    }

    this->username = std::move(tempUsername);
    this->password = std::move(tempPassword);
    this->registerDate = tempRegisterDate;
    this->lastLoginDate = tempLastLoginDate;


}

std::ostream& operator<<(std::ostream& out, const LibraryPerson& person)
{
    person.display(out);
    return out;
}
