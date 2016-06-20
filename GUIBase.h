#pragma once

#include "IEventHandler.h"
#include <string>
#include "Rect.h"
#include "Text.h"

class GUIBase : IEventHandler
{

public:
	virtual void OnDraw() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnHover() = 0;
	virtual void OnDrag() = 0;

	//setters
	void setActive(bool active);
	void setPos(D3DXVECTOR2 ctrlpos);
	void setChild(GUIBase *object); 
	void setSiblings(GUIBase *object); 
	void setParent(GUIBase *object);
	void setPrevSiblings(GUIBase *object);
	void setControlId(std::string std);
	void setControlColor(D3DXVECTOR4 color);
	void setControlName(std::string std);
	void setControlPosition(D3DXVECTOR2 pos);
	void setDimensions(D3DXVECTOR2 dim);
	void setOrientation(bool value);
	//getters
	std::string getControlId(void);
	std::string  getControlName(void);
	GUIBase *getChild(void);
	GUIBase *getSiblings(void);
	GUIBase *getParent(void);
	GUIBase *getPrevSiblings(void);
	D3DXVECTOR4* getControlColor(void);
	D3DXVECTOR2* getControlPosition(void);
	D3DXVECTOR2* getDimensions(void);
	bool getOrientation(void);

protected:
	D3DXVECTOR2 controlPosition;	//top/left
	D3DXVECTOR2 dimensions;			//width/height
	bool isActive;					//active/deactivated
	bool isHovered;					//check for hover status
	bool orientation;				//Horizontal or Vertical?
	D3DXVECTOR4 controlColor;		//RGBA color
	D3DXVECTOR4 controlSelectedColor;		//selected RGBA color
	D3DXVECTOR4 controlHoveredColor;		//hovered RGBA color
	D3DXVECTOR4 controlClickedColor;		//clicked RGBA color
	std::string controlID;			//ID of the control used for sending messages
	std::string controlName;		//NAME of the element as the same as element type
	GUIBase *parent;				//Parent
	GUIBase *child;					//Child
	GUIBase *siblings;				//Siblings
	GUIBase *prevSiblings;				
};