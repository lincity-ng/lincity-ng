/* ---------------------------------------------------------------------- *
 * lcwin32.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "lcstring.h"
#include "lin-city.h"
#include "common.h"
#include "lctypes.h"
#include "pixmap.h"
#include "screen.h"

#define USE_WINDOWS_FONT 1
#undef USE_WINDOWS_FONT

int
AdjustX (int x)
{
    x <<= pix_double;
    x += borderx;
    return x;
}

int
AdjustY (int y)
{
    y <<= pix_double;
    y += bordery;
    return y;
}

int
UnAdjustX (int x)
{
    x -= borderx;
    x >>= pix_double;
    return x;
}

int
UnAdjustY (int y)
{
    y -= bordery;
    y >>= pix_double;
    return y;
}

void
HandleError (char *description, int degree)
{
    MessageBox (NULL, description, "ERROR", MB_OK);
    if (degree == FATAL)
    {
        exit (-1);
    }
}

void
setcustompalette (void)
{
    char s[100];
    unsigned int n, r, g, b;
    int i, flag[256];
    FILE *inf;

    for (i = 0; i < 256; i++)
	flag[i] = 0;
    if ((inf = fopen (colour_pal_file, "r")) == 0)
	HandleError ("Can't find the colour pallet file", FATAL);

    while (feof (inf) == 0) {
	fgets (s, 99, inf);
	if (sscanf (s, "%u %u %u %u", &n, &r, &g, &b) == 4)
	{
	    r = ((r * (1 - gamma_correct_red))
		 + (64 * sin ((float) r * M_PI / 128))
		 * gamma_correct_red);
	    g = ((g * (1 - gamma_correct_green))
		 + (64 * sin ((float) g * M_PI / 128))
		 * gamma_correct_green);
	    b = ((b * (1 - gamma_correct_blue))
		 + (64 * sin ((float) b * M_PI / 128))
		 * gamma_correct_blue);
	    AddPaletteEntry (n, r, g, b);
	    flag[n] = 1;
	}
    }
    fclose (inf);
    for (i = 0; i < 256; i++) {
        if (flag[i] == 0) {
	    printf ("Colour %d not loaded\n", i);
	    HandleError ("Can't continue", FATAL);
	}
    }
    UpdatePalette ();
}

COLORREF
GetPaletteColorref (int col)
{
    assert (col >= 0 || col <= 255);
    if (display.hasPalette)
	return PALETTEINDEX (col);
    else
	return display.colorrefPal[col];
}

HBRUSH
GetPaletteBrush (int col)
{
    assert (col >= 0 || col <= 255);
    if (display.brushPal[col] == 0)
	display.brushPal[col] = CreateSolidBrush (GetPaletteColorref (col));
    return display.brushPal[col];
}

int CALLBACK
EnumFontFamProc (ENUMLOGFONT FAR * lpelf,     // pointer to logical-font data
		 NEWTEXTMETRIC FAR * lpntm,   // pointer to physical-font data 
		 int FontType,	// type of font 
		 LPARAM lParam	// address of application-defined data  
)
{
    //  if (0 != strcmp (lpelf->elfLogFont.lfFaceName, "iso8859-1-16x16"))
    //  if (0 != strcmp (lpelf->elfLogFont.lfFaceName, "Lincity"))
    //  if (0 != strcmp (lpelf->elfLogFont.lfFaceName, "Tester"))
    if (0 != strcmp (lpelf->elfLogFont.lfFaceName, "iso8859-1-9x15"))
        return 0;

    // GCS:  I'm not sure if it's OK to just copy the pointer here.
    //       This may cause problems, but let's give it a whirl.
    // *((LOGFONT**) lParam) = &(lpelf->elfLogFont);

    // GCS:  No wait, I changed my mind.  Copy the entire LOGFONT struct.
    *(LOGFONT *) lParam = lpelf->elfLogFont;
    return 1;
}

void
init_windows_font (void)
{
#if defined (USE_WINDOWS_FONT)
    LOGFONT logfont;
    int fonts_added = AddFontResource (windowsfontfile);
    if (fonts_added != 1) {
	HandleError ("Can't open the font file", FATAL);
    }
    SendMessage (HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    ProcessPendingEvents ();

    // GCS: This typedef is for the typecast...
    // typedef int (FAR WINAPI *voidfnptr)(void);
    // KSH: Changed the typedef to get around VC 6 strict type enformcement
#if defined (commentout)
    typedef int (FAR WINAPI * voidfnptr) (const struct tagLOGFONTA *, const struct tagTEXTMETRICA *, unsigned long, long);
    EnumFontFamilies (display.hdcMem, "Lincity", (voidfnptr) EnumFontFamProc, (LPARAM) & logfont);
#endif
    // GCS: Hmm.  There must be a way to get this to work on both compilers.  
    //            How about this?
    EnumFontFamilies (display.hdcMem, "iso8859-1-9x15", (FONTENUMPROC) EnumFontFamProc, (LPARAM) & logfont);
    //EnumFontFamilies (display.hdcMem, "iso8859-1-16x16", (FONTENUMPROC) EnumFontFamProc, (LPARAM) & logfont);

    display.hFont = CreateFontIndirect (&logfont);
    if (!display.hFont) {
	HandleError ("Error executing CreateFontIndirect", FATAL);
    }
    SelectObject (display.hdcMem, display.hFont);
#endif /* defined (USE_WINDOWS_FONT) */
}

