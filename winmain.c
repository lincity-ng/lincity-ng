/* ---------------------------------------------------------------------- *
 * winmain.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <memory.h>
#include "lcstring.h"
#include "lin-city.h"
#include "common.h"
#include "generic.h"
#include "mouse.h"
#include "lchelp.h"
#include "module_buttons.h"

// Define this to use DIB's instead of DDB's
// GCS: WIN32_USEDIB will no longer work, since I didn't update the 
// window resizing code for DIB's.
//#define WIN32_USEDIB

// Global Variables
static char szClassNameWithMenu[] = APPNAME " (with menu)";
static char szClassNameWithoutMenu[] = APPNAME " (without menu)";
static char szAppName[] = APPNAME;	// The name of this application
static char szTitle[] = APPNAME;	// The title bar text
static HBRUSH hbrBackground = 0;

int pending_mouse_event = 0;
int pending_mouse_x = 0;
int pending_mouse_y = 0;

int pending_resize_event = 0;
int pending_resize_w = 0;
int pending_resize_h = 0;

// Private Function Prototypes
static ATOM MyRegisterClass (CONST WNDCLASS *);
static BOOL InitApplication (HINSTANCE);
static BOOL InitInstance (HINSTANCE, int);
static LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK About (HWND, UINT, WPARAM, LPARAM);
static BOOL CenterWindow (HWND, HWND);
static void InitializeBackingStore (HWND);
static BOOL CopyBackingStoreToScreen (HDC, HWND, LPPAINTSTRUCT);
static void CreateDDB (HWND hWnd);
static void CreateDIB (void);
static void InitializePalette (void);
static void DoSquareMouse (HDC hdc);
static void ResizeBackingStore (HWND hWnd);
static void ResizeDDB (HWND hWnd);


//----------------------------------------------------------------------------
//  FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)
//
//  PURPOSE: Entry point for the application.
//
//  COMMENTS:
//
//      This function initializes the application and processes the
//      message loop.
//----------------------------------------------------------------------------
int WINAPI
WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    char *args[] = {"wlincity"};
  
    if (!hPrevInstance) {
	// Perform instance initialization:
	if (!InitApplication (hInstance)) {
	    return (FALSE);
	}
    }
    
    // Perform application initialization, including creating the
    // Lin-City main window.
    if (!InitInstance (hInstance, nCmdShow)) {
	return (FALSE);
    }

    // Load keyboard accelerators (shortkut keys)
    display.hAccelTable = LoadAccelerators (hInstance, szAppName);
  
    // Take care of any outstanding messages
    ProcessPendingEvents ();
    
    // lincity_main() contains main message loop
    lincity_main (1, args);

    // Take care of any outstanding messages after main() finishes.
    ProcessPendingEvents ();

    return 0;
    lpCmdLine;	// This prevents 'unused formal parameter' warnings
}



//----------------------------------------------------------------------------
//  FUNCTION: MyRegisterClass(CONST WNDCLASS*)
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//      This function and its usage is only necessary if you want this code
//      to be compatible with Win32 systems prior to the 'RegisterClassEx'
//      function that was added to Windows 95. It is important to call this function
//      so that the application will get 'well formed' small icons associated
//      with it.
//----------------------------------------------------------------------------
ATOM MyRegisterClass (CONST WNDCLASS * lpwc) 
{
    HANDLE hMod;
    FARPROC proc;
    WNDCLASSEX wcex;
    
    hMod = GetModuleHandle ("USER32");
    if (hMod != NULL) {
#if defined (UNICODE)
	proc = GetProcAddress ((HMODULE) hMod, "RegisterClassExW");
#else /*  */
	proc = GetProcAddress ((HMODULE) hMod, "RegisterClassExA");
#endif /*  */
	if (proc != NULL) {
	    // GCS: This typedef is for the typecast in the return stmt below
	    typedef int (FAR WINAPI * my_FARPROC) (WNDCLASSEX *);

	    wcex.style = lpwc->style;
	    wcex.lpfnWndProc = lpwc->lpfnWndProc;
	    wcex.cbClsExtra = lpwc->cbClsExtra;
	    wcex.cbWndExtra = lpwc->cbWndExtra;
	    wcex.hInstance = lpwc->hInstance;
	    wcex.hIcon = lpwc->hIcon;
	    wcex.hCursor = lpwc->hCursor;
	    wcex.hbrBackground = lpwc->hbrBackground;
	    wcex.lpszMenuName = lpwc->lpszMenuName;
	    wcex.lpszClassName = lpwc->lpszClassName;
	    
	    // Added elements for Windows 95:
	    wcex.cbSize = sizeof (WNDCLASSEX);
	    wcex.hIconSm = LoadIcon (wcex.hInstance, "SMALL");
	    
	    return (*((my_FARPROC) (proc))) (&wcex);
	}
    }
    return (RegisterClass (lpwc));
}


