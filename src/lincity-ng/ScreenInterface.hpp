#ifndef __lc_screen_h__
#define __lc_screen_h__

#include "lincity/engglobs.h"
#include "lincity/lctypes.h"

void screen_full_refresh ();
void print_stats ();

void update_main_screen (int full_refresh);
void updateMessageTitle();

#endif
