#include "Rect.h"
#include "pugixml.hpp"
#include "GUIBase.h"
#include "EditBox.h"
#include "Button.h"
#include "CSSParser.h"

//(PugiXML+CSSParser)->ClassTree->RenderTree->paint();

/*This Class is something like PUGI XML Tree but in out version filled by objects
 so branches and nodes are exactly the same but we're appending them objects like a 
 Buttons,EditBoxes,Sliders etc....

 RenderClass is tree where we specify particular render depth for objects so we can handle
 with better performance.
*/
#ifndef ClassTreeMARK
#define ClassTreeMARK
class ClassTree
{
public:
	GUIBase *root = NULL;										//ClassRoot
	ParentCommand *rootCSS = NULL;								//CSSRoot
	pugi::xml_node rootXML;										//XMLRoot (Document)
	CSSParser parse = CSSParser("layout/warlockgui.css"); 		//Access to parser

	int plasticID = 0;											//AID
	std::vector<GUIBase *> allObjects;

	ClassTree();
	void initTree(pugi::xml_node currNode,GUIBase *prevNode);
	void printTree(GUIBase *root,int depth,bool precise);

	//Update from previous branches or add something new if it was changed or doesn't exist
	void updateRelative(GUIBase *current);
	void updateAbsolute(GUIBase *current);
	void updateClass(GUIBase *current);
	void updateId(GUIBase *current);
	void appendValues(GUIBase *current, GUIBase *prevNode, pugi::xml_node &node);
	void renderTree();
	void traverseTree(GUIBase *current, int depth);
	//Format Print
	void depthSpace(int depth);
	void borders(int length);

	//WORKERS
	D3DXVECTOR4 getColorCSS(ChildCommand *attribute);
	int recalculate(ChildCommand *attribute,GUIBase *prevNode,bool horizontal);
	//Identify Element
	GUIBase* elementToObject(std::string std);

	//GETTERS
	std::string getPlasticID(void);
};
#endif