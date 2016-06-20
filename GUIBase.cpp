#include "GUIBase.h"

//SETTERS
void GUIBase::setControlName(std::string std)
{
	this->controlName = std;
}

void GUIBase::setControlId(std::string std)
{
	this->controlID = std;
}

void GUIBase::setControlColor(D3DXVECTOR4 color)
{
	this->controlColor = color;
}

void GUIBase::setActive(bool active)
{
	this->isActive = active;
}
void GUIBase::setPos(D3DXVECTOR2 ctrlpos)
{
	this->controlPosition = ctrlpos;
}

void GUIBase::setChild(GUIBase *object) //Child
{
	this->child = object;
}

void GUIBase::setSiblings(GUIBase *object) //Siblings
{
	this->siblings = object;
}

void GUIBase::setPrevSiblings(GUIBase *object)
{
	this->prevSiblings = object;
}

void GUIBase::setParent(GUIBase *object)
{
	this->parent = object;
}

void GUIBase::setControlPosition(D3DXVECTOR2 pos)
{
	this->controlPosition = pos;
}

void GUIBase::setDimensions(D3DXVECTOR2 dim)
{
	this->dimensions = dim;
}

void GUIBase::setOrientation(bool value)
{
	this->orientation = value;
}


//GETTERS
std::string GUIBase::getControlName()
{
	return this->controlName;
}

GUIBase* GUIBase::getChild()
{
	return this->child;
}

GUIBase* GUIBase::getSiblings()
{
	return this->siblings;
}

GUIBase* GUIBase::getPrevSiblings()
{
	return this->prevSiblings;
}

GUIBase* GUIBase::getParent()
{
	return this->parent;
}

D3DXVECTOR4* GUIBase::getControlColor()
{
	return &this->controlColor;
}

D3DXVECTOR2* GUIBase::getDimensions()
{
	return &this->dimensions;
}

D3DXVECTOR2* GUIBase::getControlPosition()
{
	return &this->controlPosition;
}

std::string GUIBase::getControlId()
{
	return this->controlID;
}

bool GUIBase::getOrientation()
{
	return this->orientation;
}