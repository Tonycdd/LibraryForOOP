#include "LibrarySystem.h"
#include <fstream> // работим с двоични файлове
#include <algorithm> // само за std::sort

LibrarySystem::LibrarySystem(const std::string& usersFile, const std::string& unitsFile )
    :fileUsers(usersFile),fileUnits(unitsFile)
{
    // някакви валидации за името?
    if (fileUsers.empty()) {
        throw std::invalid_argument("Invalid name!");
    }
    else if (fileUnits.empty()) {
        throw std::invalid_argument("Invalid name!");
    }
    else if (fileUsers == fileUnits) {
        throw std::invalid_argument("Those names are equal!");
    }
    
    std::ifstream file1(usersFile, std::ios::binary);
    
    if (!file1.is_open()) {
        throw std::ios_base::failure("Error with opening fileUsers!");
    }

    std::ifstream file2(unitsFile, std::ios::binary);
    if (!file2.is_open()) {
            file1.close();
            file2.close();
            throw std::ios_base::failure("Error with opening fileUnits!");
        }   
    bool emptyCase = false;
    try
    {
        deserialize(file1,file2);
        if (infoUnits.empty()) {
            std::cerr << "There is no info loaded about units!\n";
            emptyCase = true;
        }
        if (infoUsers.empty()) {
            std::cerr << "There is no info loaded about users!\n";
            emptyCase = true;
        }
        
        if (emptyCase) {
            // регистрираме по подразбиране един admin user
            Date today = Date(); // днешна дата
            Administrator* initialUser = new Administrator("admin", "i<3C++", today, today, "admin@email.bg");
            currentPerson = initialUser; // важно!
            users.push_back(initialUser);
        }
    }
    catch (...)
    {
        // тук ще ги затворим, ако нещо се счупи
        file1.close();
        file2.close();
        throw std::runtime_error("Somethign went wrong with deserializing in LibrarySystem!");
    }
    file1.close();
    file2.close();
}

LibrarySystem::LibrarySystem(LibrarySystem&& other) noexcept
    :units(std::move(other.units)),users(std::move(other.users))
    ,infoUnits(std::move(other.infoUnits)), infoUsers(std::move(other.infoUsers)),
    fileUsers(std::move(other.fileUsers)), fileUnits(std::move(other.fileUnits))
{
    currentPerson = other.currentPerson;
    other.currentPerson = nullptr;
}

LibrarySystem& LibrarySystem::operator=(LibrarySystem&& other) noexcept
{
    if (this != &other) {
        std::swap(units, other.units);
        std::swap(users, other.users);
        std::swap(fileUsers, other.fileUsers);
        std::swap(fileUnits, other.fileUnits);
        std::swap(infoUnits, other.infoUnits);
        std::swap(infoUsers, other.infoUsers);
        std::swap(currentPerson, other.currentPerson);
    }
    return *this;
}

LibrarySystem::~LibrarySystem()
{
	// правим си наши копия, затова трябва да ги трием тук

	for (size_t i = 0; i < units.size(); i++)
	{
		delete units[i];
	}
	for (size_t i = 0; i < users.size(); i++)
	{
		delete users[i];
	}
    users.clear();
    units.clear();
}

bool LibrarySystem::login(const std::string& username, const std::string& password)
{
    // трябва да проверим дали не е първо в системата
    if (!canAddNewUser(username, password)) {
        std::cerr << "This user is already in ourSystem!" << "\n";
        return false;
    }

    // иначе можем да го добавим
    // трябва да отидем на мястото във файла и да го заредим в системата ни
    size_t filePos = 0;
    for (size_t i = 0; i < infoUsers.size(); i++)
    {
        // намираме го
        if (infoUsers[i].username == username) {
            filePos = infoUsers[i].pos;
            break;
        }
    }

    // тук отваряме отново файла
    std::ifstream file(fileUsers, std::ios::binary | std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Cannot open file!" << "\n";
        return false;
    }
    
    // отиваме на дадената позиция във файла
    file.seekg(filePos, std::ios::beg);
    if (!file.good()) {
        file.close();
        return false;
    }
    TypeOfReader type;
    file.read(reinterpret_cast<char*>(&type), sizeof(type));
    if (!file.good()) {
        file.close();
        std::cerr << "Error with file!" << "\n";
        return false;
    }
    LibraryPerson* newUser = nullptr;
    try
    {
        // може да фейлне
        newUser = LibraryFactory::createPersonFromStream(file, type);
        newUser->lastLoginDate = Date(); // днес текущо време?
    }
    catch (...)
    {
        file.close();
        throw;
    }
    // слагаме го във системата ни?
    users.push_back(newUser);

    currentPerson = newUser; // Важно е!
    
    return true;
}