void
gl_setpalettecolor (long x, long r, long g, long b)
{
    if (x >= 0 && x <= 255) {
	AddPaletteEntry (x, r, g, b);
    }
}

void
Fgl_setfontcolors (int bg, int fg)
{
    text_fg = fg;
    text_bg = bg;
}

void
Fgl_setfont (int fw, int fh, void *fp)
{
    open_font = fp;
    open_font_height = fh;
}

void
Fgl_setpixel (int x, int y, int col)
{
    int i;

    if (clipping_flag)
        if (x < xclip_x1 || x > xclip_x2 || y < xclip_y1 || y > xclip_y2)
	    return;
    col &= 0xff;

    // Draw to pixmap buffer
    i = pixmap_index(x,y);
    pixmap[i] = (unsigned char) col;

    // Draw to DC
    if (display.useDIB) {
	BYTE *pixel = display.pBits;
	unsigned long row = display.pbminfo->bmiHeader.biHeight - (y + bordery);
	unsigned long rowlength = display.pbminfo->bmiHeader.biWidth;
	unsigned long mod = rowlength % sizeof (DWORD);
	if (mod) {
	    rowlength += (sizeof (DWORD) - mod);
	}			// Pad out to DWORD boundary
	pixel += row * rowlength + x + borderx;	// Ptr arith w/ 4 bytes per pixel
	*pixel = col;
    } else {
	if (pix_double) {
	    RECT rect;
	    HBRUSH hbr, hbrOld;
	    rect.left = AdjustX (x);
	    rect.top = AdjustY (y);
	    rect.right = AdjustX (x + 1);
	    rect.bottom = AdjustY (y + 1);
	    hbr = GetPaletteBrush (col);
	    hbrOld = (HBRUSH) SelectObject (display.hdcMem, hbr);
	    FillRect (display.hdcMem, &rect, hbr);
	    hbr = (HBRUSH) SelectObject (display.hdcMem, hbrOld);
	} else {
	    SetPixel (display.hdcMem, x + borderx, y + bordery, GetPaletteColorref (col));
	}
    }
    // It's too slow to write directly to screen or to queue update event
    // for every pixel.  Instead, the caller will have to do this.
}


int
Fgl_getpixel (int x, int y)
{
    return pixmap_getpixel (x, y);
}


