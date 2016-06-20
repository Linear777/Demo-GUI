#include "pugixml.hpp"
#include "CSSParser.h"
#include <vector>
#include "Rect.h"
#include <random>
#include "Settings.h"
#include "IEventHandler.h"
#include "Text.h"
#include "ClassTree.h"

enum elementtype
{
	elem_box,			//div	--	done
	elem_button,		// done
	elem_slider,
	elem_swapchainpanel, //3D View
	elem_listbox,
	elem_listitem,
	elem_label,			//done
	elem_tab,
};


class Document : IEventHandler
{
public:

	std::string path;
	pugi::xml_document doc;

	struct simple_walker : pugi::xml_tree_walker
	{
		//document object model
		int x, y;
		ParentCommand *id = NULL;
		ParentCommand *oldId = NULL;
		std::string name;
		elementtype type;
		bool refresh;
		bool update;

		
		CSSParser parse = CSSParser("layout/warlockgui.css"); //Parser
		ClassTree *ObjectTree = new ClassTree(); //ClassTree
		void updateByEvent()
		{
			if ((oldId != NULL) && oldId->isFeature("hover"))
			{
				ParentCommand *pom = oldId->featureEvent;
				while (pom != NULL)
				{
					if (!strcmp(pom->bonus, "hover"))
					{
						ChildCommand *child = pom->firstChild;
						while (child != NULL)
						{
							char p[100];
							ChildCommand * childMain = oldId->getChild(child->desc);
							strcpy(p, childMain->bonus);
							strcpy(childMain->bonus, child->bonus);
							strcpy(child->bonus, p);
							child = child->nextChild;
						}
						break;
					}
					pom = pom->featureEvent;
				}
			}
			oldId = id;
			//Ak neobsahuje feature tak ho preskoc
			if (id == NULL)
			{
				return;
			}
			ParentCommand *pom = id->featureEvent;
			while (pom != NULL)
			{
				if (!strcmp(pom->bonus, "hover"))
				{
					update = true;
					ChildCommand *child = pom->firstChild;
					while (child != NULL)
					{
						char p[100];
						ChildCommand * childMain = id->getChild(child->desc);
						strcpy(p, childMain->bonus);
						strcpy(childMain->bonus, child->bonus);
						strcpy(child->bonus, p);
						child = child->nextChild;
					}
					break;
				}
				pom = pom->featureEvent;
			}
			//Obnov stare do povodneho stavu
			refresh = true;
		}
		void updateInterElements(pugi::xml_node &node, ParentCommand *object)
		{

			bool vertical = true;
			bool position = false;
			ChildCommand *pom = object->firstChild;
			pugi::xml_node& prevNode = node.previous_sibling();
			pugi::xml_node& prevParent = node.parent();

			//Coordinates of Parent Node X,Y in pixels
			int parentX = prevParent.attribute("left").as_int();
			int parentY = prevParent.attribute("top").as_int();

			//Coordinates of Parent Node WIDTH,HEIGHT in pixels 
			int parentWidth = prevParent.attribute("width").as_int();
			int parentHeight = prevParent.attribute("height").as_int();
			
			if (object->isFilled("position"))
			{
				position = true;
			}

			if (!strcmp(prevParent.attribute("orientation").as_string(), "horizontal"))
			{
				vertical = false;
			}

			if (prevParent.name() == "")
			{
				parentWidth = Settings::getInstance().getScreenWidth();
				parentHeight = Settings::getInstance().getScreenHeight();
			}
			while (pom != NULL)
			{		
				if (!strcmp(node.attribute(pom->desc).value(), pom->bonus))
				{		
					pom = pom->nextChild;
					continue;
				}
				char pole[20];
				if (!strcmp(pom->desc, "width"))
				{

						sprintf(pole, "%dpx", (pom->getPixels() + (int)(((float)(pom->getPercents()) / 100) * parentWidth)));
						strcpy(pom->bonus, pole);			
				}
				else if (!strcmp(pom->desc, "height"))
				{
					sprintf(pole, "%dpx", (pom->getPixels() + (int)(((float)(pom->getPercents()) / 100) * parentHeight)));
					strcpy(pom->bonus, pole);
				}
				else if (!strcmp(pom->desc, "top"))
				{
					//Pozri ci jeho x+width nezasahuje
					int nodeY = 0;

					if (!position) //Relativna
					{
						int nodeY = (pom->getPixels() + (int)(((float)(pom->getPercents()) / 100) * parentHeight));
						int pomP = nodeY;

						nodeY += parentY;

						int prevNodeY = parse.getPixels(prevNode.attribute("top").value());
						int prevNodeH = parse.getPixels(prevNode.attribute("height").value());

						int offsetY = prevNodeY + prevNodeH;
						int presah = offsetY - nodeY;

						if (presah >= 0)
						{
							// ak mame napriklad 100px tak musime pripocitat presah a potom zase do stringu dat
							// do stringu dame vo formate = x%+ypx plati aj pre px alebo % rovne nule
							// teda aj 10%+0px je spravne
							//zatial to je len v pixeloch pripocitane
							if (vertical)
							{
								sprintf(pole, "%dpx", offsetY + pomP);
								strcpy(pom->bonus, pole);
							}
							else
							{
								sprintf(pole, "%dpx", nodeY);
								strcpy(pom->bonus, pole);
							}
						}
						else
						{
							sprintf(pole, "%dpx", nodeY);
							strcpy(pom->bonus, pole);
						}
					}
					else //Absolutna
					{
						int nodeY = (pom->getPixels() + (int)((float)(pom->getPercents()) / 100 * Settings::getInstance().getScreenHeight()));
						sprintf(pole, "%dpx", nodeY);
						strcpy(pom->bonus, pole);

					}
				}
				else if (!strcmp(pom->desc, "left"))
				{
					//Pozri ci jeho x+width nezasahuje
					int nodeX = 0;

					if (!position) //Relativna
					{
						int nodeX = (pom->getPixels() + (int)(((float)(pom->getPercents()) / 100) * parentWidth));

						int pomP = nodeX;

						nodeX += parentX;

						int prevNodeX = parse.getPixels(prevNode.attribute("left").value());
						int prevNodeW = parse.getPixels(prevNode.attribute("width").value());

						int offsetX = prevNodeX + prevNodeW;
						int presah = offsetX - nodeX;

						if (presah >= 0)
						{
							// ak mame napriklad 100px tak musime pripocitat presah a potom zase do stringu dat
							// do stringu dame vo formate = x%+ypx plati aj pre px alebo % rovne nule
							// teda aj 10%+0px je spravne
							//zatial to je len v pixeloch pripocitane
							if (!vertical){

								sprintf(pole, "%dpx", offsetX + pomP);
								strcpy(pom->bonus, pole);
							}
							else
							{
								sprintf(pole, "%dpx", nodeX);
								strcpy(pom->bonus, pole);
							}
						}
						else
						{
							sprintf(pole, "%dpx", nodeX);
							strcpy(pom->bonus, pole);
						}
					}
					else //Absolutna
					{
						int nodeX = (pom->getPixels() + (int)((float)(pom->getPercents()) / 100 * Settings::getInstance().getScreenWidth()));
						sprintf(pole, "%dpx", nodeX);
						strcpy(pom->bonus, pole);
					}
				}
				if (!node.attribute(pom->desc).empty())
				{			
						node.attribute(pom->desc).set_value(pom->bonus);
				}
				else
				{
						node.append_attribute(pom->desc).set_value(pom->bonus);
				}
				pom = pom->nextChild;
			}
		}


