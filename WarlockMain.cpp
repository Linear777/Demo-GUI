#include "Memtrack.h"
#include <stdlib.h>
#include "System.h"
#include <io.h>
#include <fcntl.h>
#include <shellapi.h>
#include "Settings.h"
#include "fwzSetup.h"

#pragma warning(disable: 4005)

void setupConsole()
{
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	fwzSettings setup;
	setup.hInstance = hInstance;
	setup.scrBPP = 32;
	setup.nVsync = 1;
	setup.nMultisample = 0;
#ifdef _DEBUG
	setup.nAlwaysOnTop = 0;
	setup.scrWidth = 1280;
	setup.scrHeight = 720;
	setup.nWindowed = 1;
	setup.nDebugOutput = 1;
	
	if (!OpenSetupDialog(&setup))
	{
		return -1;
	}

	if (setup.nDebugOutput == 1)
	{
		setupConsole();
	}

#else
	setup.nAlwaysOnTop = 0;
	setup.scrWidth = 1280;
	setup.scrHeight = 720;
	setup.nWindowed = 0;
	setup.nDebugOutput = 0;

	if (!OpenSetupDialog(&setup))
	{
		return -1;
	}

	if (setup.nDebugOutput == 1)
	{
		setupConsole();
	}
#endif

	Settings::getInstance().setScreenWidth(setup.scrWidth);
	Settings::getInstance().setScreenHeight(setup.scrHeight);
	Settings::getInstance().setVsync(setup.nVsync == 1);
	Settings::getInstance().setFullscreen(setup.nWindowed == 0);

	System* system;
	bool result;


	// Create the system object.
	system = new System;
	if (!system)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = system->Initialize();
	if (result)
	{
		system->Run();
		
	}

	// Shutdown and release the system object.
	system->Shutdown();
	delete system;
	system = 0;

#ifdef _DEBUG
	dumpmem();
#endif
	return 0;
}