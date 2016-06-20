
#include "fwzSetup.h"
#include "resource.h"
#include "d3dclass.h"
#include "Memtrack.h"

fwzSettings * setupcfg;

typedef struct {
  int w,h;
} RES;

// if your windows supports more than 4000 reolutions, well, we're screwed :)

#define MAXNUMRES 4096

int nRes = 0;
RES ress[MAXNUMRES];

BOOL CALLBACK DlgFunc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
  switch ( uMsg ) {
    case WM_INITDIALOG: {
      wchar_t s[500];
      int i=0;

      DEVMODE d;
      EnumDisplaySettings(NULL,0,&d);
      while(1) {
        BOOL h = EnumDisplaySettings(NULL,i++,&d);
        if (!h || nRes>MAXNUMRES) break;
        
/*** You can use this following line to select only certain aspect ratios ***/
		if ((d.dmPelsWidth * 9) / 16 != d.dmPelsHeight)
		{
			continue;
		}

		if (d.dmBitsPerPel != setupcfg->scrBPP)
		{
			continue;
		}

		if (!nRes || ress[nRes-1].w != d.dmPelsWidth || ress[nRes-1].h != d.dmPelsHeight) 
		{
          ress[nRes].w = d.dmPelsWidth;
          ress[nRes].h = d.dmPelsHeight;
          nRes++;
          swprintf(s, L"%dx%d\0",d.dmPelsWidth,d.dmPelsHeight);

		  SendDlgItemMessageW(hWnd, IDC_RESOLUTION, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(s));
        }
      }
      
	  for (i = 0; i < nRes; i++)
	  {
		  if (ress[i].w == setupcfg->scrWidth && ress[i].h == setupcfg->scrHeight)
		  {
			  SendDlgItemMessage(hWnd, IDC_RESOLUTION, CB_SETCURSEL, i, 0);
		  }
	  }
      if (!setupcfg->nWindowed) {
        SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_SETCHECK, 1, 1);
        EnableWindow(GetDlgItem(hWnd,IDC_ONTOP),0);
      }
	  if (setupcfg->nVsync)
	  {
		  SendDlgItemMessage(hWnd, IDC_VSYNC, BM_SETCHECK, 1, 1);
	  }
	  if (setupcfg->nMultisample)
	  {
		  SendDlgItemMessage(hWnd, IDC_MULTI, BM_SETCHECK, 1, 1);
	  }
	  if (setupcfg->nDebugOutput)
	  {
		  SendDlgItemMessage(hWnd, IDC_DEBUG_CONSOLE, BM_SETCHECK, 1, 1);
	  }

    } break;
    case WM_COMMAND:
    switch( LOWORD(wParam) ) {
      case IDOK: {
        setupcfg->scrWidth = ress[SendDlgItemMessage(hWnd, IDC_RESOLUTION,		CB_GETCURSEL, 0, 0)].w;
        setupcfg->scrHeight= ress[SendDlgItemMessage(hWnd, IDC_RESOLUTION,		CB_GETCURSEL, 0, 0)].h;
        setupcfg->nWindowed    =!SendDlgItemMessage(hWnd, IDC_FULLSCREEN,		BM_GETCHECK , 0, 0);
        setupcfg->nAlwaysOnTop = SendDlgItemMessage(hWnd, IDC_ONTOP,			BM_GETCHECK , 0, 0);
        setupcfg->nVsync       = SendDlgItemMessage(hWnd, IDC_VSYNC,			BM_GETCHECK , 0, 0);
        setupcfg->nMultisample = SendDlgItemMessage(hWnd, IDC_MULTI,			BM_GETCHECK, 0, 0);
		setupcfg->nDebugOutput = SendDlgItemMessage(hWnd, IDC_DEBUG_CONSOLE,	BM_GETCHECK, 0, 0);
        EndDialog (hWnd, TRUE);
	  } break;
      case IDCANCEL: {
        EndDialog (hWnd, FALSE);
      } break;
      case IDC_FULLSCREEN: {
        /* cake. */
        if (SendDlgItemMessage(hWnd, IDC_FULLSCREEN, BM_GETCHECK , 0, 0)) {
          SendDlgItemMessage(hWnd, IDC_ONTOP, BM_SETCHECK, 0, 0);
          EnableWindow(GetDlgItem(hWnd,IDC_ONTOP),0);
        } else {
          EnableWindow(GetDlgItem(hWnd,IDC_ONTOP),1);
        }
      } break;
	  case IDC_MOTIVATION:
	  {
		  ShellExecute(NULL, L"open", L"https://www.youtube.com/watch?v=93kGeOIhgNw", NULL, NULL, SW_SHOWMAXIMIZED);
		  break;
	  }
    } break;
  }
  return ( WM_INITDIALOG == uMsg ) ? TRUE : FALSE;
}

int OpenSetupDialog(fwzSettings * s) {
  setupcfg = s;
  return DialogBox(s->hInstance,MAKEINTRESOURCE(IDD_SETUP),GetForegroundWindow(),DlgFunc);
}