//----------------------------------------------------------------------------
//  FUNCTION: InitApplication(HANDLE)
//
//  PURPOSE: Initializes window data and registers window classes
//
//  COMMENTS:
//         We need two classes -- one which has a menu, one without a menu
//         (for full-screen).  This is registered by calling RegisterClass 
//         for Win 3.1/NT, or MyRegisterClass for Win 95.
//----------------------------------------------------------------------------
BOOL InitApplication (HINSTANCE hInstance) 
{
    WNDCLASS wcWithMenu;
    WNDCLASS wcWithoutMenu;
    // If Lin City is already running, focus existing process.
    // Win32 always sets hPrevInstance to NULL, so check w/ FindWindow()
    HWND hwnd = FindWindow (szClassNameWithMenu, NULL);
  
    if (!hwnd)
	hwnd = FindWindow (szClassNameWithoutMenu, NULL);
    if (hwnd) {
	// We found another version of ourself. Lets defer to it:
	if (IsIconic (hwnd)) {
	    ShowWindow (hwnd, SW_RESTORE);
	}
	SetForegroundWindow (hwnd);
	return FALSE;
    }

    // Create brush for background: Nasty Brown (Lin City color # 105)
    hbrBackground = CreateSolidBrush (RGB (0x4C, 0x4C, 0));

    // Fill in WNDCLASS for class WITH MENU.
    wcWithMenu.style = 0;
    wcWithMenu.lpfnWndProc = (WNDPROC) WndProc;
    wcWithMenu.cbClsExtra = 0;
    wcWithMenu.cbWndExtra = 0;
    wcWithMenu.hInstance = hInstance;
    wcWithMenu.hIcon = LoadIcon (hInstance, szAppName);
    wcWithMenu.hCursor = LoadCursor (NULL, IDC_ARROW);
    wcWithMenu.hbrBackground = hbrBackground;
  
    // Windows95 has different recommended help menu format.
#if defined (USE_WIN32_MENU)
    if (IS_WIN95) {
	wcWithMenu.lpszMenuName = "WIN95";
    } else {
	wcWithMenu.lpszMenuName = szAppName;
    }
    wcWithMenu.lpszClassName = szClassNameWithMenu;
#else
    wcWithMenu.lpszMenuName = "";
    wcWithMenu.lpszClassName = szClassNameWithMenu;
#endif

    // Fill in WNDCLASS for class WITHOUT MENU.
    wcWithoutMenu.style = 0;
    wcWithoutMenu.lpfnWndProc = (WNDPROC) WndProc;
    wcWithoutMenu.cbClsExtra = 0;
    wcWithoutMenu.cbWndExtra = 0;
    wcWithoutMenu.hInstance = hInstance;
    wcWithoutMenu.hIcon = LoadIcon (hInstance, szAppName);
    wcWithoutMenu.hCursor = LoadCursor (NULL, IDC_ARROW);
    wcWithoutMenu.hbrBackground = hbrBackground;
    wcWithoutMenu.lpszMenuName = "";
    wcWithoutMenu.lpszClassName = szClassNameWithoutMenu;
    
    // Register the window classes and return success/failure code.
    if (IS_WIN95) {
	BOOL rv;
	rv = MyRegisterClass (&wcWithMenu);
	if (!rv)
	    return rv;
	rv = MyRegisterClass (&wcWithoutMenu);
	return rv;
    } else {
	BOOL rv;
	rv = RegisterClass (&wcWithMenu);
	if (!rv)
	    return rv;
	rv = RegisterClass (&wcWithoutMenu);
	return rv;
    }
}


//----------------------------------------------------------------------------
// CheckClientSize (int width, int height, int with_menus)
// --
// Return 1 if the screen can support a client of this size
//----------------------------------------------------------------------------
void
CheckClientSize (int width, int height, int with_menus)
{
    display.screenW = GetSystemMetrics (SM_CXSCREEN);
    display.screenH = GetSystemMetrics (SM_CYSCREEN);
}

//----------------------------------------------------------------------------
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE:  Creates main window 
//----------------------------------------------------------------------------
BOOL
InitInstance (HINSTANCE hInstance, int nCmdShow)
{
    HDC hDCGlobal = GetDC (NULL);
    INT iRasterCaps;
    char *szClassName = 0;
    DWORD dwStyle;
    RECT client_size;

    // Determine graphics capabilities
    iRasterCaps = GetDeviceCaps (hDCGlobal, RASTERCAPS);
    if (iRasterCaps & RC_PALETTE) {
	display.hasPalette = TRUE;
	display.paletteSize = GetDeviceCaps (hDCGlobal, SIZEPALETTE);
	display.defaultPaletteSize = GetDeviceCaps (hDCGlobal, NUMCOLORS);
    } else {
	display.hasPalette = FALSE;
    }

    display.colorDepth = GetDeviceCaps (hDCGlobal, BITSPIXEL);
    ReleaseDC (NULL, hDCGlobal);
  
    // Decide whether to use DIB's or DDB's
#if defined (WIN32_USEDIB)
    if (display.colorDepth == 8)	// Only use DIB for 256 colors
	display.useDIB = TRUE;
    else
	display.useDIB = FALSE;
#else /*  */
    display.useDIB = FALSE;
#endif /*  */
    
    // Do some global initializations
    display.hInst = hInstance;
    display.fullscreen = FALSE;
    //display.fullscreen = TRUE;
    display.screenW = GetSystemMetrics (SM_CXSCREEN);
    display.screenH = GetSystemMetrics (SM_CYSCREEN);
    display.winFullscreenClientW = (INT) GetSystemMetrics (SM_CXFULLSCREEN);
    display.winFullscreenClientH = (INT) GetSystemMetrics (SM_CYFULLSCREEN);
    InitializePalette ();

    // Choose one of Lin City window types
    //    a)  no pix doubling, no border
    //    b)  no pix doubling, 30 pixel border
    //    c)  pix doubling, no border
    // GCS FIX: This doesn't work.  I need to call AdjustWindowRect on these values (?)
    if ((display.screenW >= 2*WINWIDTH + 2*BORDERX) &&
	(display.screenH >= 2*WINHEIGHT + 2*BORDERY)) {
	pix_double = 1;
	borderx = 1;
	bordery = 1;
    }
    else if ((display.screenW >= 2*WINWIDTH) &&
	     (display.screenH >= 2*WINHEIGHT)) {
	pix_double = 1;
	borderx = 0;
	bordery = 0;
    }
    else if ((display.screenW >= WINWIDTH + (2 * BORDERX)) 
	     &&(display.screenH >= WINHEIGHT + (2 * BORDERY))) {
	pix_double = 0;
	borderx = BORDERX;
	bordery = BORDERY;
    } else {
	pix_double = 0;
	borderx = 0;
	bordery = 0;
    }
    display.clientW = ((pix_double + 1) * WINWIDTH) + (2 * borderx);
    display.clientH = ((pix_double + 1) * WINHEIGHT) + (2 * bordery);
    
    // Choose one of three client window types
    //    a)  full screen
    //    b)  maximized window
    //    c)  regular window
    // Prefer regular over maximized over full screen
    dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
    client_size.left = 0;
    client_size.top = 0;
    client_size.right = display.clientW;
    client_size.bottom = display.clientH;
    AdjustWindowRect (&client_size, dwStyle, TRUE);

    if (((client_size.right - client_size.left) <= display.screenW) 
	&&((client_size.bottom - client_size.top) <= display.screenH))
    {
	// Use regular (overlapped) window
	szClassName = szClassNameWithMenu;
	if (client_size.left < 0) {
	    client_size.right -= client_size.left;
	    client_size.left = CW_USEDEFAULT;
	}
	if (client_size.top < 0) {
	    client_size.bottom -= client_size.top;
	    client_size.top = CW_USEDEFAULT;
	}
    } else {
	// Check maximized by subtracting out the DLGFRAME size.
	// Note that the "obvious" method of calling AdjustWindowRect()
	// with style WS_MAXIMIZE doesn't work!
	int win_border_x = GetSystemMetrics (SM_CXDLGFRAME);
	int win_border_y = GetSystemMetrics (SM_CYDLGFRAME);
	if (((client_size.right - client_size.left - 2 * win_border_x) <= display.screenW) 
	    &&((client_size.bottom - client_size.top - 2 * win_border_y) <= display.screenH))
	{
	    // Use maximized window
	    szClassName = szClassNameWithMenu;
	    // dwStyle |= WS_MAXIMIZE;
	    if (client_size.left < 0) {
		client_size.right -= client_size.left;
		client_size.left = CW_USEDEFAULT;
	    }
	    if (client_size.top < 0) {
		client_size.bottom -= client_size.top;
		client_size.top = CW_USEDEFAULT;
	    }
	    nCmdShow = SW_SHOWMAXIMIZED;
	} else {
	    // Need fullsize window
	    szClassName = szClassNameWithoutMenu;
	    dwStyle = WS_POPUP;
	    client_size.left = 0;
	    client_size.top = 0;
	    client_size.right = display.screenW;
	    client_size.bottom = display.screenH;
	}
    }
    
    display.min_h = client_size.bottom - client_size.top;
    display.min_w = client_size.right - client_size.left;

    // Create the window
    display.hWnd = CreateWindow (szClassName,	// Class name
				 szTitle,	// Caption
				 dwStyle,	// Style
				 client_size.left, client_size.top,	// Position
				 client_size.right, client_size.bottom,	// Size
				 (HWND) NULL,	// Parent window (no parent)
				 (HMENU) NULL,		// use class menu
				 (HINSTANCE) hInstance,	// handle to window instance
				 (LPVOID) NULL		// no params to pass on
				 );
  
    if (!display.hWnd) {
	return (FALSE);
    }
  
    // Display the window
    ShowWindow (display.hWnd, nCmdShow);
    UpdateWindow (display.hWnd);
    
    return (TRUE);
}