void
Fgl_hline (int x1, int y1, int x2, int col)
{
    int xor = 0;
    RECT rect;

    if (col == -1) {
	xor = 1;
    }
    col &= 0xff;
    pixmap_hline (x1, y1, x2, col);

    // Calculate extent of update rectangle
    rect.left = AdjustX (x1);
    rect.top = AdjustY (y1);
    rect.right = AdjustX (x2);	// LineTo() does not include final pixel of line
    rect.bottom = AdjustY (y1 + 1);

    // Draw to DC
    if (display.useDIB) {
	HPEN hpen, hpenOld;
	HBITMAP hOldBitmapMem;
	int oldrop2;
	hpen = CreatePen (PS_SOLID, 0, GetPaletteColorref (col));
	hpenOld = (HPEN) SelectObject (display.hdcMem, hpen);
	hOldBitmapMem = (HBITMAP) SelectObject (display.hdcMem, display.hDIB);
	MoveToEx (display.hdcMem, x1 + borderx, y1 + bordery, NULL);
	if (xor)
	    oldrop2 = SetROP2 (display.hdcMem, R2_NOT);
	LineTo (display.hdcMem, x2 + borderx, y1 + bordery);
	if (xor)
	    SetROP2 (display.hdcMem, oldrop2);
	display.hDIB = (HBITMAP) SelectObject (display.hdcMem, hOldBitmapMem);
	hpen = (HPEN) SelectObject (display.hdcMem, hpenOld);
	DeleteObject (hpen);
    } else {
	if (pix_double) {
	    HBRUSH hbr, hbrOld;
	    int oldrop2;
	    hbr = GetPaletteBrush (col);
	    hbrOld = (HBRUSH) SelectObject (display.hdcMem, hbr);
	    if (xor)
		oldrop2 = SetROP2 (display.hdcMem, R2_NOT);
	    FillRect (display.hdcMem, &rect, hbr);
	    if (xor)
		SetROP2 (display.hdcMem, oldrop2);
	    hbr = (HBRUSH) SelectObject (display.hdcMem, hbrOld);
	} else {
	    HPEN hpen, hpenOld;
	    int oldrop2;
	    hpen = CreatePen (PS_SOLID, 0, GetPaletteColorref (col));
	    hpenOld = (HPEN) SelectObject (display.hdcMem, hpen);
	    MoveToEx (display.hdcMem, x1 + borderx, y1 + bordery, NULL);
	    if (xor)
		oldrop2 = SetROP2 (display.hdcMem, R2_NOT);
	    LineTo (display.hdcMem, x2 + borderx, y1 + bordery);
	    if (xor)
		SetROP2 (display.hdcMem, oldrop2);
	    hpen = (HPEN) SelectObject (display.hdcMem, hpenOld);
	    DeleteObject (hpen);
	}
    }

    // Queue update event
    InvalidateRect (display.hWnd, &rect, FALSE);
}

void
Fgl_line (int x1, int y1, int dummy, int y2, int col)
     /* vertical lines only. */
{
    RECT rect;
    int y;
    col &= 0xff;
    pixmap_vline (x1, y1, y2, col);

    // Calculate extent of update rectangle
    rect.left = AdjustX (x1);
    rect.top = AdjustY (y1);
    rect.right = AdjustX (x1 + 1);
    rect.bottom = AdjustY (y2);	// LineTo() does not include final pixel of line

    // Draw to DC
    if (display.useDIB) {
	HPEN hpen, hpenOld;
	HBITMAP hOldBitmapMem;
	hpen = CreatePen (PS_SOLID, 0, GetPaletteColorref (col));
	hpenOld = (HPEN) SelectObject (display.hdcMem, hpen);
	hOldBitmapMem = (HBITMAP) SelectObject (display.hdcMem, display.hDIB);
	MoveToEx (display.hdcMem, x1 + borderx, y1 + bordery, NULL);
	LineTo (display.hdcMem, x1 + borderx, y2 + bordery);
	display.hDIB = (HBITMAP) SelectObject (display.hdcMem, hOldBitmapMem);
	hpen = (HPEN) SelectObject (display.hdcMem, hpenOld);
	DeleteObject (hpen);
    } else {
	if (pix_double) {
	    HBRUSH hbr, hbrOld;
	    hbr = GetPaletteBrush (col);
	    hbrOld = (HBRUSH) SelectObject (display.hdcMem, hbr);
	    FillRect (display.hdcMem, &rect, hbr);
	    hbr = (HBRUSH) SelectObject (display.hdcMem, hbrOld);
	} else {
	    HPEN hpen, hpenOld;
	    hpen = CreatePen (PS_SOLID, 0, GetPaletteColorref (col));
	    hpenOld = (HPEN) SelectObject (display.hdcMem, hpen);
	    MoveToEx (display.hdcMem, AdjustX (x1), AdjustY (y1), NULL);
	    LineTo (display.hdcMem, AdjustX (x1), AdjustY (y2));
	    hpen = (HPEN) SelectObject (display.hdcMem, hpenOld);
	    DeleteObject (hpen);
	}
    }

    // Queue update event
    InvalidateRect (display.hWnd, &rect, FALSE);
}


