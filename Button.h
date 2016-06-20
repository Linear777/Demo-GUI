#include "GUIBase.h"


#ifndef ButtonMARK
#define ButtonMARK
class Button : public GUIBase
{
public:
	wchar_t *getText();
	void setText(wchar_t *text);

	void EventHandler(const Event &e);

	void OnDraw();
	void OnUpdate();
	void OnHover();
	void OnDrag();
private:
	wchar_t *text;
};
#endif