#ifndef _Graphics_H_
#define _Graphics_H_


#include "d3dclass.h"
#include "Camera.h"
#include "Document.h"
#include "Text.h"
#include "EditBox.h"
#include "ClassTree.h"
#include "Line.h"
/////////////
// GLOBALS //
/////////////

const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	bool Initialize(HWND);
	void Shutdown();
	bool Frame();
	Document *doc;
	ClassTree *objectTree;
	EditBox *edit;

private:
	bool Render();

private:
	D3DClass* m_D3D;
	Camera* m_Camera;
};

#endif