void
Fgl_write (int x, int y, char *s)
{
    int i;
    RECT rect;
#if defined (USE_WINDOWS_FONT)
    SIZE size;
    // Select background and foreground colors
    SetTextColor (display.hdcMem, GetPaletteColorref (text_fg));
    SetBkColor (display.hdcMem, GetPaletteColorref (text_bg));

    // Draw text to backing store
    ExtTextOut (display.hdcMem, AdjustX (x), AdjustY (y), 0, NULL,
		s, strlen (s), NULL);

    // Calculate size of text
    GetTextExtentPoint32 (display.hdcMem, s, strlen (s), &size);
#endif

    // Draws to pixmap only when USE_WINDOWS_FONT is defined
    for (i = 0; i < (int) (strlen (s)); i++)
	my_x_putchar (x + i * 8, y, s[i]);

    // Queue update event
#if defined (USE_WINDOWS_FONT)
    rect.left = AdjustX (x);
    rect.top = AdjustY (y);
    rect.right = AdjustX (x + size.cx);
    rect.bottom = AdjustY (y + size.cy);
    InvalidateRect (display.hWnd, &rect, FALSE);
#else
    rect.left = AdjustX (x);
    rect.top = AdjustY (y);
    rect.right = AdjustX (x + i * 8);
    rect.bottom = AdjustY (y + 8);
    InvalidateRect (display.hWnd, &rect, FALSE);
#endif
}


void
open_write (int x, int y, char *s)
{
    int i;
    RECT rect;
    for (i = 0; i < (int) (strlen (s)); i++)
        open_x_putchar (x + i * 8, y, s[i]);

    // Queue update event
    rect.left = AdjustX (x);
    rect.top = AdjustY (y);
    rect.right = AdjustX (x + i * 8);
    rect.bottom = AdjustY (y + open_font_height);
    InvalidateRect (display.hWnd, &rect, FALSE);
}

void
my_x_putchar (int xx, int yy, unsigned char c)
{
    int x, y, b;
    for (y = 0; y < 8; y++) {
	b = main_font[c * 8 + y];
	for (x = 0; x < 8; x++) {
	    if ((b & 0x80) == 0) {
#if defined (USE_WINDOWS_FONT)
		pixmap_setpixel (xx + x, yy + y, text_bg);
#else
		Fgl_setpixel (xx + x, yy + y, text_bg);
#endif
	    } else {
#if defined (USE_WINDOWS_FONT)
		pixmap_setpixel (xx + x, yy + y, text_fg);
#else
		Fgl_setpixel (xx + x, yy + y, text_fg);
#endif
	    }
	    b = b << 1;
	}
    }
}


void
open_x_putchar (int xx, int yy, unsigned char c)
{
    int x, y, b;
    for (y = 0; y < open_font_height; y++) {
	b = open_font[c * open_font_height + y];
	for (x = 0; x < 8; x++) {
	    if ((b & 0x80) == 0)
		Fgl_setpixel (xx + x, yy + y, text_bg);
	    else
		Fgl_setpixel (xx + x, yy + y, text_fg);
	    b = b << 1;
	}
    }
}


void
Fgl_fillbox (int x1, int y1, int w, int h, int col)
{
    RECT rect;
    if (clipping_flag) {
	if (x1 < xclip_x1)
	    x1 = xclip_x1;
	if (x1 + w > xclip_x2)
	    w = xclip_x2 - x1;
	if (y1 < xclip_y1)
	    y1 = xclip_y1;
	if (y1 + h > xclip_y2)
	    h = xclip_y2 - y1;
    }
    col &= 0xff;
    pixmap_fillbox (x1, y1, w, h, col);

    // Draw to DC
    if (display.useDIB) {
	HBITMAP hOldBitmapMem;
	HBRUSH hbr, hbrOld;
	rect.left = x1 + borderx;
	rect.top = y1 + bordery;
	rect.right = x1 + borderx + w;
	rect.bottom = y1 + bordery + h;
	hbr = GetPaletteBrush (col);
	hbrOld = (HBRUSH) SelectObject (display.hdcMem, hbr);
	hOldBitmapMem = (HBITMAP) SelectObject (display.hdcMem, display.hDIB);
	FillRect (display.hdcMem, &rect, hbr);
	display.hDIB = (HBITMAP) SelectObject (display.hdcMem, hOldBitmapMem);
	hbr = (HBRUSH) SelectObject (display.hdcMem, hbrOld);
    } else {
	HBRUSH hbr, hbrOld;
	rect.left = AdjustX (x1);
	rect.top = AdjustY (y1);
	rect.right = AdjustX (x1 + w);
	rect.bottom = AdjustY (y1 + h);
	hbr = GetPaletteBrush (col);
	hbrOld = (HBRUSH) SelectObject (display.hdcMem, hbr);
	FillRect (display.hdcMem, &rect, hbr);
	hbr = (HBRUSH) SelectObject (display.hdcMem, hbrOld);
    }

    // Queue update event
    InvalidateRect (display.hWnd, &rect, FALSE);
}