//----------------------------------------------------------------------------
//  FUNCTION: ProcessNextEvent ()
//            Wait for a message if necessary,
//            then process all messages.
//----------------------------------------------------------------------------
void
ProcessNextEvent (void) 
{
  
    MSG msg;
    if (GetMessage (&msg, NULL, 0, 0)) {
	if (!TranslateAccelerator (msg.hwnd, (HACCEL) display.hAccelTable, 
				   &msg)) {
	    TranslateMessage (&msg);
	    DispatchMessage (&msg);
	}
    }
    ProcessPendingEvents ();
}



//----------------------------------------------------------------------------
//  FUNCTION: ProcessPendingEvents ()
//            Process all outstanding messages.
//----------------------------------------------------------------------------
void
ProcessPendingEvents (void) 
{
    MSG msg;
    while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
	if (!TranslateAccelerator (msg.hwnd, (HACCEL) display.hAccelTable, 
				   &msg)) {
	    TranslateMessage (&msg);
	    DispatchMessage (&msg);
	}
    }
}



//----------------------------------------------------------------------------
//  FUNCTION: EnableWindowsMenuItems ()
//  FUNCTION: DisableWindowsMenuItems ()
//            Enable and Disable "Load" and "Save" items
//----------------------------------------------------------------------------
void
EnableWindowsMenuItems (void) 
{
#if defined (USE_WIN32_MENU)
    HMENU hMenu = GetMenu (display.hWnd);
    EnableMenuItem (hMenu, IDM_OPEN, MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem (hMenu, IDM_SAVE, MF_BYCOMMAND | MF_ENABLED);
    EnableMenuItem (hMenu, IDM_HELPCONTENTS, MF_BYCOMMAND | MF_ENABLED);
#endif
}


void
DisableWindowsMenuItems (void) 
{
#if defined (USE_WIN32_MENU)
    HMENU hMenu = GetMenu (display.hWnd);
    EnableMenuItem (hMenu, IDM_OPEN, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem (hMenu, IDM_SAVE, MF_BYCOMMAND | MF_GRAYED);
    EnableMenuItem (hMenu, IDM_HELPCONTENTS, MF_BYCOMMAND | MF_GRAYED);
#endif
}



//----------------------------------------------------------------------------
//  FUNCTION: HandleMouse ()
//----------------------------------------------------------------------------
void
HandleMouse () 
{
    MSG msg;
    /* Process queued events, ignoring mouse moves. */
    while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
	if (!TranslateAccelerator (msg.hwnd, (HACCEL) display.hAccelTable, 
				   &msg)) {
	    pending_mouse_x = cs_mouse_x;
	    pending_mouse_y = cs_mouse_y;
	    pending_mouse_event = 0;
	    pending_resize_event = 0;
	    TranslateMessage (&msg);
	    DispatchMessage (&msg);
	    if (pending_mouse_event)
		cs_mouse_handler (pending_mouse_event,
				  pending_mouse_x - cs_mouse_x,
				  pending_mouse_y - cs_mouse_y);
	    if (pending_resize_event)
	        resize_geometry (pending_resize_w, pending_resize_h);
	}
    }
  
    /* Now move the mouse if necessary. */
    if (pending_mouse_x != cs_mouse_x 
	|| pending_mouse_y != cs_mouse_y)
	cs_mouse_handler (0, pending_mouse_x - cs_mouse_x, 
			  pending_mouse_y - cs_mouse_y);
}



//----------------------------------------------------------------------------
//  FUNCTION: GetKeystroke ()
//----------------------------------------------------------------------------
char
GetKeystroke () 
{
    char key;
    /* GCS 02/02/2003  I found out that sometimes the mouse events were
		       getting lost here. */
    HandleMouse ();
//    ProcessPendingEvents ();
    key = x_key_value;
    x_key_value = 0;
    return key;
}



//----------------------------------------------------------------------------
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//----------------------------------------------------------------------------
LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
#if defined (USE_WIN32_MENU)
    int wmId, wmEvent;
#endif
    
    switch (message)
    {
#if defined (USE_WIN32_MENU)
    case WM_COMMAND: {
	wmId = LOWORD (wParam);
	wmEvent = HIWORD (wParam);

	//Parse the menu selections:
	switch (wmId)
	{

	case IDM_OPEN:
	    load_flag = 1;
	    DisableWindowsMenuItems ();
	    break;

	case IDM_SAVE:
	    save_flag = 1;
	    DisableWindowsMenuItems ();
	    break;

	case IDM_EXIT:
	    // GCS FIX: I'm still not quite satisfied with this...
	    // x_key_value = 'q';
	    DestroyWindow (hWnd);
	    break;

	case IDM_ABOUT:
	    DialogBox (display.hInst, "AboutBox", hWnd, (DLGPROC) About);
	    break;

	case IDM_HELPCONTENTS:
	    help_flag = 1;
	    DisableWindowsMenuItems ();
	    activate_help ("index.hlp");
	    break;

	default:
	    return (DefWindowProc (hWnd, message, wParam, lParam));
	}
    }
    break;
#endif
    
    case WM_KEYDOWN: {
	int nVirtKey = (int) wParam;
	x_key_shifted = (GetKeyState (VK_SHIFT) & 0x80000000) ? TRUE : FALSE;
	switch (nVirtKey)
	{
	case VK_LEFT:
	    x_key_value = 1;
	    break;
  
	case VK_DOWN:
	    x_key_value = 2;
	    break;
  
	case VK_UP:
	    x_key_value = 3;
	    break;
  
	case VK_RIGHT:
	    x_key_value = 4;
	    break;
  
	case VK_ESCAPE:
	    x_key_value = 5;
	    break;
	}
    }
    break;
      
    case WM_CHAR: {
	TCHAR chCharCode = (TCHAR) wParam;	// character code
	LPARAM lKeyData = lParam;	// key data
	if (chCharCode == 8) {		// Fix backspace
	    chCharCode = 127;
	} else if (chCharCode == VK_ESCAPE) {  // Fix escape
	    chCharCode = 5;
	}
	x_key_value = chCharCode;
    }
    break;
	
    // RightClick on windows non-client area
    case WM_NCRBUTTONUP:
	if (IS_WIN95 && SendMessage (hWnd, WM_NCHITTEST, 0, lParam) == HTSYSMENU)
	{
	    // The user has clicked the right button on the applications
	    // 'System Menu'. Here is where you would alter the default
	    // system menu to reflect your application. Notice how the
	    // explorer deals with this. For this app, we aren't doing
	    // anything
	    return (DefWindowProc (hWnd, message, wParam, lParam));
	} else {
	    // Nothing we are interested in, allow default handling...
	    return (DefWindowProc (hWnd, message, wParam, lParam));
	}
	break;
	
	// RightClick in windows client area
    case WM_RBUTTONDOWN:
	pending_mouse_x = UnAdjustX (LOWORD (lParam));
	pending_mouse_y = UnAdjustY (HIWORD (lParam));
	pending_mouse_event = LC_MOUSE_RIGHTBUTTON | LC_MOUSE_PRESS;
	cs_mouse_shifted = (wParam & MK_SHIFT) ? 1 : 0;
	break;
	
	// Left Click in windows client area
    case WM_LBUTTONDOWN:
	pending_mouse_x = UnAdjustX (LOWORD (lParam));
	pending_mouse_y = UnAdjustY (HIWORD (lParam));
	pending_mouse_event = LC_MOUSE_LEFTBUTTON | LC_MOUSE_PRESS;
	cs_mouse_shifted = (wParam & MK_SHIFT) ? 1 : 0;
	break;
      
    case WM_RBUTTONUP:
	pending_mouse_x = UnAdjustX (LOWORD (lParam));
	pending_mouse_y = UnAdjustY (HIWORD (lParam));
	pending_mouse_event = LC_MOUSE_RIGHTBUTTON | LC_MOUSE_RELEASE;
	cs_mouse_shifted = (wParam & MK_SHIFT) ? 1 : 0;
	break;

    case WM_LBUTTONUP:
	pending_mouse_x = UnAdjustX (LOWORD (lParam));
	pending_mouse_y = UnAdjustY (HIWORD (lParam));
	pending_mouse_event = LC_MOUSE_LEFTBUTTON | LC_MOUSE_RELEASE;
	cs_mouse_shifted = (wParam & MK_SHIFT) ? 1 : 0;
	break;
	
    case WM_MOUSEMOVE:
	pending_mouse_x = UnAdjustX (LOWORD (lParam));
	pending_mouse_y = UnAdjustY (HIWORD (lParam));
	cs_mouse_shifted = (wParam & MK_SHIFT) ? 1 : 0;
	break;
	
	// Only comes through on plug'n'play systems
    case WM_DISPLAYCHANGE: {
	SIZE szScreen;
	BOOL fChanged = (BOOL) wParam;
	  
	szScreen.cx = LOWORD (lParam);
	szScreen.cy = HIWORD (lParam);
	  
	if (fChanged) {
	    // The display 'has' changed. szScreen reflects the
	    // new size.
	    MessageBox (GetFocus (), "Display Changed", szAppName, 0);
	} else {
	    // The display 'is' changing. szScreen reflects the
	    // original size.
	    MessageBeep (0);
	}
    }
    break;
	
    case WM_PAINT: {
	PAINTSTRUCT ps;
	HDC hdc;
	  
	hdc = BeginPaint (hWnd, &ps);
	CopyBackingStoreToScreen (hdc, hWnd, &ps);
	EndPaint (hWnd, &ps);
    }
    break;
	
    // Create client area
    case WM_CREATE:
	InitializeBackingStore (hWnd);
	break;
	
    case WM_DESTROY:
	// PostQuitMessage(0);
	exit (0);		// OK??
	break;
	
    case WM_SIZE:
	// Resize window
	if (wParam != SIZE_MINIMIZED) {
	    pending_resize_event = 1;
	    pending_resize_w = LOWORD (lParam);
	    pending_resize_h = HIWORD (lParam);
	    ResizeBackingStore (hWnd);
	}
	break;

    case WM_GETMINMAXINFO: {
	/* GCS FIX */
	/* This needs to include client menu */
	/* To do this, I need to call AdjustWindowRect, but 
	    probably I need consider that the window might be POPUP... */
	LPMINMAXINFO lpmmi = (LPMINMAXINFO) lParam;
#if defined (commentout)
	lpmmi->ptMinTrackSize.x = (640 << pix_double) + 2 * borderx;
	lpmmi->ptMinTrackSize.y = (480 << pix_double) + 2 * bordery;
#endif
	lpmmi->ptMinTrackSize.x = display.min_w;
	lpmmi->ptMinTrackSize.y = display.min_h;
	break;
	}

    default:
	return (DefWindowProc (hWnd, message, wParam, lParam));
    }
    return (0);
}