bool LibrarySystem::logout()
{
    if (!currentPerson) {
        std::cerr << "There is no user in our system!" << "\n";
        return false;
    }

    currentPerson = nullptr;
    return true;
}

void LibrarySystem::findUnits(const std::string& type, 
    const std::string& option, const std::string& value,
    const std::string& sortKey, const std::string& sortOrder,
    int topCount) const
{
    if (currentPerson == nullptr) {
        std::cerr << "We need atelast one active person!" << "\n";
        return;
    }
    std::vector<LibraryUnit*> filtered;

    for (size_t i = 0; i < units.size(); i++)
    {
        // тип - намираме първо правилния тип от колекцията
        if (type == "books" && units[i]->getType() != Type::BOOK)
            continue;
        if (type == "series" && units[i]->getType() != Type::SERIES)
            continue;
        if (type == "newsletters" && units[i]->getType() != Type::PERIODICALS)
            continue;

        // критерий за търсенето 
        if (option == "title")
        {
            if (units[i]->getTitle().find(value) != std::string::npos)
                filtered.push_back(units[i]);
        }

        else if (option == "author")
        {
            // Book и Series  - те имат автор
            // тук ще го направим с каст-за да избегнем проверки
            if (Book* book = dynamic_cast<Book*>(units[i]))
            {
                if (book->getAuthor() == value) // string operator==
                    filtered.push_back(units[i]);
            }

            // Periodicals и Series
            else if (Periodicals* periodical = dynamic_cast<Periodicals*>(units[i]))
            {
                std::vector<Article> artc = periodical->getArticles();
                for (size_t j = 0; j < artc.size(); j++)
                {
                    if (artc[j].author == value) {
                        filtered.push_back(units[i]);
                    }
                }
            }
        }
        
        else if (option == "tag")
        {
            // ключови думи 
            std::vector<std::string> keyWord = units[i]->getKeyWordsForFind();
            for (size_t i = 0; i < keyWord.size(); i++)
            {
                if (keyWord[i] == value) {
                    filtered.push_back(units[i]);
                }
            }
        }
    }
        // сега дали имаме сортиране?
        if (!sortKey.empty()) {
            // значи трябва да сортираме 
            // взаимствано от интернет!
            std::sort(filtered.begin(), filtered.end(), [&](LibraryUnit* a, LibraryUnit* b) {
                if (sortKey == "title")
                return sortOrder != "desc" ? a->getTitle() <= b->getTitle() : a->getTitle() > b->getTitle();
            if (sortKey == "year")
                return sortOrder != "desc" ? a->getDate().getYear() <= b->getDate().getYear() : a->getDate().getYear()> b->getDate().getYear();
            return true;
            });
        }

        // Ограничение top
        size_t limit = (topCount > 0 && !sortKey.empty()) ? std::min<size_t>(topCount, filtered.size()) : filtered.size();

        // Извеждане
        for (size_t i = 0; i < limit; ++i)
        {
            filtered[i]->display();
        }
}

