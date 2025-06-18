#pragma once
#include "LibraryUnit.h"
#include <vector> // позволено е и ще го ползваме за ключови думи,
// той има добре дефинирани функции за сортиране, търсене и тн.

class Book : public virtual LibraryUnit // искаме да избегнем диамантения проблем при множественото наследяване
{
public:
	// голяма 6-тица
	
	explicit Book(std::istream& is)
		: LibraryUnit(is)
	{
		deserializeBookUnit(is);
	}

	Book(const std::string& title,
		const std::string& publishers,
		const std::string& description,
		int day, int month, int year,
		const Rate& rate,
		const Genre& genre,
		const std::string& author,
		const std::string& isbn = "",
		const std::vector<std::string>& v = {}); // параметризиран конструктор
	Book(const Book& other);
	Book& operator= (const Book& other);
	Book(Book&& other)noexcept;
	Book& operator=(Book&& other)noexcept;
	virtual ~Book()noexcept override;

	// гетъри
	// връщаме по референция да избегнем копиране и е конст, за да не позволява промяна от външния свят
	inline const std::string& getAuthor()const { return author; };
	inline const std::string& getISBN()const { return ISBN; };
	inline const std::vector<std::string>& getKeyWords()const { return keyWords; };
	
	// сетъри
	bool setNewAuthor(const std::string& newAth);
	bool setNewKeyWords(const std::vector<std::string>& newKeyWords);
	bool setNewISBN(const std::string& newISBN);


	// имплементираме чисто-виртуалните методи
	// извикваме копи ctor 
	// всеки от наследниците знае какъв е 
	inline virtual LibraryUnit* clone() const override { return new Book(*this); };
	inline virtual void display(std::ostream& os = std::cout)const override;
	inline virtual size_t getCount()const override { return counter; };
	inline static size_t getBookCount(){ return counter; };
	
	virtual void serialize(std::ostream& os)const override;
	virtual void deserialize(std::istream& is) override;
	inline virtual Type getType()const override { return Type::BOOK; };
	
	// за list info в системата
	inline virtual std::vector<std::string> getIdentifiers()const override {
		return {getISBN()}; 
	}

	// функции за добавяне на ключови думи
	void addKeyWords(const std::string& key);

	virtual std::vector<std::string> getKeyWordsForFind()const override { return keyWords; };

	// статичен метод за интерактивен режим за фабриката
	static Book* createInteractively();

	virtual bool change()override;
protected:

	Book();
	bool changeBookPart();
protected:
	// сериализира само неговите полета и десериaлизацияя
	void serializeBookUnit(std::ostream& os)const;
	void deserializeBookUnit(std::istream& is);

	void printBook(std::ostream& out = std::cout)const;
private:
	std::string author;
	std::vector<std::string> keyWords;
	std::string ISBN; // не е задължителен
	// останалите неща идват от базовата единица

	static size_t counter; // брои броят създадени книги
};

std::ostream& operator<<(std::ostream& out, const Book& book);