//----------------------------------------------------------------------------
//  FUNCTION: InitializeBackingStore ()
//----------------------------------------------------------------------------
void
InitializeBackingStore (HWND hWnd) 
{
    RECT rc;
    
    GetClientRect (hWnd, &rc);
    display.winW = rc.right - rc.left;
    display.winH = rc.bottom - rc.top;
    
    InitializePalette ();
    CreateDDB (hWnd);
    if (display.useDIB) {
	CreateDIB ();
    }
    display.hSaveUnderHdc = 0;
    display.hSaveUnderBitmap = 0;
}

void
ResizeBackingStore (HWND hWnd) 
{
    ResizeDDB (hWnd);
    if (display.useDIB) {
	// This had better be false
	exit(-1);
    }
}



//----------------------------------------------------------------------------
//  FUNCTION: InitializePalette ()
//----------------------------------------------------------------------------
void
InitializePalette (void)
{
    INT iLoop;
    
    display.hPal = 0;
    display.pLogPal = 0;
    // Clear out palette arrays
    for (iLoop = 0; iLoop < 256; iLoop++) {
	display.colorrefPal[iLoop] = 0;
	display.brushPal[iLoop] = 0;
    }
    
    // If the device uses a palette (e.g. 8 bit display),
    // we need to create an HPALETTE for the HDC.
    if (display.hasPalette) {
	// Allocate memory for LOGPALETTE
	display.pLogPal = (NPLOGPALETTE) LocalAlloc (LMEM_FIXED, 
						     (sizeof (LOGPALETTE) + 
						      (sizeof (PALETTEENTRY) * (display.paletteSize))));
	if (!display.pLogPal) {
	    MessageBox (display.hWnd, "Not enough memory for logical palette.", NULL, MB_OK | MB_ICONHAND);
	    PostQuitMessage (0);
	    exit (-1);
	}
	display.pLogPal->palVersion = 0x300;
	display.pLogPal->palNumEntries = display.paletteSize;
      
	// Fill palette with system colors by default
	for (iLoop = 0; iLoop < display.paletteSize; iLoop++) {
	    *((WORD *) (&display.pLogPal->palPalEntry[iLoop].peRed)) = (WORD) iLoop;
	    display.pLogPal->palPalEntry[iLoop].peBlue = 0;
	    display.pLogPal->palPalEntry[iLoop].peFlags = PC_EXPLICIT;
	}
      
	// Convert LOGPALETTE into HPALETTE
	display.hPal = CreatePalette ((LPLOGPALETTE) display.pLogPal);
    }
}