void LibrarySystem::findReaders(const std::string& option, const std::string& value)
{
    if (currentPerson == nullptr) {
        std::cerr << "We need atelast one active person!" << "\n";
        return;
    }
    Date today; // създава дата - която е днешната със std::time
    if (option == "name") {
        for (size_t i = 0; i < users.size(); i++)
        {
            if (users[i]->getUsername() == value && users[i]->getType() == TypeOfReader::READER) {
                users[i]->display();
                return;
            }
        }
        std::cerr << "No user with name: " << value << "\n";
        return;
    }

    else if (option == "ID") {
        for (size_t i = 0; i < users.size(); i++)
        {
            const Reader* isReader = dynamic_cast<Reader*>(units[i]);
            int num = std::stoi(value);
            if (isReader && isReader->hasBorrowedUnit((num)))
            {
                isReader->display();
                continue;
            }
        }
        std::cerr << "No users took unit with id: " << value << "\n";
        return;
    }


    else if (option == "state") {
        if (value == "overdue")
        {
            for (size_t i = 0; i < users.size(); i++)
            {
                const Reader* isReader = dynamic_cast<Reader*>(units[i]);
                if (isReader && isReader->hasOverdueBooks(today)) {
                    isReader->display();
                }
            }
        }

        else if (value == "active") {
            for (size_t i = 0; i < users.size(); i++)
            {
                const Reader* isReader = dynamic_cast<Reader*>(units[i]);
                if (isReader && isReader->borrowedLastMonth(today) > 5) {
                    isReader->display();
                }

            }
        }

        else if(value == "inactive") 
        {
            for (size_t i = 0; i < users.size(); i++)
            {
                const Reader* isReader = dynamic_cast<Reader*>(units[i]);
                if (isReader && isReader->monthsSinceLastBorrow(today) > 3) {
                    isReader->display();
                }
            }
        }
        else {
            std::cerr << "Invalid option for state!" << '\n';
            return;
        }
    }

    else {
        std::cerr << "Invalid option" << "\n";
        return;
    }
}

bool LibrarySystem::serialize(std::ostream& file1, std::ostream& file2)const
{
    if (!file1 || !file2) {
        throw std::invalid_argument("Invalid files!");
    }

    // иначе сериализираме всичките обекти във файл
    size_t size = units.size();
    file2.write(reinterpret_cast<const char*>(&size),sizeof(size));
    if (!file2.good())
    {
        // не се грижим да затваряме файловете ние! 
        throw std::ios_base::failure("Error with ofstream file!");
    }

    for (size_t i = 0; i < size; i++)
    {
        size_t pos = file2.tellp();

        Type type = units[i]->getType();
     
        file2.write(reinterpret_cast<const char*>(&type), sizeof(type));
        if (!file2.good()) {
         
            throw std::ios_base::failure("Error with ofstream file!");
        }

        unsigned int id = units[i]->getUniqueNumber();
        file2.write(reinterpret_cast<const char*>(&id), sizeof(id));
        if (!file2.good()) {
            
            throw std::ios_base::failure("Error with ofstream file!");
        }

        bool isFree = units[i]->isAvailable();
        file2.write(reinterpret_cast<const char*>(&isFree), sizeof(isFree));
        if (!file2.good()) {
            
            throw std::ios_base::failure("Error with ofstream file!");
        }

        units[i]->serialize(file2);
        if (!file2.good()) {
            
            throw std::ios_base::failure("Error with ofstream file!");
        }
    }

    if (!file2.good()) {
      
        throw std::ios_base::failure("Error with ofstream file!");
    }

    size_t sizeOfUsers = users.size();

    file1.write(reinterpret_cast<const char*>(&sizeOfUsers), sizeof(sizeOfUsers));
    if (!file1.good()) {
        throw std::ios_base::failure("Error with ofstream file!");
    }

    for (size_t i = 0; i < sizeOfUsers; i++)
    {
        size_t pos = file1.tellp();

        TypeOfReader type = users[i]->getType();
        file1.write(reinterpret_cast<const char*>(&type), sizeof(type));
        if (!file1.good()) {
            throw std::ios_base::failure("Error with ofstream file!");
        }

        std::string username = users[i]->getUsername();
        size_t len = username.size();

        file1.write(reinterpret_cast<const char*>(&len), sizeof(len));
        if (!file1.good()) {
            throw std::ios_base::failure("Error with ofstream file!");
        }

        file1.write(username.c_str(), len);
        if (!file1.good()) {
            throw std::ios_base::failure("Error with ofstream file!");
        }

        std::string password = users[i]->getPassword(); // затова е friend...
        len = password.size();
        file1.write(reinterpret_cast<const char*>(&len), sizeof(len));
        if (!file1.good()) {
            throw std::ios_base::failure("Error with ofstream file!");
        }

        file1.write(password.c_str(), len);
        if (!file1.good()) {
            throw std::ios_base::failure("Error with ofstream file!");
        }

        users[i]->serialize(file1);
        if (!file1.good()) {
            throw std::ios_base::failure("Error with ofstream file!");
        }
    }if (!file1.good()) {
        throw std::ios_base::failure("Error with ofstream file!");
    }

    return true;
}

