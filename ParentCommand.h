#pragma once

class ParentCommand
{
public:
	char desc[100];
	char bonus[100];
	struct ChildCommand *firstChild = NULL;
	struct ParentCommand *nextParent = NULL;
	struct ParentCommand *featureEvent = NULL;


	ChildCommand * getChild(const char* str)
	{
		if (this == NULL)
		{
			return NULL;
		}
		ChildCommand* pom = this->firstChild;

		while (pom != NULL)
		{
			if (!strcmp(str, pom->desc))
			{
				return pom;
			}
			pom = pom->nextChild;
		}
		return NULL;
	}

	bool isFilled(char *str)
	{
		ChildCommand *pom = this->firstChild;
		while (pom != NULL)
		{
			if (!strcmp(str, pom->desc))
			{
				return true;
			}
			pom = pom->nextChild;
		}
		return false;
	}


	bool isFeature(char *str)
	{
		ParentCommand *feature = this->featureEvent;

		while (feature != NULL)
		{
			if (!strcmp(feature->bonus, str))
			{
				return true;
			}
			feature = feature->featureEvent;
		}
		return false;
	}

	ChildCommand *getLastChild()
	{
		if (this == NULL)
		{
			return NULL;
		}
		ChildCommand *pom = this->firstChild;
		if (pom == NULL)
		{
			return NULL;
		}
		while (pom->nextChild != NULL)
		{	
			pom = pom->nextChild;
		}
		return pom;
	}
	void cleanAttributesFrom(ChildCommand *child)
	{
		ChildCommand *pom;
		while (child != NULL)
		{
			pom = child;
			child = child->nextChild;
			delete pom;
		}
	}
	void parentAppendClass(ParentCommand *Class)
	{
		if (Class == NULL)
		{
			return;
		}
		//Ak ID nema ziadne features tak bude mat take ako CLASS
		if (Class->bonus[0] == '\0')
		{
			this->parentAppendAttributes(Class);
		}
		if (this->featureEvent == NULL)
		{
			if (Class->featureEvent == NULL && Class->bonus[0] != '\0')
			{
				this->featureEvent = Class;
			}
			else
			{
				this->featureEvent = Class->featureEvent;
			}
			return;
		}


		//Semka treba pozerat co ma ID feature a CLASS feature ID > CLASS pozor na to
		ParentCommand *traversalId = this->featureEvent;
		ParentCommand *traversalClass = Class->featureEvent;
		bool found;
		if (traversalClass == NULL)
		{
			traversalClass = Class;
		}
		while (traversalClass != NULL)
		{
			found = false; //nenasli sme zatial ziadny ID = CLASS
			traversalId = this->featureEvent;
			while (traversalId != NULL)
			{
				if (!strcmp(traversalId->bonus, traversalClass->bonus))
				{
					parentAppendFeature(traversalId,traversalClass);
					found = true;
					break;
				}
				traversalId = traversalId->featureEvent;
			}
			if (found == false)
			{
				//Musime pridat feature do ID z CLASS
				ParentCommand *newFeature = this->getLastFeature();
				newFeature->featureEvent = new ParentCommand; // ukazuje na novy branch
				newFeature->featureEvent->featureEvent = NULL;
				this->parentAppendFeature(newFeature->featureEvent, traversalClass);
			}
			traversalClass = traversalClass->featureEvent;
		}
	}

	void parentAppendFeature(ParentCommand *featureId, ParentCommand *featureClass)
	{
		ChildCommand *p = featureId->getLastChild();
		ChildCommand *parentTraverse = featureClass->firstChild;
		ChildCommand *newChild = NULL;
	
		while (parentTraverse != NULL)
		{
			if (featureId->isFilled(parentTraverse->desc) == false)
			{
				newChild = new ChildCommand;
				strcpy(newChild->desc, parentTraverse->desc);
				strcpy(newChild->bonus, parentTraverse->bonus);
				if (p == NULL)
				{
					featureId->firstChild = newChild;
					p = featureId->firstChild;
					p->nextChild = NULL;
				}
				else
				{
					p->nextChild = newChild;
					p = newChild;
					p->nextChild = NULL;
				}
				
			}
			parentTraverse = parentTraverse->nextChild;
		}
	}

	void parentAppendAttributes(ParentCommand *parent)
	{
		ChildCommand *p = this->getLastChild();
		ChildCommand *parentTraverse = parent->firstChild;
		ChildCommand *newChild = NULL;

		while (parentTraverse != NULL)
		{
			if (this->isFilled(parentTraverse->desc) == false)
			{
			newChild = new ChildCommand;
			strcpy(newChild->desc, parentTraverse->desc);
			strcpy(newChild->bonus, parentTraverse->bonus);
			if (p == NULL)
			{
				this->firstChild = newChild;
				p = this->firstChild;
				p->nextChild = NULL;
			}
			else
			{
				p->nextChild = newChild;
				p = newChild;
				p->nextChild = NULL;
			}
			}
			parentTraverse = parentTraverse->nextChild;
		}
	}

	void updateBasicElements()
	{
		if (this == NULL)
		{
			return;
		}
		ChildCommand *pom = this->firstChild;
		
		if (this->isFilled("top") == false)
		{
			ChildCommand *newNode = new ChildCommand;
			strcpy(newNode->desc, "top");
			strcpy(newNode->bonus, "0px");
			newNode->nextChild = pom;
			pom = newNode;

		}
		if (this->isFilled("width") == false)
		{
			ChildCommand *newNode = new ChildCommand;
			strcpy(newNode->desc, "width");
			strcpy(newNode->bonus, "50px");
			newNode->nextChild = pom;
			pom = newNode;

		}
		if (this->isFilled("height") == false)
		{
			ChildCommand *newNode = new ChildCommand;
			strcpy(newNode->desc, "height");
			strcpy(newNode->bonus, "50px");
			newNode->nextChild = pom;
			pom = newNode;

		}
		if (this->isFilled("left") == false)
		{
			ChildCommand *newNode = new ChildCommand;
			strcpy(newNode->desc, "left");
			strcpy(newNode->bonus, "0px");
			newNode->nextChild = pom;
			pom = newNode;

		}
		if (this->isFilled("background-color") == false)
		{
			ChildCommand *newNode = new ChildCommand;
			strcpy(newNode->desc, "background-color");
			strcpy(newNode->bonus, "rgb(223,124,454)");
			newNode->nextChild = pom;
			pom = newNode;

		}
		
		this->firstChild = pom;
	}


	void printAttributes()
	{
		ChildCommand *pom = this->firstChild;

		while (pom != NULL)
		{
			std::cout << pom->desc << " : " << pom->bonus << "\n";
			pom = pom->nextChild;
		}
		return;
	}
	void printAttributes(ParentCommand *f)
	{
		ChildCommand *pom = f->firstChild;

		while (pom != NULL)
		{
			std::cout << pom->desc << " : " << pom->bonus << "\n";
			pom = pom->nextChild;
		}
		return;
	}
	void printFeatures()
	{
		ParentCommand *pom = this->featureEvent;
		while (pom != NULL)
		{
			std::cout << "------------\n" << pom->desc << " : " << pom->bonus << "\n";
			printAttributes(pom);
			std::cout << "------------\n";
			pom = pom->featureEvent;
		}
		return;
	}
	ParentCommand *getLastFeature()
	{
		ParentCommand *pom = this->featureEvent;
		if (pom == NULL)
		{
			return NULL;
		}
		while (pom->featureEvent != NULL)
		{
			pom = pom->featureEvent;
		}
		return pom;
	}
	
};