//----------------------------------------------------------------------------
//  FUNCTION: CreateDIB ()
//----------------------------------------------------------------------------
void
CreateDIB (void) 
{
    // Let's create a 256 color DIB, just for testing!
    ULONG sizBMI;
    INT iNumClr = 256;
    INT iLoop;
    
    // Get memory for 256 color BITMAPINFO
    sizBMI = sizeof (BITMAPINFOHEADER) + sizeof (RGBQUAD) * iNumClr;
    if ((display.pbminfo = (PBITMAPINFO) GlobalAlloc (GMEM_FIXED | GMEM_ZEROINIT, sizBMI)) == NULL) {
	MessageBox (display.hWnd, "Failed in Memory Allocation for bminfo!", "Error", MB_OK);
	exit (-1);
    }

    // Fill in bitmap info
    display.pbminfo->bmiHeader.biSize = 0x28;	// GDI needs this to work
    display.pbminfo->bmiHeader.biWidth = display.winW;
    display.pbminfo->bmiHeader.biHeight = display.winH;
    display.pbminfo->bmiHeader.biPlanes = 1;
    display.pbminfo->bmiHeader.biBitCount = 8;
    display.pbminfo->bmiHeader.biCompression = BI_RGB;
    // this choice for biSizeImage reflects one byte per pixel
    display.pbminfo->bmiHeader.biSizeImage = display.winW * display.winH;
    display.pbminfo->bmiHeader.biXPelsPerMeter = 0;
    display.pbminfo->bmiHeader.biYPelsPerMeter = 0;
    display.pbminfo->bmiHeader.biClrUsed = 0;
    display.pbminfo->bmiHeader.biClrImportant = 0;
    
    // Fill in colormap w/ black
    for (iLoop = 0; iNumClr; iLoop++) {
	display.pbminfo->bmiColors[iLoop].rgbRed = 0;
	display.pbminfo->bmiColors[iLoop].rgbGreen = 0;
	display.pbminfo->bmiColors[iLoop].rgbBlue = 0;
	display.pbminfo->bmiColors[iLoop].rgbReserved = 0;
    }
  
    // CreateDIBSection() will allocate the pBits.
    display.hDIB = CreateDIBSection (display.hdcMem, display.pbminfo, 
				     DIB_RGB_COLORS, 
				     (void **) (&display.pBits), NULL, 0);
}