bool LibrarySystem::deserialize(std::istream& file1, std::istream& file2)
{
    if (!file1 || !file2) {
        throw std::invalid_argument("Invalid arguments!");
    }
    // от интренет за празен файл
    if ((file2.tellg() == 0 && file2.peek() == std::ifstream::traits_type::eof())
        ||(file1.tellg() == 0 && file1.peek() == std::ifstream::traits_type::eof())) {
        return false;
    }
    size_t unitsCount;
    file2.read(reinterpret_cast<char*>(&unitsCount), sizeof(unitsCount));
    if (!file2) {
        throw std::ios_base::failure("Error with ofstream file!");
    }
    for (size_t i = 0; i < unitsCount; ++i) {
        size_t pos = file2.tellg();  // Записваме текущата позиция

        Type type;
        file2.read(reinterpret_cast<char*>(&type), sizeof(type));
        if (!file2.good()) {
            // не се грижим ние да затваряме файловете!
            throw std::ios_base::failure("Error with ofstream file!");
        }

        unsigned int id;
        file2.read(reinterpret_cast<char*>(&id), sizeof(id));
        if (!file2.good()) {
            throw std::ios_base::failure("Error with ofstream file!");
        }

        bool isFree;
        file2.read(reinterpret_cast<char*>(&isFree), sizeof(isFree));
        if (!file2.good()) {
            throw std::ios_base::failure("Error with ofstream file!");
        }

        // Пропускаме останалата част от обекта
        LibraryUnit* temp = LibraryFactory::createUnitFromStream(file2, type);
        delete temp; // само да минем по обекта, не го пазим

        // те са валидни, тъй като сме създали веднъж обект
        
        UniqueIDAndFilePositions meta{ id, pos, isFree };
        // примерно: this->unitMeta.push_back(meta);
        infoUnits.push_back(meta);
    }

    size_t userCount;
    file1.read(reinterpret_cast<char*>(&userCount), sizeof(userCount));
    if (!file1.good()) {
        throw std::ios_base::failure("Error with ofstream file!");
    }

    for (size_t i = 0; i < userCount; ++i) {
        size_t pos = file1.tellg();

        TypeOfReader type;
        file1.read(reinterpret_cast<char*>(&type), sizeof(type));
        if (!file1.good()) {
            throw std::ios_base::failure("Error with ofstream file!");
        }

        size_t usernameSize;
        file1.read(reinterpret_cast<char*>(&usernameSize), sizeof(usernameSize));
        if (!file1.good()) {
            throw std::ios_base::failure("Error with ofstream file!");
        }

        std::string username(usernameSize, '\0');
        file1.read(&username[0], usernameSize);
        if (!file1.good()) {
            throw std::ios_base::failure("Error with ofstream file!");
        }

        size_t passwordSize;
        file1.read(reinterpret_cast<char*>(&passwordSize), sizeof(passwordSize));
        if (!file1.good()) {
            throw std::ios_base::failure("Error with ofstream file!");
        }

        std::string password(passwordSize, '\0');
        file1.read(&password[0], passwordSize);
        if (!file1.good()) {
            throw std::ios_base::failure("Error with ofstream file!");
        }

        // Пропускаме останалата част от потребителя
       
         LibraryPerson* temp = LibraryFactory::createPersonFromStream(file1, type);
         delete temp;
      

        MetaInfoAboutUsers meta{ username, password, pos };
        // примерно: this->userMeta.push_back(meta);
        infoUsers.push_back(meta);
    }

    return true;
}

