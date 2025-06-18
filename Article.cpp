#include "Article.h"
#include <exception>
#include <stdexcept>
#include <iostream>

Article::Article() : author("No_author"), title("No_title"), keyWords()
{
}

Article::Article(const std::string& author,
		const std::string& title,
		const std::vector<std::string>& v)
		: author(author), title(title), keyWords(v) {
		if (author.empty()) {
			throw std::invalid_argument("Article must have an author!");
		}
		else if (title.empty()) {
			throw std::invalid_argument("Article must have an article!");
		}
}
	

	// мутатор за добавяне на нови ключови думи
void Article::addKeyWords(const std::string& key) {
		// първо проверяваме дали вече е добавена в нашата колекция
		for (size_t i = 0; i < keyWords.size(); i++)
		{
			// опретор= на std::string
			if (keyWords[i] == key) {
				std::cout << "This key words is already in our collection of key words!";
				return;
			}
		}
		// иначе е нова дума
		keyWords.push_back(key);
}

	// за сравняване на Articles 
bool Article::operator==(const Article& other)const {
		// сравняваме заглавия и автор
		if (this->author == other.author && this->title == other.title) {
			return true;
		}
		return false;
}

void Article::print(std::ostream& out) const
{
	// някакво по-различно принтиране, за да правим разликите
	out << "author = (" << author << "), title = (" << title << "), key words = (";
	for (size_t i = 0; i < keyWords.size(); i++)
	{
		out << keyWords[i];
		if (i + 1 < keyWords.size()) {
			out << ", ";
		}
	}
	out << ")\n"; 
}

void Article::serialize(std::ostream& os) const
{
	// тази функция е различна смислово от print
	// работим с двоичен файл
	size_t len;
	len = author.size();
	os.write(reinterpret_cast<const char*>(&len), sizeof(len));
	if (os.fail()) {
		throw std::ios_base::failure("Error writing size of author!");
	}
	os.write(author.c_str(), len);
	if (os.fail()) {
		throw std::ios_base::failure("Error writing author!");
	}

	len = title.size();
	os.write(reinterpret_cast<const char*>(&len), sizeof(len));
	if (os.fail()) {
		throw std::ios_base::failure("Error writing size of title!");
	}
	os.write(title.c_str(), len);
	if (os.fail()) {
		throw std::ios_base::failure("Error writing title!");
	}

	len = keyWords.size();
	os.write(reinterpret_cast<const char*>(&len), sizeof(len));
	if (os.fail()) {
		throw std::ios_base::failure("Error writing size of keyWords!");
	}
	size_t keyWordSize = keyWords.size();
	for (size_t i = 0; i < keyWordSize; i++)
	{
		len = keyWords[i].size();
		os.write(reinterpret_cast<const char*>(&len), sizeof(len));
		if (os.fail()) {
			throw std::ios_base::failure("Error writing size of one of the keyWords!");
		}
		os.write(keyWords[i].c_str(), len);
		if (os.fail()) {
			throw std::ios_base::failure("Error writing one of the keyWords!");
		}
	}
}

void Article::deserialize(std::istream& is)
{
	if (!is.good()) {
		throw std::invalid_argument("Invalid input stream in article!");
	}

	size_t size;
	std::string tempAuthor, tempTitle;
	std::vector<std::string> tempKeyWords;

	// author
	is.read(reinterpret_cast<char*>(&size), sizeof(size));
	
	if (is.fail()) {
		throw std::ios_base::failure("Error with reading size of article author!");
	}
	tempAuthor.resize(size);
	if (size > 0) {
		is.read(&tempAuthor[0], size);
		if (is.fail()) {
			throw std::ios_base::failure("Error with reading author of article!");
		}
	}

	// title
	is.read(reinterpret_cast<char*>(&size), sizeof(size));

	if (is.fail()) {
		throw std::ios_base::failure("Error with reading size of title of an article!");
	}
	tempTitle.resize(size);
	if (size > 0) {
		is.read(&tempTitle[0], size);
		if (is.fail()) {
			throw std::ios_base::failure("Error with reading title of an article!");
		}
	}

	// keyWords
	is.read(reinterpret_cast<char*>(&size), sizeof(size));
	if (is.fail()) {
		throw std::ios_base::failure("Error with reading size of keyWords of an author!");
	}
	tempKeyWords.reserve(size); // size = 0 унищожава цялото съдържание
	size_t keyWordsSize = size;

	for (size_t i = 0; i < keyWordsSize ; i++)
	{
		is.read(reinterpret_cast<char*>(&size), size);
		if (is.fail()) {
			throw std::ios_base::failure("Error reading current keyWords size!");
		}
		std::string keyWord(size, '\0');
		is.read(&keyWord[0], size);
		if (is.fail()) {
			throw std::ios_base::failure("Error reading current keyWord!");
		}
		tempKeyWords.push_back(std::move(keyWord));
	}


	// някаква базова валидация
	if (tempTitle.empty()) {
		throw std::invalid_argument("Title cannot be empty!");
	}
	if (tempAuthor.empty()) {
		throw std::invalid_argument("Author cannot be empty!");
	}
	this->author = std::move(tempAuthor);
	this->title = std::move(tempTitle);
	this->keyWords = std::move(tempKeyWords);

}
