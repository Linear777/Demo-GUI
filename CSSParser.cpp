#include "CSSParser.h"
#include "Memtrack.h"

CSSParser::CSSParser(const char *path)
{
	char Line[500], help[500], Parent[100], ParentBonus[100];
	ParentBonus[0] = '\0';

	file = fopen(path, "r");
	int bracket = 0;
	int preskoc = 0;
	int value = 0;

	while (fgets(Line, 500, file))
	{
		preskoc = 0;
		for (int i = 0; i < strlen(Line); i++)
		{
			if (Line[i]>64)
			{
				preskoc = 1; break;
			}
		}
		if (!preskoc)
		{
			continue;
		}
		sscanf(Line, "%s", help);

		if (help[0] == '{')
		{
			bracket = 1;
			continue;
		}
		if (help[0] == '}')
		{
			currentParent->firstChild = rootParent;
			bracket = 0;
			continue;
		}
		if (!bracket)
		{
			currentParent = new ParentCommand;
			currentParent->firstChild = NULL;
			rootParent = currentParent->firstChild;
			sscanf(Line, "%s %[^{\n]", Parent, ParentBonus);

			if (ParentBonus[0] != '\0')
			{
				strcpy(currentParent->desc, Parent);
				strcpy(currentParent->bonus, ParentBonus);
			}
			else
			{
				if (Parent[strlen(Parent) - 1] == '{') Parent[strlen(Parent) - 1] = '\0';
				strcpy(currentParent->desc, Parent);
				strcpy(currentParent->bonus, "\0");
			}

			ParentBonus[0] = '\0';
			bracket = 1;

			currentParent->nextParent = root;
			root = currentParent;
			continue;
		}
		else
		{
			currentChild = new ChildCommand;

			sscanf(Line, "%[^:]", currentChild->desc);
			sscanf(currentChild->desc, "%s", currentChild->desc);
			sscanf(strchr(Line, ':') + 1, "%[^;]", currentChild->bonus);
			if (currentChild->bonus[0] < 65)
			{
				char *p = NULL;
				int i = 0;
				while ((currentChild->bonus[i] > (int)('9') && currentChild->bonus[i] < 65) || currentChild->bonus[i] < (int)('0'))
				{
					p = &currentChild->bonus[i++];

				}
				if (p!= NULL)
				strcpy(currentChild->bonus, p + 1);
			}

			ParentBonus[0] = '\0';
			currentChild->nextChild = rootParent;
			rootParent = currentChild;

			if (!bracket)
			{
				currentParent->firstChild = rootParent;
			}
		}
	}
	root = currentParent;
	this->sort();
	
}


int CSSParser::getPixels(const char * bonus)
{
	int pom = 0;
	const char *p = NULL;

	if (isPixel(bonus))
	{
		if ((p = strchr(bonus, '+')) != NULL)
		{
			sscanf(p + 1, "%d", &pom);
			return pom;
		}

		sscanf(bonus, "%d", &pom);
	}
	return pom;
}

int CSSParser::getPercents(const char * bonus)
{
	int pom = 0;

	if (isPercent(bonus))
	{
		sscanf(bonus, "%d", &pom);
		return pom;
	}
	return pom;
}

float *CSSParser::getRGBA(const char * bonus)
{
	float *rgbaVector = (float*)malloc(sizeof(float)*4);
	const char *r = NULL, *g = NULL, *b = NULL, *a = NULL;

	if (strstr(bonus, "rgb"))
	{
		r = strchr(bonus, '(') + 1;
		g = strchr(bonus, ',') + 1;
		b = strchr(g, ',') + 1;

		if (!strchr(bonus, '.'))
		{
			int pomFloat = 0;

			sscanf(r, "%d", &pomFloat);
			rgbaVector[0] = (float)(pomFloat) / 255;

			sscanf(g, "%d", &pomFloat);
			rgbaVector[1] = (float)(pomFloat) / 255;

			sscanf(b, "%d", &pomFloat);
			rgbaVector[2] = (float)(pomFloat) / 255;

			if (strchr(bonus, 'a'))
			{
				a = strchr(b, ',') + 1;
				sscanf(a, "%d", &pomFloat);
				rgbaVector[3] = (float)(pomFloat) / 255;

			}
			else
			{
				rgbaVector[3] = 0.0f;
			}
		}
		else
		{
			sscanf(r, "%f", &rgbaVector[0]);
			sscanf(g, "%f", &rgbaVector[1]);
			sscanf(b, "%f", &rgbaVector[2]);

			if (strchr(bonus, 'a'))
			{
				a = strchr(b, ',') + 1;
				sscanf(a, "%f", &rgbaVector[3]);
			}
			else
			{
				rgbaVector[3] = 0.0f;
			}
		}
	}
	

	return rgbaVector;
}


ParentCommand* CSSParser::appendNewID()
{
	ParentCommand *last = this->getLastId();
	ParentCommand* newID = new ParentCommand;

	last->nextParent = newID;
	newID->nextParent = NULL;

	newID->updateBasicElements();
	return newID;
}
bool CSSParser::isPercent(const char * bonus)
{
	if (strchr(bonus, '%'))
	{
		return 1;
	}

	return 0;
}

bool CSSParser::isPixel(const char * bonus)
{
	if (strstr(bonus, "px"))
	{
		return 1;
	}

	return 0;
}