void LibrarySystem::addUnit(const Type& type)
{
    if (!currentPerson || currentPerson->getType() != TypeOfReader::ADMINISTRATOR) {
        std::cerr << "Only admins can do that operation!" << "\n";
        return;
    }

    LibraryUnit* newItem = LibraryFactory::createUnitInteractively(type);
    if (newItem == nullptr) {
        std::cerr << "Adding new unit been cancelled!" << "\n";
        return;
    }
    units.push_back(newItem);
    std::cout << "Successfully pushed new Unit in our system!" << "\n";
    return;
}

void LibrarySystem::removeUnit(unsigned int id)
{
    if (!currentPerson || currentPerson->getType() != TypeOfReader::ADMINISTRATOR) {
        std::cerr << "Only admins can do that operation!" << "\n";
        return;
    }

    for (size_t i = 0; i < units.size(); i++)
    {
        if (units[i]->getId() == id) {
            std::swap(units[i], units.back());
            delete units.back();
            units.pop_back();
            // size се намалява и той и нямаме достъп до nullptr
        }
    }

    // махаме ги от записите на читателите
    for (size_t i = 0; i < users.size(); i++)
    {
        Reader* isReader = dynamic_cast<Reader*>(users[i]);
        if (isReader == nullptr) {
            continue;
        }
        // иначе
        std::vector<LibraryUnit*> unit = isReader->getUnits();
        for (size_t j = 0; j < unit.size(); j++)
        {
            if (unit[i]->getId() == id) {
                unit[i] = nullptr;
            }
        }
    }
}

bool LibrarySystem::changeUnit(unsigned int id)
{
    if (!currentPerson || currentPerson->getType() != TypeOfReader::ADMINISTRATOR) {
        std::cerr << "Only admins can do that operation!" << "\n";
        return false;
    }

    for (size_t i = 0; i < units.size(); i++)
    {
        if (units[i]->getId() == id) {
            
            try
            {
                if (!units[i]->change()) {
                    return false;
                }
                return true;
            }
            catch (...)
            {
                std::cerr << "Cannot change current unit!";
                return false;
            }
        }
    }
    std::cerr << "Not found!\n";
    return false;
}

void LibrarySystem::addUser(const std::string& name, const std::string& password, bool isAdmin)
{
    if (!currentPerson || currentPerson->getType() != TypeOfReader::ADMINISTRATOR) {
        std::cerr << "Only admins can do that operation!" << "\n";
        return;
    }
    TypeOfReader type = READER;
    if(isAdmin){
        type = ADMINISTRATOR;
    }

    LibraryPerson* newItem = LibraryFactory::createPersonInteractively(type);
    if (newItem == nullptr) {
        std::cerr << "Adding new unit been cancelled!" << "\n";
        return;
    }
    users.push_back(newItem);
    std::cout << "Successfully pushed new Unit in our system!" << "\n";
    return;
}

void LibrarySystem::removeUser(const std::string& name)
{
    if (!currentPerson || currentPerson->getType() != TypeOfReader::ADMINISTRATOR) {
        std::cerr << "Only admins can do that operation!" << "\n";
        return;
    }

    size_t count = 0;
    for (size_t i = 0; i < users.size(); i++)
    {
        if (users[i]->getType() == ADMINISTRATOR) {
            count++;
        }
    }

    for (size_t i = 0; i < users.size(); i++)
    {
        if (users[i]->getUsername() == name) {
            //  трябва да го махнем
            if (users[i]->getType() == TypeOfReader::ADMINISTRATOR && count == 1) {
                std::cerr << "Cannot remove cause  this is the last Admin in our system!\n";
                return;
            }
            // иначе можем да го махнем
            if (currentPerson->getUsername() == users[i]->getUsername() &&
                currentPerson->getPassword() == users[i]->getPassword())
            {
                if (!this->logout()) {
                    std::cerr << "Something happened!\n";
                    return;
                };

                // иначе е било успешно
                std::cout << "Successfully removed user: " << name << "\n";
                return;
            }
            // иначе не е текущо логнатият
            std::swap(users[i], users.back());
            delete users.back();
            users.pop_back();
            return;
        }
    }
    std::cerr << "Not found!\n";
    return;
}

