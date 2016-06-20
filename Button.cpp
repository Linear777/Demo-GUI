#include "Button.h"

//SETTERS
wchar_t* Button::getText()
{

	return NULL;
}
void Button::setText(wchar_t *text)
{


}

//GETTERS

//EVENT HANDLENERS
void Button::EventHandler(const Event &e)
{
	if (e.arg1 >= this->controlPosition.x && e.arg1 <= this->controlPosition.x + this->dimensions.x && e.arg2 >= this->controlPosition.y && e.arg2 <= this->controlPosition.y + this->dimensions.y)
	{
		this->isHovered = true;
	}
	else
	{
		this->isHovered = false;
	}

}

void Button::OnDraw()
{
	if (this->isActive == true)
	{
		return;
	}
	if (this->isHovered == true)
	{
		Rect::getInstance().drawRect(this->controlPosition.x, this->controlPosition.y, this->dimensions.x, this->dimensions.y, this->controlHoveredColor);
	}
	else
	{
		Rect::getInstance().drawRect(this->controlPosition.x, this->controlPosition.y, this->dimensions.x, this->dimensions.y, this->controlColor);
	}

	Text::getInstance().addText(11.0f, this->controlPosition.x + (this->dimensions.x / 2), this->controlPosition.y + (this->dimensions.y / 2), L"asd", 0xffffffff, FW1_CENTER | FW1_VCENTER);
}

void Button::OnUpdate()
{

}

void Button::OnHover()
{

}

void Button::OnDrag()
{

}