//----------------------------------------------------------------------------
//  FUNCTION: CreateDDB ()
//----------------------------------------------------------------------------
void
CreateDDB (HWND hWnd) 
{
    HDC hdc;
    HBITMAP hBitmapOri;
    HPALETTE hPalOld;
    RECT rect;
    HBRUSH hbr, hbrOld;

    hdc = GetDC (hWnd);
    if (display.hasPalette) {
	hPalOld = SelectPalette (hdc, (HPALETTE) display.hPal, FALSE);
	if (RealizePalette (hdc)) {
	    UpdateColors (hdc);
	}
    }
  
    // Before an application can use a memory device 
    // context for drawing operations, it must select 
    // a bitmap of the correct width and height into 
    // the device context. Once a bitmap has been selected, 
    // the device context can be used to prepare images 
    // that will be copied to the screen or printed. 
    display.hdcMem = CreateCompatibleDC (hdc);
    if (display.hasPalette) {
	display.hPaletteMemOri = SelectPalette (display.hdcMem, (HPALETTE) display.hPal, FALSE);
	RealizePalette (display.hdcMem);
    }
    display.hBitmap = CreateCompatibleBitmap (hdc, display.winW, display.winH);
    hBitmapOri = (HBITMAP) SelectObject (display.hdcMem, display.hBitmap);
  
    // Write that nasty brown color into the backing store
    rect.left = 0;
    rect.top = 0;
    rect.right = display.winW;
    rect.bottom = display.winH;
    hbr = hbrBackground;
    hbrOld = (HBRUSH) SelectObject (hdc, hbr);	// Select brush
    FillRect (display.hdcMem, &rect, (HBRUSH) hbr);	// Draw rectangle
    hbr = (HBRUSH) SelectObject (display.hdcMem, hbrOld);		// Unselect brush
  
#if defined (commentout)
    // GCS: For now, just keep the old bitmap around.
    //      I'll use it to swap out the full-sized color bitmap
    //      when I do a SetDIBits/GetDIBits.
    display.hBitmapOri = hBitmapOri;
#endif /*  */
    
#if defined (commentout)
    // GCS: Instead of reselecting the default bitmap back into the hdc,
    //      I'm gonna delete it. Any problems with this??
    DeleteObject (hBitmapOri);
#endif /*  */
    
    ReleaseDC (hWnd, hdc);
}


void
ResizeDDB (HWND hWnd) 
{
    RECT rect;
    HDC hdc;
    HBRUSH hbr, hbrOld;

    hdc = GetDC (hWnd);
    DeleteObject (display.hBitmap);
    display.hBitmap = CreateCompatibleBitmap (hdc, pending_resize_w, pending_resize_h);
    SelectObject (display.hdcMem, display.hBitmap);

    rect.left = 0;
    rect.top = 0;
    rect.right = pending_resize_w;
    rect.bottom = pending_resize_h;
    hbr = hbrBackground;
    hbrOld = (HBRUSH) SelectObject (hdc, hbr);		    // Select brush
    FillRect (display.hdcMem, &rect, (HBRUSH) hbr);	    // Draw rectangle
    hbr = (HBRUSH) SelectObject (display.hdcMem, hbrOld);   // Unselect brush

    ReleaseDC (hWnd, hdc);
}

//----------------------------------------------------------------------------
//  FUNCTION: CopyBackingStoreToScreen ()
//----------------------------------------------------------------------------
BOOL CopyBackingStoreToScreen (HDC hdc, HWND hWnd, LPPAINTSTRUCT ps) 
{
    HPALETTE hPalOld, hPalOldMem;
    
    // Is this necessary???
    if (display.hasPalette) {
	hPalOldMem = SelectPalette (display.hdcMem, (HPALETTE) display.hPal, FALSE);
	RealizePalette (display.hdcMem);
	hPalOld = SelectPalette (hdc, (HPALETTE) display.hPal, FALSE);
	RealizePalette (hdc);
    }
    
    // Copy DIB into DDB
    if (display.useDIB) {
	if (SetDIBits (display.hdcMem, display.hBitmap, 0, 
		       display.pbminfo->bmiHeader.biHeight, (LPSTR) display.pBits, 
		       display.pbminfo, DIB_RGB_COLORS) == 0)
	{
	    MessageBox (display.hWnd, "Failed in SetDIBits!", "Error", MB_OK);
	    exit (-1);
	}
    }
  
    // Blast DDB onto screen
    if (!BitBlt (hdc, ps->rcPaint.left, ps->rcPaint.top, 
		 (int) ps->rcPaint.right - ps->rcPaint.left, 
		 (int) ps->rcPaint.bottom - ps->rcPaint.top, 
		 display.hdcMem, ps->rcPaint.left, 
		 ps->rcPaint.top, SRCCOPY))
    {
	MessageBox (NULL, "BitBlt failed!", "Failure!", MB_OK);
    }
  
    
    // Draw square mouse
    if (cs_square_mouse_visible) {
	DoSquareMouse (hdc);
    }
    
    if (display.hasPalette) {
	display.hPal = SelectPalette (display.hdcMem, hPalOldMem, FALSE);
	display.hPal = SelectPalette (hdc, hPalOld, FALSE);
    }
    return TRUE;
}


//----------------------------------------------------------------------------
//  FUNCTION: DoSquareMouse ()
//----------------------------------------------------------------------------
void
DoSquareMouse (HDC hdc) 
{
    RECT rect;
    int size;
    HBRUSH hbr, hbrOld;

    size = (main_groups[selected_module_group].size) * 16;
  
  // Select Brush
    hbr = GetPaletteBrush (white (31));
    hbrOld = (HBRUSH) SelectObject (hdc, hbr);
  
  // Top rectangle
    rect.left = AdjustX (omx - 2);
    rect.top = AdjustY (omy - 2);
    rect.right = AdjustX (omx + size + 1);
    rect.bottom = AdjustY (omy - 1);
    FillRect (hdc, &rect, hbr);
  
    
  // Left rectangle
    rect.left = AdjustX (omx - 2);
    rect.top = AdjustY (omy);
    rect.right = AdjustX (omx - 1);
    rect.bottom = AdjustY (omy + size - 1);
    FillRect (hdc, &rect, hbr);
    
  // Right rectangle
    rect.left = AdjustX (omx + size);
    rect.top = AdjustY (omy);
    rect.right = AdjustX (omx + size + 1);
    rect.bottom = AdjustY (omy + size - 1);
    FillRect (hdc, &rect, hbr);
  
  // Bottom rectangle
    rect.left = AdjustX (omx - 2);
    rect.top = AdjustY (omy + size);
    rect.right = AdjustX (omx + size + 1);
    rect.bottom = AdjustY (omy + size + 1);
    FillRect (hdc, &rect, hbr);
    
  // Unselect brush
    hbr = (HBRUSH) SelectObject (display.hdcMem, hbrOld);
}


