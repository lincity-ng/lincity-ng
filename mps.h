/* ---------------------------------------------------------------------- *
 * mps.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#ifndef __mps_h__
#define __mps_h__

#ifdef old_mps
/* Note: these should not overlap with the LC_MOUSE_BUTTON defines.
   See mappoint_stats() for details.  Sorry about the bad hack (GCS). */
#define MPS_GLOBAL_FINANCE 0x10
#define MPS_GLOBAL_OTHER_COSTS 0x11
#define MPS_GLOBAL_HOUSING 0x12

#define MPS_GLOBAL_STYLE_MIN MPS_GLOBAL_FINANCE
#define MPS_GLOBAL_STYLE_MAX MPS_GLOBAL_HOUSING

#endif

#include "geometry.h"
#include "cliglobs.h"

void mps_full_refresh (void);
void mappoint_stats (int, int, int);

/* New, simplified mps routines */

#define MPS_MAP 0 /* Left click on map; x/y module statistics */
#define MPS_ENV 1 /* Right click on map; x/y environmental status */
#define MPS_GLOBAL 2 /* Global style; choose one of below */

#define MPS_GLOBAL_FINANCE 0 /* Overall financial information */
#define MPS_GLOBAL_OTHER_COSTS 1 /* More detailed financials */
#define MPS_GLOBAL_HOUSING 2 /* Citywide population information */

#define MPS_GLOBAL_STYLES 3 /* Number of global styles */

#define MPS_INFO_CHARS (MAPPOINT_STATS_W / 8) + 1

void mps_init();

void mps_set(int style, int x, int y); /* Attaches an area or global display */
void mps_redraw(void);  /* Re-draw the mps area, bezel and all */
void mps_refresh(void); /* refresh the information display's contents */
void mps_update(void);  /* Update text contents for later display (refresh) */
void mps_global_advance(void); /* Changes global var to next display */


/* mps_info storage functions; place values of corresponding type into
   mps_info[], performing certain pretification. The single argument 
   forms center their argument.  The dual arguments left-justify the
   first and right-justify the second.  the ..p forms put a % after
   the second argument 
 */
void mps_store_title(int i, char * t);
void mps_store_ss(int i, char * s1, char * s2);
void mps_store_sss(int i, char * s1, char * s2, char * s3);
void mps_store_sd(int i, char * s, int d);
void mps_store_sfp(int i, char * s, double fl);

/* Data for new mps routines */
extern char mps_info[MAPPOINT_STATS_LINES][MPS_INFO_CHARS];
extern int mps_global_style;

/* MPS Global displays */
void mps_global_finance(void);

#define _generic_mps
#ifdef _generic_mps
#endif


#endif /* __mps_h__ */