ParentCommand* CSSParser::getMouseAt(int x, int y)
{

	ParentCommand *pom = this->root;
	ChildCommand *pom2 = NULL;

	int tempX = 0;
	int tempY = 0;
	int width = 0;
	int height = 0;

	while (pom != NULL)
	{
		tempX = 0;
		tempY = 0;
	
		pom2 = pom->firstChild;
		while (pom2 != NULL)
		{
			if (!strcmp(pom2->desc, "top"))
			{
				tempY = this->getPixels(pom2->bonus);
			}
			else if (!strcmp(pom2->desc, "left"))
			{
				tempX = this->getPixels(pom2->bonus);
			}
			else if (!strcmp(pom2->desc, "width"))
			{
				width = this->getPixels(pom2->bonus);
			}
			else if (!strcmp(pom2->desc, "height"))
			{
				height = this->getPixels(pom2->bonus);
			}

			pom2 = pom2->nextChild;
		}
		if (x >= tempX && y >= tempY && x <= tempX + width && y <= tempY + height)
		{
			return pom;
		}
		pom = pom->nextParent;
	}
	return NULL;
}
void CSSParser::sort()
{
	ParentCommand *mainPom, *prevPom, *Pom;
	mainPom = prevPom = this->root;
	Pom =  NULL;
	bool end = false;

	while (mainPom != NULL)
	{
		if ((!strcmp(mainPom->bonus, "hover") || !strcmp(mainPom->bonus, "click")))
		{
			Pom = this->root;
			while (Pom != NULL)
			{
				if (!strcmp(mainPom->desc, Pom->desc) && Pom->bonus[0] == '\0')
				{	
					if (mainPom == root)
					{
						mainPom->featureEvent = Pom->featureEvent;
						Pom->featureEvent = mainPom;
						root = mainPom->nextParent;
						break;
					}
					prevPom->nextParent = mainPom->nextParent;
					mainPom->featureEvent = Pom->featureEvent;
					Pom->featureEvent = mainPom;
					
					end = true;
					break;
				}
				Pom = Pom->nextParent;
			}
		}
		if (end)
		{
			mainPom = root;
			end = false;
			continue;
		}
		prevPom = mainPom;
		mainPom = mainPom->nextParent;
	 }
	/*Tento sort priradi kazdemu ID jeho feature a nalinkuje #idcka pekne za sebou
	Do Pointra na feature sa ulozi adressa #id hover,click atd....
	To iste sa spravi s classami a taktiez indiv. classe sa priradi class hover atd..
	potom vezme retazec ktory obsahuje za sebou iduce classy a vytvori z toho jednotny
	strom , teraz mame les a vymazavanie ci linkovanie je pohodlnejsie vdaka lepsiemu
	pristupu a architekture stromov*/
	
	//Teraz vytvori linker za sebou iducich classov,

	ParentCommand *pomRoot = root;
	ParentCommand *prevNode = NULL;
	ParentCommand *classRoot = NULL;
	ParentCommand *toJoin = NULL;
	ParentCommand *toContinue = NULL;
	while (pomRoot != NULL)
	{
		if (pomRoot->desc[0] == '.')
		{
			toContinue = pomRoot->nextParent;
			if (pomRoot == root)
			{
				if (toJoin == NULL)
				{
					toJoin = pomRoot;
					classRoot = pomRoot;
				}
				else
				{
					toJoin->nextParent = pomRoot;
					toJoin = pomRoot;
				}
				root = toContinue;
				pomRoot->nextParent = NULL;
				pomRoot = root;
				continue;
			}
			if (classRoot == NULL)
			{
				classRoot = pomRoot;
			}
			prevNode->nextParent = pomRoot->nextParent;
			pomRoot->nextParent = NULL;
			if (toJoin == NULL)
			{
				toJoin = pomRoot;
			}
			else
			{
				toJoin->nextParent = pomRoot;
				toJoin = pomRoot;
			}
			pomRoot = toContinue;
			continue;
		}
			
		prevNode = pomRoot;
		pomRoot = pomRoot->nextParent;
	}
	this->classRoot = classRoot;
}

ParentCommand *CSSParser::getObjectById(const char* name)
{
	ParentCommand *startingNodeParent = this->root;
	while (startingNodeParent != NULL)
	{
		if (!strcmp(startingNodeParent->desc + 1, name) && startingNodeParent->desc[0] == '#')
		{
			return startingNodeParent;
		}
		startingNodeParent = startingNodeParent->nextParent;
	}
	return NULL;
}
ParentCommand *CSSParser::getLastId()
{
	ParentCommand *pom = this->root;
	if (pom == NULL)
	{
		return NULL;
	}
	while (pom->nextParent != NULL)
	{
		pom = pom->nextParent;
	}
	return pom;
}
ParentCommand *CSSParser::getClass(const char *str)
{
	ParentCommand *pom = this->classRoot;
	while (pom != NULL)
	{
		if (!strcmp(pom->desc +1 , str))
		{
			return pom;
		}
		pom = pom->nextParent;
	}
	return NULL;
}

void CSSParser::printTree(ParentCommand *instance)
{

	ParentCommand *pom = instance;
	ParentCommand *pom2 = NULL;

	while (pom != NULL)
	{
		std::cout << pom->desc <<" "<< pom->bonus << "\n";
		pom2 = pom->featureEvent;
		while (pom2 != NULL)
		{
			std::cout << "+ " << pom2->bonus << "\n";
			pom2 = pom2->featureEvent;
		}	
		std::cout << "---------------------\n";
		pom = pom->nextParent;
	}
}
void CSSParser::setRowId()
{
	sprintf(this->rowCountId,"#%d",++this->counter);
}
void CSSParser::clear()
{
	


	
}