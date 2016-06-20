////////////////////////////////////////////////////////////////////////////////
// Filename: System.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _System_H_
#define _System_H_


///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "Input.h"
#include "Graphics.h"
#include "resource.h"
#include "EventDispatcher.h"
#include "IEventHandler.h"

class System
{
public:
	System();
	System(const System&);
	~System();

	bool Initialize();
	void Shutdown();
	void Run();
	HWND GetHWND();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows();
	void ShutdownWindows();

private:
	wchar_t* m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	Input* m_Input;
	Graphics *m_Graphics;
};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////
static System* ApplicationHandle = 0;


#endif