//----------------------------------------------------------------------------
//  FUNCTION: CopyPixmapToScreen ()
//----------------------------------------------------------------------------
#if defined (USE_PIXMAPS)
void
CopyPixmapToScreen (int t2, int src_x, int src_y, int width, int height, 
		    int dst_x, int dst_y) 
{
    HDC hdcPixmap;
    RECT rect;
  
    // Calculate update rectangle
    rect.left = AdjustX (dst_x);
    rect.top = AdjustY (dst_y);
    rect.right = AdjustX (dst_x + width + 1);
    rect.bottom = AdjustY (dst_y + height + 1);
  
    // Copy bitmap
    if (display.useDIB) {
	HBITMAP hOldBitmap, hOldBitmapMem;
	hdcPixmap = CreateCompatibleDC (display.hdcMem);
	hOldBitmap = (HBITMAP) SelectObject (hdcPixmap, icon_pixmap[t2]);
	hOldBitmapMem = (HBITMAP) SelectObject (display.hdcMem, display.hDIB);
	BitBlt (display.hdcMem, dst_x, dst_y, width, height, hdcPixmap, 
		src_x, src_y, SRCCOPY);
	icon_pixmap[t2] = (HBITMAP) SelectObject (hdcPixmap, hOldBitmap);
	display.hDIB = (HBITMAP) SelectObject (display.hdcMem, hOldBitmapMem);
	DeleteDC (hdcPixmap);
    } else {
	HBITMAP hOldBitmap;
	hdcPixmap = CreateCompatibleDC (display.hdcMem);
	hOldBitmap = (HBITMAP) SelectObject (hdcPixmap, icon_pixmap[t2]);
	BitBlt (display.hdcMem, AdjustX (dst_x), AdjustY (dst_y), 
		width << pix_double, height << pix_double, hdcPixmap, 
		src_x << pix_double, src_y << pix_double, SRCCOPY);
	icon_pixmap[t2] = (HBITMAP) SelectObject (hdcPixmap, hOldBitmap);
	DeleteDC (hdcPixmap);
    }
    
    // Queue up refresh
    InvalidateRect (display.hWnd, &rect, FALSE);
}
#endif /*  */


