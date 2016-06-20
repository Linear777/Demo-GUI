#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "ChildCommand.h"
#include "ParentCommand.h"


/*CSS to C++ Warlock Engine Parser


|ParentCommand-------------------------|  |ChildCommand-------------------------------|
|								       |  |											  |
|      desc               bonus		   |  |          desc              bonus		  |
| |#nazov,.nazov|  |button,box,atd..|  |  |  |margin,padding,atd.|  |boolean,px,%,|	  |
|______________________________________|  |___________________________________________|


*/

#ifndef CSSParserMARK
#define CSSParserMARK
class CSSParser
{
public:

	FILE *file;
	ParentCommand *root = NULL,*classRoot = NULL, *currentParent = NULL;
	ChildCommand *rootParent = NULL, *currentChild = NULL, *pom = NULL;
	int counter = 0;
	char rowCountId[20]; //Ani za kokot to nepretecie

	ParentCommand *getMouseAt(int x, int y);
	ParentCommand* appendNewID(void);
	ParentCommand* getObjectById(const char * str);
	ParentCommand *getClass(const char *str);
	ParentCommand *getLastId(void);

	CSSParser(const char *path);
	int getPixels(const char * bonus);
	int getPercents(const char * bonus);
	float * getRGBA(const char * bonus);
	bool isPercent(const char * bonus);
	bool isPixel(const char * bonus);
	void printTree(ParentCommand *instance);
	void sort(void);
	void clear(void);
	void setRowId(void);

};
#endif