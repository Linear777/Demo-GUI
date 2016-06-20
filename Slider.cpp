#include "Slider.h"


void Slider::OnDraw()
{
	Rect::getInstance().drawRect(this->controlPosition.x, this->controlPosition.y, this->dimensions.x, this->dimensions.y, this->controlColor);
	//TODO: Finish slider ondraw effect

}

void Slider::OnUpdate()
{

}

void Slider::OnHover()
{

}

void Slider::OnDrag()
{

}