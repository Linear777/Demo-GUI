#include "ClassTree.h"
#include <iostream>
#include "Settings.h"


/*
TODO Render in depth
*/

ClassTree::ClassTree()
{
	//Setting up Root
	this->root = new EditBox();
	this->root->setSiblings(NULL);
	this->root->setChild(NULL);
	this->root->setControlName("BODY");
	this->root->setDimensions(D3DXVECTOR2(Settings::getInstance().getScreenWidth(),
										  Settings::getInstance().getScreenHeight()));
	//Join CSS
	this->rootCSS = parse.root;
	
	//Join XML
	pugi::xml_document doc;
	doc.load_file("layout/warlockgui.xml");
	this->rootXML = doc.child("body");					  //Everything must be in <body> tag
	
	initTree(this->rootXML,this->root);
}

std::string ClassTree::getPlasticID()
{
	return std::to_string(++(this->plasticID));
}

void ClassTree::depthSpace(int depth)
{
	for (int i = 0; i < depth; i++)
	{
		std::cout << "   ";
	}
}

void ClassTree::borders(int length)
{
	depthSpace(length);
	for (int i = 0; i < 10; i++)
	{
		std::cout << "-";
	}
	std::cout << std::endl;

}

//I didn't like recursion. But now it's our jewel;
void ClassTree::initTree(pugi::xml_node currNode,GUIBase *prevNode)
{
	bool first = false;
	GUIBase *newNode = NULL;
	GUIBase *siblings = NULL;

	for (pugi::xml_node::iterator it = currNode.begin(); it != currNode.end(); it++)
	{
		//[init of current Object
		newNode = elementToObject(it->name());
		//]
		if (first == false)
		{
			prevNode->setChild(newNode);
			first = true;
		}
		//appendValues(newNode, prevNode, *it);

		if (siblings != NULL)
		{
			newNode->setPrevSiblings(siblings);
			siblings->setSiblings(newNode);
		}
		
		appendValues(newNode, prevNode, *it);

		initTree(*it, newNode);
		siblings = newNode;
	}
}





void ClassTree::appendValues(GUIBase *current,GUIBase *prevNode, pugi::xml_node &node)
{
	/*This method initialize whole ClassTree for better visualisation and performance
	It finds specific ID from XMLTree(PUGI) and connect with Branch in CSSTree key is ID;
	If ID doesn't exist we must create it because we need to know the difference between many of them
	If CLASS exists we must append it to ID attributes as good as features
	17:54 16.2.2015 -Last Update
	09:58 
	*/


	const char *id = node.attribute("id").as_string();				//Get ID string from XML
	const char *Class = node.attribute("class").as_string();		//Get CLASS string from XML
	bool orientation = prevNode->getOrientation();
	//[Initialization of NAME and ID
	current->setControlName(node.name());							//Set Name for Object
	current->setParent(prevNode);									//Set Parent for Object
	
	
	ParentCommand *particularID = parse.getObjectById(id);			//Get ID-Node from CSSTree
	
	
			//Else set ID from XML
	
	if (id[0] == '\0')												//If ID doesn't exist
	{
		updateId(current);
		if (particularID == NULL)
		{
			if (Class[0] != '\0')									//If CLASS occured in XML
			{
				particularID = parse.getClass(Class);				//Append CLASS to ID-Node
			}
			else
			{
			particularID = parse.appendNewID();	
			}//Append new ID-Node to CSSTree
		}
	}
	else
	{
		if (particularID == NULL)
		{
			particularID = parse.appendNewID();
		}
		current->setControlId(node.attribute("id").as_string());
		if (Class[0] != '\0')											//If CLASS occured in XML
		{
			particularID->parentAppendClass(parse.getClass(Class));		//Append CLASS to ID-Node
		}
	}
	GUIBase *parent = prevNode;											//Object->Parent
	GUIBase *prevSiblings = current->getPrevSiblings();					//Object->PreviousSiblings

	if (prevSiblings == NULL)
	{
		prevSiblings = prevNode;
	}

	particularID->updateBasicElements();
	//[Calculation from CSS , after that we need to add previous position of prevSiblings
	int CSSx = recalculate(particularID->getChild("left"),prevNode,true);	
	int CSSy = recalculate(particularID->getChild("top"), prevNode,false);

	int CSSwidth = recalculate(particularID->getChild("width"), prevNode,true);
	int CSSheight = recalculate(particularID->getChild("height"), prevNode,false);


	ChildCommand *command = particularID->getChild("orientation");

	
		if (command != NULL)
		{
			if (!strcmp("vertical", command->bonus))
			{
				current->setOrientation(true);
			}
			else 
			{
				current->setOrientation(false);
			}
			//Many else ifs many orientations
		}
		//[Add previous relative positioning + yours
		D3DXVECTOR2 *position = prevSiblings->getControlPosition(); //Get pos from prevSibling
		D3DXVECTOR2 *dimensions = prevSiblings->getDimensions();	//Get dim from prevSibling
		
		if (prevSiblings == prevNode)
		{
			CSSx += position->x;
			CSSy += position->y;
		}
		else
		{
			if (orientation == false)
			{
				CSSx += position->x + dimensions->x;
				CSSy += prevNode->getControlPosition()->y;
			}
			else
			{
				CSSx += prevNode->getControlPosition()->x;
				CSSy += position->y + dimensions->y;
			}
			
		}
	


	D3DXVECTOR4 color = getColorCSS(particularID->getChild("background-color"));
	current->setControlColor(color);								//Color
	current->setControlPosition(D3DXVECTOR2(CSSx,CSSy));			//Position
	current->setDimensions(D3DXVECTOR2(CSSwidth, CSSheight));		//Dimensions

	this->allObjects.push_back(current);
}


