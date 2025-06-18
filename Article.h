#pragma once
#include <string>
#include <vector>
#include <iostream>

// структура за статия
// plain - struct не наследява нищо, за да не се усложнява ненужно
struct Article
{
	std::string author;
	std::string title;
	std::vector<std::string> keyWords;

	Article();
	Article(const std::string& author,
		const std::string& title,
		const std::vector<std::string>& v = {});
	// останалото го оставяме за компилатора, тъй като неговата имплементация ще свърши работа

	// мутатор за добавяне на нови ключови думи
	void addKeyWords(const std::string& key);

	// за сравняване на Articles 
	bool operator==(const Article& other)const;
	void print(std::ostream& out = std::cout)const;
	void serialize(std::ostream& os)const;
	void deserialize(std::istream& is);
};
