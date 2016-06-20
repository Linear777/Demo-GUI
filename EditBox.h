#include <string>
#include "GUIBase.h"
#include "IEventHandler.h"

#ifndef EditBoxMARK
#define EditBoxMARK
class EditBox : public GUIBase
{
public:
	std::wstring getText();
	void setChar(wchar_t ch);
	void deleteChar();
	
	void moveCursorLeft();
	void moveCursorRight();
	int getCursorPos();

	void EventHandler(const Event &e);

	void OnDraw();
	void OnUpdate();
	void OnHover();
	void OnDrag();
private:
	std::wstring text;
	signed int cursorPos = -1;
};
#endif