D3DXVECTOR4 ClassTree::getColorCSS(ChildCommand *attribute)
{
	float *color = attribute->getRGBA();

	return D3DXVECTOR4(color[0], color[1], color[2], color[3]);
}

int ClassTree::recalculate(ChildCommand *attribute,GUIBase *prevNode,bool horizontal)
{
	if (attribute == NULL)
	{
		return 0;
	}

	int percent = attribute->getPercents();
	int pixels = attribute->getPixels();
	int screenResolution;

	if (horizontal == true)
	{
		screenResolution = prevNode->getDimensions()->x;
	}
	else
	{
		screenResolution = prevNode->getDimensions()->y;
	}

	percent = (int)((float)(percent)*screenResolution) / 100;

	return pixels+percent;
}


void ClassTree::renderTree()
{
	
	for (std::vector<GUIBase *>::iterator it = allObjects.begin(); it != allObjects.end(); it++)
	{
		/*int x = (*it)->getControlPosition()->x;
		int y = (*it)->getControlPosition()->y;
		int width = (*it)->getDimensions()->x;
		int height = (*it)->getDimensions()->y;

		Rect::getInstance().drawRect(x, y, width, height, *((*it)->getControlColor()));*/
		
		(*it)->OnDraw();
		
	}

}
void ClassTree::printTree(GUIBase *root,int depth,bool precise)
{
	if (root == NULL)
	{
		return;
	}

		depthSpace(depth);
	
		std::cout << "<" << root->getControlName() << " id = '" << root->getControlId() << "'/>\n";
	
		if (precise == true)
		{
			int k = 0;
			borders(depth);
			
			depthSpace(depth);	std::cout << "Left :" << root->getControlPosition()->x << "\n";
			depthSpace(depth);	std::cout << "Top :" << root->getControlPosition()->y << "\n";
			depthSpace(depth);	std::cout << "Width :" << root->getDimensions()->x << "\n";
			depthSpace(depth);	std::cout << "Height :" << root->getDimensions()->y << "\n";
			depthSpace(depth);  std::cout << "Color :" << root->getControlColor()->x;
								std::cout << "," << root->getControlColor()->y;
								std::cout << "," << root->getControlColor()->z;
								std::cout << "," << root->getControlColor()->w << "\n";
			borders(depth);
		}
	std::cout << std::endl;

	printTree(root->getChild(), depth + 1,precise);
	printTree(root->getSiblings(), depth,precise);

}

//IDENTIFY METHODS

GUIBase* ClassTree::elementToObject(std::string std)
{
	
	if (std.compare("box") == 0)
	{
		return new EditBox();	//aww dude srsly?
	}
	else if (std.compare("button") == 0)
	{
		return new Button();
	}
	else if (std.compare("slider") == 0)
	{
		//return new Slider();
	}
	else if (std.compare("tabbar") == 0)
	{
		//return new Tabbar();
	}
	//so on.. We need to add new Objects to compare and improve this method because it isn't enum
}

//UPDATE METHODS

void ClassTree::updateRelative(GUIBase *current)
{
	

}

void ClassTree::updateAbsolute(GUIBase *current)
{

}

void ClassTree::updateClass(GUIBase *current)
{

}

void ClassTree::updateId(GUIBase *current)
{
	current->setControlId(this->getPlasticID());
}