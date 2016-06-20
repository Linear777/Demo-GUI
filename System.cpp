////////////////////////////////////////////////////////////////////////////////
// Filename: System.cpp
////////////////////////////////////////////////////////////////////////////////
#include "System.h"
#include "Memtrack.h"

System::System()
{
	m_Input = 0;
	m_Graphics = 0;
}


System::System(const System& other)
{
}


System::~System()
{
}


bool System::Initialize()
{
	bool result;


	// Initialize the windows api.
	InitializeWindows();

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new Input;
	if (!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	m_Input->Initialize();

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new Graphics;
	if (!m_Graphics)
	{
		return false;
	}

	// Initialize the graphics object.
	result = m_Graphics->Initialize(m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}


void System::Shutdown()
{
	// Release the graphics object.
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// Shutdown the window.
	ShutdownWindows();

	return;
}


void System::Run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}

	return;
}


bool System::Frame()
{
	bool result;


	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Do the frame processing for the graphics object.
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

HWND System::GetHWND()
{
	return m_hwnd;
}


LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_CHAR:
	{
		EventDispatcher::Get()->SendEvent(E_EDITBOX_WRITE_CHAR, LOWORD(wparam), 0);
		break;
	}
		// Check if a key has been pressed on the keyboard.
	case WM_KEYDOWN:
	{
		// If a key is pressed send it to the input object so it can record that state.
		m_Input->KeyDown((unsigned int)wparam);

		EventDispatcher::Get()->SendEvent(E_EDITBOX_MOVE_CURSOR, LOWORD(wparam), 0);

		printf("KeyDown: %d\n\n", wparam);
		if (wparam == 116)
		{
			EventDispatcher::Get()->SendEvent(E_RELOADGUI, LOWORD(lparam), HIWORD(lparam));
		}
		return 0;
	}

	// Check if a key has been released on the keyboard.
	case WM_KEYUP:
	{
		// If a key is released then send it to the input object so it can unset the state for that key.
		m_Input->KeyUp((unsigned int)wparam);
		//printf("KeyUp: %d\n\n", wparam);
		return 0;
	}

	case WM_MOUSEMOVE:
	{
		int x = LOWORD(lparam);
		int y = HIWORD(lparam);
		//printf("MouseX: %d\n", x);
		//printf("MouseY: %d\n\n", y);
		EventDispatcher::Get()->SendEvent(E_MOUSEMOVE, LOWORD(lparam), HIWORD(lparam));
		break;
	}

	case WM_LBUTTONDOWN:
	{
		EventDispatcher::Get()->SendEvent(E_MOUSELEFTBUTTONPRESS, LOWORD(lparam), HIWORD(lparam));
		break;
	}

	case WM_LBUTTONUP:
	{
		EventDispatcher::Get()->SendEvent(E_MOUSELEFTBUTTONRELEASE, LOWORD(lparam), HIWORD(lparam));
		break;
	}
	case WM_SIZE:
	{
		EventDispatcher::Get()->SendEvent(E_WNDRESIZE, LOWORD(lparam), HIWORD(lparam));
		break;
	}

	// Any other messages send to the default message handler as our application won't make use of them.
	default:
	{
		return DefWindowProcW(hwnd, umsg, wparam, lparam);
	}
	}
}


void System::InitializeWindows()
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	#define WSTR2( s ) L##s
	#define WSTR( s ) WSTR2( s )

	m_applicationName = L"Warlock DemoTool V0.02 (Build Date: " WSTR(__DATE__) L" - " WSTR(__TIME__) L")";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (Settings::getInstance().isFullscreen())
	{
		// If full screen set the screen to mxposimum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 1366x768 resolution.
		screenWidth = Settings::getInstance().getScreenWidth();
		screenHeight = Settings::getInstance().getScreenHeight();

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	RECT WindowRect;
	WindowRect.left = 0;
	WindowRect.right = screenWidth;
	WindowRect.top = 0;
	WindowRect.bottom = screenHeight;

	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED | WS_MINIMIZEBOX;
	AdjustWindowRect(&WindowRect, dwStyle, FALSE);

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(
		NULL,//WS_EX_APPWINDOW, 
		m_applicationName, 
		m_applicationName,
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, //WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, 
		posY, 
		WindowRect.right - WindowRect.left, 
		WindowRect.bottom - WindowRect.top,
		NULL, 
		NULL, 
		m_hinstance, 
		NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(true);

	return;
}


void System::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (Settings::getInstance().isFullscreen())
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_CREATE:
	{
		HANDLE hIcon = LoadImageA(NULL, "images/favicon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
		if (hIcon)
		{
			SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		}
		else
		{
			MessageBox(hwnd, L"Could not load large icon!", L"Error", MB_OK | MB_ICONERROR);
		}
		return 0;
	}
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		EventDispatcher::Get()->ResetInstance();
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}