		void initNode(pugi::xml_node& node)
		{
			//Pre konkretny XML pugi node vrati konkretny CSS parser root;
			//Zmyslom initNode funkcie je prehladat CSS parser tree a priradit hodnoty do XML pugiho
			//Tato funkcia by sa mala vykonavat len v pritomnosti Event Handlerov 
			ParentCommand *pomm = parse.getObjectById(node.attribute("id").as_string());

			if (pomm == NULL)
			{
				if (node.attribute("class").empty() == false)
				{
					parse.setRowId();
					pomm = new ParentCommand;
					strcpy(pomm->desc, parse.rowCountId);
					ParentCommand * pomoc = parse.root->nextParent;
					parse.root->nextParent = pomm;
					pomm->nextParent = pomoc;
					pomm->updateBasicElements();
					node.append_attribute("id").set_value(parse.rowCountId+1);
				}
				else
				{
					return;
				}
				
			}
			if (node.attribute("class").empty() == false)
			{
				ParentCommand *getClass = parse.getClass(node.attribute("class").as_string());
				pomm->parentAppendClass(getClass);		
				node.remove_attribute("class");		
			}
			pomm->updateBasicElements();
			updateInterElements(node, pomm);		
		}

		virtual bool for_each(pugi::xml_node& node)
		{
			if (refresh)
			{
				initNode(node);
			}
			render(node);
			
			return true; // continue traversal
		}

