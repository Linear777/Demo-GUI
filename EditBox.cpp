#include "EditBox.h"
#include "Memtrack.h"

std::wstring EditBox::getText()
{
	return this->text;
}

void EditBox::setChar(wchar_t ch)
{
	cursorPos++;
	std::wstring wstr;
	wstr += ch;
	text.insert(cursorPos, wstr);
	
}

void EditBox::moveCursorRight()
{
	if (cursorPos <= text.length() - 1)
	{
		cursorPos++;
	}
}

void EditBox::moveCursorLeft()
{
	if (cursorPos > 0)
	{
		cursorPos--;
	}
}

void EditBox::deleteChar()
{
	if (text.length() > 0)
	{
		text.pop_back();
		cursorPos--;
	}
}

int EditBox::getCursorPos()
{
	return cursorPos;
}

void EditBox::EventHandler(const Event &e)
{
	switch (e.Type)
	{
		case E_EDITBOX_WRITE_CHAR:
		{
			if (e.arg1 == 8)
			{
				deleteChar(); 
			}
			else
			{
				setChar((wchar_t)e.arg1);
			}
		}
		case E_EDITBOX_MOVE_CURSOR:
		{
			if (e.arg1 == 37)
			{
				moveCursorLeft();
			}
			else if (e.arg1 == 39)
			{
				moveCursorRight();
			}
			break;
		}
	}
}

void EditBox::OnDraw()
{
	Rect::getInstance().drawRect(this->controlPosition.x, this->controlPosition.y, this->dimensions.x, this->dimensions.y, this->controlColor);
}

void EditBox::OnUpdate()
{

}

void EditBox::OnHover()
{

}

void EditBox::OnDrag()
{

}