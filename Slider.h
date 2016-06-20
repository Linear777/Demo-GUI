#include "GUIBase.h"
class Slider : public GUIBase
{
public:
	void OnDraw();
	void OnUpdate();
	void OnHover();
	void OnDrag();

private:
	D3DXVECTOR2 sliderPos;
	D3DXVECTOR2 boundaries; // min/max
	float value;

};