//----------------------------------------------------------------------------
//  FUNCTION: About(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for "About" dialog box
//              This version allows greater flexibility over the contents of the 'About' box,
//              by pulling out values from the 'Version' resource.
//
//  MESSAGES:
//
//      WM_INITDIALOG - initialize dialog box
//      WM_COMMAND      - Input received
//
//----------------------------------------------------------------------------
#if defined (USE_WIN32_MENU)
LRESULT CALLBACK About (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{
    static HFONT hfontDlg;	// Font for dialog text
    static HFONT hFinePrint;	// Font for 'fine print' in dialog
    DWORD dwVerInfoSize;		// Size of version information block
    LPSTR lpVersion;		// String pointer to 'version' text
    DWORD dwVerHnd = 0;		// An 'ignored' parameter, always '0'
    UINT uVersionLen;
    WORD wRootLen;
    BOOL bRetCode;
    int i;
    char szFullPath[256];
    char szResult[256];
    char szGetName[256];
    DWORD dwVersion;
    char szVersion[40];
    DWORD dwResult;
    
    switch (message)
    {
    case WM_INITDIALOG:
	ShowWindow (hDlg, SW_HIDE);
	hfontDlg = CreateFont (16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
			       VARIABLE_PITCH | FF_SWISS, "");
	hFinePrint = CreateFont (16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
				 VARIABLE_PITCH | FF_SWISS, "");
	CenterWindow (hDlg, GetWindow (hDlg, GW_OWNER));
	GetModuleFileName (display.hInst, szFullPath, sizeof (szFullPath));
	
	// Now lets dive in and pull out the version information:
	dwVerInfoSize = GetFileVersionInfoSize (szFullPath, &dwVerHnd);
	if (dwVerInfoSize) {
	    LPSTR lpstrVffInfo;
	    HANDLE hMem;
	    hMem = GlobalAlloc (GMEM_MOVEABLE, dwVerInfoSize);
	    lpstrVffInfo = (char *) GlobalLock (hMem);
	    GetFileVersionInfo (szFullPath, dwVerHnd, dwVerInfoSize, lpstrVffInfo);

	    // The below 'hex' value looks a little confusing, but
	    // essentially what it is, is the hexidecimal representation
	    // of a couple different values that represent the language
	    // and character set that we are wanting string values for.
	    // 040904E4 is a very common one, because it means:
	    //   US English, Windows MultiLingual characterset
	    // Or to pull it all apart:
	    // 04------             = SUBLANG_ENGLISH_USA
	    // --09----             = LANG_ENGLISH
	    // ----04E4 = 1252 = Codepage for Windows:Multilingual
	    lstrcpy (szGetName, "\\StringFileInfo\\040904E4\\");
	    wRootLen = lstrlen (szGetName);	// Save this position
	  
	    // Set the title of the dialog:
	    lstrcat (szGetName, "ProductName");
	    bRetCode = VerQueryValue ((LPVOID) lpstrVffInfo, 
				      (LPSTR) szGetName, 
				      (LPVOID *) & lpVersion, 
				      (UINT *) & uVersionLen);
	    lstrcpy (szResult, "About ");
	    lstrcat (szResult, lpVersion);
	    SetWindowText (hDlg, szResult);
	  
	    // Walk through the dialog items that we want to replace:
	    for (i = DLG_VERFIRST; i <= DLG_VERLAST; i++) {
		GetDlgItemText (hDlg, i, szResult, sizeof (szResult));
		szGetName[wRootLen] = (char) 0;
		lstrcat (szGetName, szResult);
		uVersionLen = 0;
		lpVersion = NULL;
		bRetCode = VerQueryValue ((LPVOID) lpstrVffInfo, 
					  (LPSTR) szGetName, 
					  (LPVOID *) & lpVersion, 
					  (UINT *) & uVersionLen);
		
		if (bRetCode && uVersionLen && lpVersion) {
		    // Replace dialog item text with version info
		    lstrcpy (szResult, lpVersion);
		    SetDlgItemText (hDlg, i, szResult);
		} else {
		    dwResult = GetLastError ();
		    wsprintf (szResult, "Error %lu", dwResult);
		    SetDlgItemText (hDlg, i, szResult);
		}
		SendMessage (GetDlgItem (hDlg, i), WM_SETFONT, 
			     (UINT) ((i == DLG_VERLAST) ? hFinePrint : hfontDlg), 
			     TRUE);
	    }
	    GlobalUnlock (hMem);
	    GlobalFree (hMem);
	} else {
	    // No version information available.
	}
	SendMessage (GetDlgItem (hDlg, IDC_LABEL), WM_SETFONT, 
		     (WPARAM) hfontDlg, (LPARAM) TRUE);
      
	// We are  using GetVersion rather then GetVersionEx
	// because earlier versions of Windows NT and Win32s
	// didn't include GetVersionEx:
	dwVersion = GetVersion ();
	if (dwVersion < 0x80000000) {
	    // Windows NT
	    wsprintf (szVersion, "Microsoft Windows NT %u.%u (Build: %u)", 
		      (DWORD) (LOBYTE (LOWORD (dwVersion))), 
		      (DWORD) (HIBYTE (LOWORD (dwVersion))), 
		      (DWORD) (HIWORD (dwVersion)));
	  
	} else if (LOBYTE (LOWORD (dwVersion)) < 4) {
	    // Win32s
	    wsprintf (szVersion, "Microsoft Win32s %u.%u (Build: %u)", 
		      (DWORD) (LOBYTE (LOWORD (dwVersion))), 
		      (DWORD) (HIBYTE (LOWORD (dwVersion))), 
		      (DWORD) (HIWORD (dwVersion) & ~0x8000));
	} else {
	    // Windows 95
	    wsprintf (szVersion, "Microsoft Windows 95 %u.%u", 
		      (DWORD) (LOBYTE (LOWORD (dwVersion))), 
		      (DWORD) (HIBYTE (LOWORD (dwVersion))));
	}
	SetWindowText (GetDlgItem (hDlg, IDC_OSVERSION), szVersion);
	ShowWindow (hDlg, SW_SHOW);
	return (TRUE);

    case WM_COMMAND:
	if (LOWORD (wParam) == IDOK || LOWORD (wParam) == IDCANCEL) {
	    EndDialog (hDlg, TRUE);
	    DeleteObject (hfontDlg);
	    DeleteObject (hFinePrint);
	    return (TRUE);
	}
	break;
    }
    return FALSE;
}



//----------------------------------------------------------------------------
//   FUNCTION: CenterWindow(HWND, HWND)
//
//   PURPOSE: Centers one window over another. 
//
//   COMMENTS:
//
//              In this function, we save the instance handle in a global variable and
//              create and display the main program window.
//
//              This functionwill center one window over another ensuring that 
//              the placement of the window is within the 'working area', meaning 
//              that it is both within the display limits of the screen, and not 
//              obscured by the tray or other framing elements of the desktop.
//----------------------------------------------------------------------------
BOOL CenterWindow (HWND hwndChild, HWND hwndParent) 
{
    RECT rChild, rParent, rWorkArea;
    int wChild, hChild, wParent, hParent;
    int xNew, yNew;
    BOOL bResult;

    // Get the Height and Width of the child window
    GetWindowRect (hwndChild, &rChild);
    wChild = rChild.right - rChild.left;
    hChild = rChild.bottom - rChild.top;
  
    // Get the Height and Width of the parent window
    GetWindowRect (hwndParent, &rParent);
    wParent = rParent.right - rParent.left;
    hParent = rParent.bottom - rParent.top;
  
    // Get the limits of the 'workarea'
    bResult = SystemParametersInfo (SPI_GETWORKAREA,
				    sizeof (RECT), 
				    &rWorkArea, 
				    0);
  
    if (!bResult) {
	rWorkArea.left = rWorkArea.top = 0;
	rWorkArea.right = GetSystemMetrics (SM_CXSCREEN);
	rWorkArea.bottom = GetSystemMetrics (SM_CYSCREEN);
    }
    
    // Calculate new X position, then adjust for workarea
    xNew = rParent.left + ((wParent - wChild) / 2);
  
    if (xNew < rWorkArea.left) {
	xNew = rWorkArea.left;
    }
    else if ((xNew + wChild) > rWorkArea.right) {
	xNew = rWorkArea.right - wChild;
    }
  
    // Calculate new Y position, then adjust for workarea
    yNew = rParent.top + ((hParent - hChild) / 2);
    if (yNew < rWorkArea.top) {
	yNew = rWorkArea.top;
    } else if ((yNew + hChild) > rWorkArea.bottom) {
	yNew = rWorkArea.bottom - hChild;
    }
  
    // Set it, and return
    return SetWindowPos (hwndChild, NULL, xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}
#endif /* USE_WIN32_MENU */


//----------------------------------------------------------------------------
//   FUNCTION: AddPaletteEntry ()
//----------------------------------------------------------------------------
void
AddPaletteEntry (int col, int red, int grn, int blu) 
{
  
    red = (red * 255) / 62;
    if (red > 255)
	red = 255;
  
    grn = (grn * 255) / 62;
    if (grn > 255)
	grn = 255;
  
    blu = (blu * 255) / 62;
    if (blu > 255)
	blu = 255;
    
    display.colorrefPal[col] = RGB (red, grn, blu);
    
    if (display.brushPal[col]) {
	DeleteObject (display.brushPal[col]);
	display.brushPal[col] = 0;
    }
  
    if (display.hasPalette) {
	display.pLogPal->palPalEntry[col].peRed = red;
	display.pLogPal->palPalEntry[col].peGreen = grn;
	display.pLogPal->palPalEntry[col].peBlue = blu;
	display.pLogPal->palPalEntry[col].peFlags = PC_NOCOLLAPSE;
    }
  
    if (display.useDIB) {
	display.pbminfo->bmiColors[col].rgbRed = red;
	display.pbminfo->bmiColors[col].rgbGreen = grn;
	display.pbminfo->bmiColors[col].rgbBlue = blu;
	display.pbminfo->bmiColors[col].rgbReserved = 0;
    }
}


//----------------------------------------------------------------------------
//  FUNCTION: UpdatePalette ()
//----------------------------------------------------------------------------
void
UpdatePalette (void) 
{
    if (display.hasPalette) {
	// Make a new HPALETTE and select to hdcGlobal and hdcMem
	HPALETTE hPalNew;
	hPalNew = CreatePalette ((LPLOGPALETTE) display.pLogPal);
	SelectPalette (display.hdcMem, hPalNew, FALSE);
	RealizePalette (display.hdcMem);
	DeleteObject (display.hPal);
	display.hPal = hPalNew;
    }
}