bool LibrarySystem::changeUser(const std::string& currentUser, const std::string& newPassword, const std::string& targetUser) {

    LibraryPerson* executor = findUserByUsername(currentUser);
    if (!executor) return false;

    // Без аргумент: текущият потребител сменя своята парола
    if (targetUser.empty()) {
        return executor->setPassword(newPassword);
    }

    // С аргумент: админ сменя паролата на друг потребител
    if (executor->getType() != TypeOfReader::ADMINISTRATOR) return false;  // Само админ може

    LibraryPerson* target = findUserByUsername(targetUser);
    if (!target) return false;

    return target->setPassword(newPassword);
}

bool LibrarySystem::userFind(const std::string& option, const std::string& optionString) const
{
    
    if (!currentPerson || currentPerson->getType() != ADMINISTRATOR) {
        std::cerr << "No active user or active but not administrator!\n";
        return false;
    }

    if (option == "name") {
        LibraryPerson* user = findUserByUsername(optionString);
        if (user == nullptr) {
            std::cerr << "Not found!";
            return false;
        }
        // иначе е намерен
        user->display();
        return true;
    }
    Date today = Date();
    std::vector<LibraryPerson*> filtered;

    if (option == "id") {
        unsigned int id = std::stoul(optionString);
        for (size_t i = 0; i < users.size(); i++)
        {
            
            // можем и да добавим някаква фукнция, но за простота ще го направим сега така
            Reader* hasToBeReader = dynamic_cast<Reader*>(users[i]);
            if (hasToBeReader == nullptr) {
                continue; // само читателите заемат книги
            }

            std::vector<LibraryUnit*> takenFromReader = hasToBeReader->getUnits();
            for (size_t j = 0; j < takenFromReader.size(); j++)
            {
                if (takenFromReader[j]->getId() == id) {
                    filtered.push_back(users[i]);
                }
            }
        }

        for (size_t i = 0; i < filtered.size(); i++)
        {
            filtered[i]->display();
        }
    }
    
    else if (option == "state" && (optionString == "overdue" || optionString == "reader" || optionString == "inactive")) 
    {
        if (optionString == "overdue") {
            for (size_t i = 0; i < users.size(); i++)
            {
                Reader* hasToBeReader = dynamic_cast<Reader*>(users[i]);
                if (hasToBeReader == nullptr) {
                    continue;
                }
                // книгите които не е върнал в срок
                if (hasToBeReader->hasOverdueBooks(today)) {
                    filtered.push_back(users[i]);
                }
            }
        }
        else if (optionString == "reader") {
            for (size_t i = 0; i < users.size(); i++)
            {
                Reader* hasToBeReader = dynamic_cast<Reader*>(users[i]);
                if (hasToBeReader == nullptr) {
                    continue;
                }
                 // дали е взел повече от 5 книги
                unsigned int count = hasToBeReader->borrowedLastMonth(today);
                if (count > 5) {
                    filtered.push_back(users[i]);
                }
            }
        }
        else if (optionString == "inactive")
        {
            for (size_t i = 0; i < users.size(); i++)
            {
                Reader* hasToBeReader = dynamic_cast<Reader*>(users[i]);
                if (hasToBeReader == nullptr) {
                    continue;
                }
                // да не е вземал книги от повече от 3 месеца
                if (hasToBeReader->monthsSinceLastBorrow(today) > 3) {
                    filtered.push_back(users[i]);
                }
            }
        }

        for (size_t i = 0; i < filtered.size(); i++)
        {
            filtered[i]->display();
            
        }
        return true;
    }

    else {
        throw std::invalid_argument("Invalid option for usersFind!\n");
    }

}

