/* ---------------------------------------------------------------------- *
 * lcwin32.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __lcwin32_h__
#define __lcwin32_h__

// Turn off compiler warning converting const double to float
#pragma warning(disable : 4244)


/* ---------------------------------------------------------------------- *
 * Defines
 * ---------------------------------------------------------------------- */
#define APPNAME "Lincity"

// Operating System Versions
#if defined (WIN32)
#define IS_WIN32 TRUE
#else	/*  */
#define IS_WIN32 FALSE
#endif	/*  */
#define IS_NT      IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)
#define IS_WIN32S  IS_WIN32 && (BOOL)(!(IS_NT) && (LOBYTE(LOWORD(GetVersion()))<4))
#define IS_WIN95 (BOOL)(!(IS_NT) && !(IS_WIN32S)) && IS_WIN32

#define WINWIDTH 640
#define WINHEIGHT 480

// Palette and DIB constants
#define PALETTESIZE	 256
#define PALVERSION   0x300


// ---------------------------------------------------------------------------
// Macros
// ---------------------------------------------------------------------------
/* Handle to a DIB */ 
DECLARE_HANDLE (HDIB);


// DIB Macros
#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits"
// parameter is the bit count for the scanline (biWidth * biBitCount),
// and this macro returns the number of DWORD-aligned bytes needed
// to hold those bits.
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)


// ---------------------------------------------------------------------------
// Type Definitions
// ---------------------------------------------------------------------------
typedef struct _disp 
{
    HINSTANCE hInst;    
    HANDLE hAccelTable;

    HWND hWnd;
    HDC hdcMem;
    HBITMAP hBitmap;		// Backing store

    HDC hSaveUnderHdc;
    HBITMAP hSaveUnderBitmap;

    int winX;
    int winY;
    int winW;
    int winH;

    BOOL fullscreen;
    INT screenW;		// Width of screen
    INT screenH;		// Height of screen
    INT winFullscreenClientW;	// Width of full screen window
    INT winFullscreenClientH;	// Height of full screen window
    INT clientW;		// Width of pixmap (adjusted by borderx/pix_double)
    INT clientH;		// Height of pixmap (adjusted by borderx/pix_double)
    long colorDepth;		// Number of color planes (e.g. 24 for full color)
    int min_h;			// Minimum size for resizing
    int min_w;			// Minimum size for resizing
    int nCmdShow;		// Initialize in which state?

    BOOL useDIB;		// Use DIB or DDB?
    BITMAPINFO *pbminfo;	// DIB Header
    PBYTE pBits;		// DIB Bits
    HBITMAP hDIB;		// DIB BITMAP

    BOOL hasPalette;
    INT paletteSize;		// Only valid if hasPalette
    INT defaultPaletteSize;	// Only valid if hasPalette
    HANDLE hPal;		// Handle to the application's logical palette
    HANDLE hPaletteMemOri;	// Original palette for hdcMem
    NPLOGPALETTE pLogPal;	// Pointer to program's logical palette
    COLORREF colorrefPal[256];	// For mapping index to COLORREF (for full color)
    HBRUSH brushPal[256];	// For mapping index to HBRUSH

    HFONT hFont;		// Lin City font
}
disp;

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
extern HANDLE hAccelTable;
extern disp display;
extern char *bg_color;
extern int verbose;
extern int pix_double;
extern int text_bg;
extern int text_fg;
extern int x_key_value;
extern BOOL x_key_shifted;
extern int borderx, bordery;
extern long unsigned int colour_table[256];
extern int xclip_x1, xclip_y1, xclip_x2, xclip_y2, clipping_flag;
extern unsigned char *open_font;
extern int open_font_height;
extern int suppress_next_expose;
extern int no_init_help;
extern char colour_pal_file[], fontfile[];
extern char windowsfontfile[];
extern int cs_mouse_x, cs_mouse_y;
/* extern int cs_current_mouse_x, cs_current_mouse_y;
extern int cs_current_mouse_button;*/
extern int cs_mouse_button;
extern int cs_mouse_shifted;
extern float gamma_correct_red, gamma_correct_green, gamma_correct_blue;

#ifdef USE_PIXMAPS
extern HBITMAP icon_pixmap[];
#endif	/* USE_PIXMAPS */


// ---------------------------------------------------------------------------
// Function Prototypes
// ---------------------------------------------------------------------------
void ProcessPendingEvents (void);
void ProcessNextEvent (void);
char GetKeystroke (void);
void HandleMouse (void);
void RefreshScreen (void);
void RefreshArea (int, int, int, int);
void SaveUnder (int, int, int, int, void *);
void RestoreSaveUnder (int, int, int, int, void *);
void CopyPixmapToScreen (int t2, int src_x, int src_y, int width, int height, 
			 int dst_x, int dst_y);
void AddPaletteEntry (int col, int red, int grn, int blu);
void HandleError (char *, int);
void UpdatePalette (void);
HBRUSH GetPaletteBrush (int col);

int AdjustX (int x);
int AdjustY (int y);
int UnAdjustX (int x);
int UnAdjustY (int y);

void my_x_putchar (int, int, unsigned char);
void open_x_putchar (int, int, unsigned char);
void call_event (void);
void do_call_event (int);
void init_windows_font (void);
void init_mouse (void);
void draw_border (void);
void lc_create_window (void);

void EnableWindowsMenuItems (void);
void DisableWindowsMenuItems (void);

#endif	/* __lcwin32_h__ */
