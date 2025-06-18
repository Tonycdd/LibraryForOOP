#pragma once
#include "LibraryPerson.h"
#include "LibraryUnit.h"
#include <iostream>


class LibraryFactory
{
public:
	static LibraryPerson* createPersonFromStream(std::istream& is,const TypeOfReader& type);
	static LibraryUnit* createUnitFromStream(std::istream& is,const Type& type);
	static LibraryUnit* createUnitInteractively(const Type& T);
	static LibraryPerson* createPersonInteractively(const TypeOfReader& T);

};