		void render(pugi::xml_node& node)
		{
			int x = node.attribute("left").as_int();
			int y = node.attribute("top").as_int();

			int width = node.attribute("width").as_int();
			int height = node.attribute("height").as_int();
		
			float *color = parse.getRGBA(node.attribute("background-color").as_string());
			

			Rect::getInstance().drawRect(x, y, width, height, D3DXVECTOR4(color[0], color[1], color[2], color[3]));
			
			delete color;
			
		}
	};

	simple_walker walker;
	void constructDom(std::string layoutfile);
	void render();
	void renderButtonText(ID3D11DeviceContext *devcon);

	void EventHandler(const Event &e)
	{
		if (e.Type == E_MOUSEMOVE)
		{
			//Dat do povodneho stavu swapnut hodno
			walker.id = walker.parse.getMouseAt(e.arg1, e.arg2);
		
			if (walker.oldId == walker.id)
			{
				// V rovnakom stvorci
				return;
			}
				walker.updateByEvent();
		}
		if (e.Type == E_RELOADGUI)
		{
			//cleanup();
			//walker.parse = CSSParser("layout/warlockgui.css");
			//constructDom("warlockgui");
			//walker.refresh = true;
			walker.parse.printTree(walker.parse.root);
		}
	}

	void cleanup()
	{
		walker.parse.clear();
		doc.reset();
	}

	struct button_walker : pugi::xml_tree_walker
	{
		ID3D11DeviceContext *devcon;

		virtual bool for_each(pugi::xml_node& node)
		{
			std::string but = "button"; 
			std::string lbl = "label";
			std::string litm = "listitem";

			if (but.compare(node.name()) == 0 || lbl.compare(node.name()) == 0 || litm.compare(node.name()) == 0)
			{
				if (node.attribute("text").empty())
				{
					return true;
				}

				float x = node.attribute("left").as_int() + (node.attribute("width").as_int() / 2);
				float y = node.attribute("top").as_int() + (node.attribute("height").as_int() / 2);
				
				UINT32 color = 0xffffffff;
				
				int fontSize = 12;

				if (!node.attribute("font-size").empty())
				{
					fontSize = node.attribute("font-size").as_int();
				}

				if (!node.attribute("font-color").empty())
				{
					//TODO: make font color from CSS
					
					color = Text::getInstance().createRGBA(255, 0, 0, 255); 
				}			

				//Text::getInstance().drawText(fontSize, x, y, getWchar(node.attribute("text").as_string()), color, FW1_CENTER | FW1_VCENTER);
			}
			return true; // continue traversal
		}

		wchar_t *getWchar(const char *c)
		{
			const size_t cSize = strlen(c) + 1;
			wchar_t* wc = new wchar_t[cSize];
			mbstowcs(wc, c, cSize);
			return wc;
		}
	};


};