bool LibrarySystem::take(const std::string& user, unsigned int id)
{
    if (currentPerson == nullptr || currentPerson->getUsername() != user) {
        std::cerr << "We need a current user logged in our system!\n";
        return false;
    }

    LibraryUnit* unit = findUnitById(id);

    if (currentPerson->getType() == ADMINISTRATOR) {
        std::cerr << "Administrators cannot take units!\n";
        return false;
    }

    if (!unit)
        return false;

    if (!unit->isAvailable())
        return false;

    if (unit->incrementTakenCopies()) {
        currentPerson->borrow(unit);
        return true;
    }

    return false;
}

bool LibrarySystem::returnUnit(unsigned int id)
{
    if (currentPerson == nullptr || currentPerson->getType() == ADMINISTRATOR) {
        std::cerr << "We need a user logged in our system to perform this return operation! \nAdmins cannot perform it!\n";
        return false;
    }
    
    if (!currentPerson->returnUnit(id)) {
        return false;
    };

    return true;
}

void LibrarySystem::booksAll(std::ostream& out) const
{
    if (currentPerson == nullptr) {
        std::cerr << "We need atelast one active person!" << "\n";
        return;
    }
    for (size_t i = 0; i < units.size(); i++)
    {
        if (units[i]->getType() == Type::BOOK) {
            units[i]->printBase();
        }
    }
}

void LibrarySystem::periodicalsAll(std::ostream& out) const
{
    if (currentPerson == nullptr) {
        std::cerr << "We need atelast one active person!" << "\n";
        return;
    }
    for (size_t i = 0; i < units.size(); i++)
    {
        if (units[i]->getType() == Type::PERIODICALS) {
            units[i]->printBase();
        }
    }
}

void LibrarySystem::seriesAll(std::ostream& out) const
{
    if (currentPerson == nullptr) {
        std::cerr << "We need atelast one active person!" << "\n";
        return;
    }
    for (size_t i = 0; i < units.size(); i++)
    {
        if (units[i]->getType() == Type::SERIES) {
            units[i]->printBase();
        }
    }
}

void LibrarySystem::all(std::ostream& out) const
{
    if (currentPerson == nullptr) {
        std::cerr << "We need atelast one active person!" << "\n";
        return;
    }
    for (size_t i = 0; i < units.size(); i++)
    {
        units[i]->printBase(); // само базовата част - някава малка информация
    }
}

void LibrarySystem::listInfo(const std::string& ISNN_OR_ISBN) const
{
    if (currentPerson == nullptr) {
        std::cerr << "We need atelast one active person!" << "\n";
        return;
    }
    // полиморфично поведение
    for (size_t i = 0; i < units.size(); i++)
    {
        std::vector<std::string> ident = units[i]->getIdentifiers(); // не копира тежки обекти, те са най -много с два елемента 
        for (size_t j = 0; j  < ident.size(); j ++)
        {
            if (ident[i] == ISNN_OR_ISBN) {
                units[i]->display(); // по-подробна информация
                return;
            }
        }
    }
    std::cerr << "Item was not found!" << "\n";
    return;
}

bool LibrarySystem::canAddNewUser(const std::string& name, const std::string& password) const
{
    // дали можем да добавин нов потребител
    // ако е в системата - не
    // ако има несъответствие с паролата - пак не

    // ако е добавен вече
    for (size_t i = 0; i < users.size(); i++)
    {
        // operator== of string
        if (users[i]->getUsername() == name) {
            return false;
        }
    }

    // ако ли пък не е добавен
    for (size_t i = 0; i < infoUsers.size(); i++)
    {
        if(infoUsers[i].username == name && infoUsers[i].password == password) 
        {
            return true;
        }
    }
    return false;
}

LibraryPerson* LibrarySystem::findUserByUsername(const std::string& n) const
{
    for (size_t i = 0; i < users.size(); i++)
    {
        if (users[i]->getUsername() == n) {
            return users[i];
        }
    }
    return nullptr;
}

LibraryUnit* LibrarySystem::findUnitById(unsigned int id) const
{
    for (size_t i = 0; i < units.size(); i++)
    {
        if (units[i]->getId() == id) { return units[i]; };
    }
    return nullptr;
}