void
Fgl_putbox (int x1, int y1, int w, int h, void *buf)
{
    unsigned char *b = (unsigned char *) buf;
    RECT rect;
    int x, y;
    for (y = y1; y < y1 + h; y++)
	for (x = x1; x < x1 + w; x++)
	    Fgl_setpixel (x, y, *(b++));

    // Queue update event
    rect.left = AdjustX (x1);
    rect.top = AdjustY (y1);
    rect.right = AdjustX (x1 + w);
    rect.bottom = AdjustY (y1 + h);
    InvalidateRect (display.hWnd, &rect, FALSE);
    // GCS: Don't update window here -- it causes farms to jump
    //      around when panning main screen
    //UpdateWindow (display.hWnd);
}

void
Fgl_getbox (int x1, int y1, int w, int h, void *buf)
{
    unsigned char *b = (unsigned char *) buf;
    int x, y;
    for (y = y1; y < y1 + h; y++)
        for (x = x1; x < x1 + w; x++)
	    *(b++) = (unsigned char) Fgl_getpixel (x, y);
}

void
RefreshScreen ()
{
    InvalidateRect (display.hWnd, NULL, FALSE);
    ProcessNextEvent ();
}

void
RefreshArea (int x1, int y1, int x2, int y2)	// bounds of refresh area
{
    RECT rect;
    rect.left = AdjustX (x1);
    rect.top = AdjustY (y1);
    rect.right = AdjustX (x2);
    rect.bottom = AdjustY (y2);
    InvalidateRect (display.hWnd, &rect, FALSE);
    UpdateWindow (display.hWnd);
}

void
Fgl_enableclipping (void)
{
    clipping_flag = 1;
}

void
Fgl_setclippingwindow (int x1, int y1, int x2, int y2)
{
    xclip_x1 = x1;
    xclip_y1 = y1;
    xclip_x2 = x2;
    xclip_y2 = y2;
}

void
Fgl_disableclipping (void)
{
    clipping_flag = 0;
}

void
do_call_event (int wait)
{
    if (wait)
        lc_usleep (1000);
    HandleMouse ();
}

void
call_event (void)
{
    do_call_event (0);
}

void
call_wait_event (void)
{
    do_call_event (1);
}

#ifdef USE_PIXMAPS
void
init_icon_pixmap (short type)
{
    unsigned char *g;
    int x, y;
    HBITMAP hOldBitmapMem;
    int w, h;
    int grp;

    grp = get_group_of_type(type);

    if( grp >= 0) {
	w = main_groups[grp].size * 16;
	h = main_groups[grp].size * 16;
	w <<= pix_double;
	h <<= pix_double;
	icon_pixmap[type] = CreateCompatibleBitmap (display.hdcMem, w, h);
	hOldBitmapMem = (HBITMAP) SelectObject (display.hdcMem, icon_pixmap[type]);

	/* Copy bits to bitmap */
	g = (unsigned char *) main_types[type].graphic;
	for (y = 0; y < main_groups[grp].size * 16; y++) {
	    for (x = 0; x < main_groups[grp].size * 16; x++) {
		if (pix_double) {
		    RECT rect;
		    HBRUSH hbr, hbrOld;
		    rect.left = x << 1;
		    rect.top = y << 1;
		    rect.right = (x + 1) << 1;
		    rect.bottom = (y + 1) << 1;
		    hbr = GetPaletteBrush (*g++);
		    hbrOld = (HBRUSH) SelectObject (display.hdcMem, hbr);
		    FillRect (display.hdcMem, &rect, hbr);
		    hbr = (HBRUSH) SelectObject (display.hdcMem, hbrOld);
		} else {
		    SetPixel (display.hdcMem, x, y, GetPaletteColorref (*g++));
		}
	    }
	}
	icon_pixmap[type] = (HBITMAP) SelectObject (display.hdcMem, hOldBitmapMem);
    }
}
#endif

int 
lc_get_keystroke (void)
{
    return GetKeystroke ();
}

void
draw_border (void)
{
}

void
init_mouse (void)
{
}
