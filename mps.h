/* ---------------------------------------------------------------------- *
 * mps.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#ifndef __mps_h__
#define __mps_h__

/* Note: these should not overlap with the LC_MOUSE_BUTTON defines.
   See mappoint_stats() for details.  Sorry about the bad hack (GCS). */
#define MPS_GLOBAL_FINANCE 0x10
#define MPS_GLOBAL_OTHER_COSTS 0x11
#define MPS_GLOBAL_HOUSING 0x12

#define MPS_GLOBAL_STYLE_MIN MPS_GLOBAL_FINANCE
#define MPS_GLOBAL_STYLE_MAX MPS_GLOBAL_HOUSING

#include "cliglobs.h"

void mps_full_refresh (void);
void mappoint_stats (int, int, int);

#define _generic_mps
#ifdef _generic_mps
#endif


#endif /* __mps_h__ */
