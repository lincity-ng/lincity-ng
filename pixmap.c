/* ---------------------------------------------------------------------- *
 * pixmap.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
/* The pixmap contains only the main screen, not the border.
 * When pix doubled, pixmap does contain the doubled pixels.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "lcstring.h"

#include "lin-city.h"
#include "lctypes.h"
#include "cliglobs.h"

#if defined (LC_X11)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include "lcx11.h"
#endif


#define USE_IMAGES 1

/* ---------------------------------------------------------------------- *
 * Private global variables
 * ---------------------------------------------------------------------- */
char* pixmap = NULL;
int pixmap_height = 0;
int pixmap_width = 0;


/* ---------------------------------------------------------------------- *
 * Public functions
 * ---------------------------------------------------------------------- */
#ifdef USE_PIXMAPS
void
init_pixmaps ()
{
    int i;
    for (i = 1; i < NUM_OF_TYPES; i++) {
	icon_pixmap[i] = 0;
    }

    init_icon_pixmap (CST_GREEN);
    /* powerlines */
    init_icon_pixmap (CST_POWERL_H_L);
    init_icon_pixmap (CST_POWERL_V_L);
    init_icon_pixmap (CST_POWERL_LD_L);
    init_icon_pixmap (CST_POWERL_RD_L);
    init_icon_pixmap (CST_POWERL_LU_L);
    init_icon_pixmap (CST_POWERL_RU_L);
    init_icon_pixmap (CST_POWERL_LDU_L);
    init_icon_pixmap (CST_POWERL_LDR_L);
    init_icon_pixmap (CST_POWERL_LUR_L);
    init_icon_pixmap (CST_POWERL_UDR_L);
    init_icon_pixmap (CST_POWERL_LUDR_L);
    prog_box ("", 6);
    init_icon_pixmap (CST_POWERL_H_D);
    init_icon_pixmap (CST_POWERL_V_D);
    init_icon_pixmap (CST_POWERL_LD_D);
    init_icon_pixmap (CST_POWERL_RD_D);
    init_icon_pixmap (CST_POWERL_LU_D);
    init_icon_pixmap (CST_POWERL_RU_D);
    init_icon_pixmap (CST_POWERL_LDU_D);
    init_icon_pixmap (CST_POWERL_LDR_D);
    init_icon_pixmap (CST_POWERL_LUR_D);
    init_icon_pixmap (CST_POWERL_UDR_D);
    init_icon_pixmap (CST_POWERL_LUDR_D);
    /* HT windmills */
    init_icon_pixmap (CST_WINDMILL_1_G);
    init_icon_pixmap (CST_WINDMILL_2_G);
    init_icon_pixmap (CST_WINDMILL_3_G);
    prog_box ("", 12);
    init_icon_pixmap (CST_WINDMILL_1_RG);
    init_icon_pixmap (CST_WINDMILL_2_RG);
    init_icon_pixmap (CST_WINDMILL_3_RG);
    init_icon_pixmap (CST_WINDMILL_1_R);
    init_icon_pixmap (CST_WINDMILL_2_R);
    init_icon_pixmap (CST_WINDMILL_3_R);
    /*LT windmills */
    init_icon_pixmap (CST_WINDMILL_1_W);
    init_icon_pixmap (CST_WINDMILL_2_W);
    init_icon_pixmap (CST_WINDMILL_3_W);
    /* communes */
    init_icon_pixmap (CST_COMMUNE_1);
    init_icon_pixmap (CST_COMMUNE_2);
    init_icon_pixmap (CST_COMMUNE_3);
    init_icon_pixmap (CST_COMMUNE_4);
    init_icon_pixmap (CST_COMMUNE_5);
    prog_box ("", 18);
    init_icon_pixmap (CST_COMMUNE_6);
    init_icon_pixmap (CST_COMMUNE_7);
    init_icon_pixmap (CST_COMMUNE_8);
    init_icon_pixmap (CST_COMMUNE_9);
    init_icon_pixmap (CST_COMMUNE_10);
    init_icon_pixmap (CST_COMMUNE_11);
    init_icon_pixmap (CST_COMMUNE_12);
    init_icon_pixmap (CST_COMMUNE_13);
    init_icon_pixmap (CST_COMMUNE_14);
    /* farms   (3 7 11 and 15 are the only ones needed) */
    init_icon_pixmap (CST_FARM_O3);
    init_icon_pixmap (CST_FARM_O7);
    init_icon_pixmap (CST_FARM_O11);
    init_icon_pixmap (CST_FARM_O15);
    /* Lt. Industry */
    prog_box ("", 24);
    init_icon_pixmap (CST_INDUSTRY_L_C);
    init_icon_pixmap (CST_INDUSTRY_L_Q1);
    init_icon_pixmap (CST_INDUSTRY_L_Q2);
    init_icon_pixmap (CST_INDUSTRY_L_Q3);
    init_icon_pixmap (CST_INDUSTRY_L_Q4);
    init_icon_pixmap (CST_INDUSTRY_L_L1);
    init_icon_pixmap (CST_INDUSTRY_L_L2);
    init_icon_pixmap (CST_INDUSTRY_L_L3);
    init_icon_pixmap (CST_INDUSTRY_L_L4);
    init_icon_pixmap (CST_INDUSTRY_L_M1);
    init_icon_pixmap (CST_INDUSTRY_L_M2);
    init_icon_pixmap (CST_INDUSTRY_L_M3);
    init_icon_pixmap (CST_INDUSTRY_L_M4);
    init_icon_pixmap (CST_INDUSTRY_L_H1);
    init_icon_pixmap (CST_INDUSTRY_L_H2);
    prog_box ("", 30);
    init_icon_pixmap (CST_INDUSTRY_L_H3);
    init_icon_pixmap (CST_INDUSTRY_L_H4);
    /* Hv. Industry */
    init_icon_pixmap (CST_INDUSTRY_H_C);
    init_icon_pixmap (CST_INDUSTRY_H_L1);
    init_icon_pixmap (CST_INDUSTRY_H_L2);
    init_icon_pixmap (CST_INDUSTRY_H_L3);
    init_icon_pixmap (CST_INDUSTRY_H_L4);
    init_icon_pixmap (CST_INDUSTRY_H_L5);
    init_icon_pixmap (CST_INDUSTRY_H_L6);
    init_icon_pixmap (CST_INDUSTRY_H_L7);
    init_icon_pixmap (CST_INDUSTRY_H_L8);
    init_icon_pixmap (CST_INDUSTRY_H_M1);
    init_icon_pixmap (CST_INDUSTRY_H_M2);
    init_icon_pixmap (CST_INDUSTRY_H_M3);
    init_icon_pixmap (CST_INDUSTRY_H_M4);
    prog_box ("", 36);
    init_icon_pixmap (CST_INDUSTRY_H_M5);
    init_icon_pixmap (CST_INDUSTRY_H_M6);
    init_icon_pixmap (CST_INDUSTRY_H_M7);
    init_icon_pixmap (CST_INDUSTRY_H_M8);
    init_icon_pixmap (CST_INDUSTRY_H_H1);
    init_icon_pixmap (CST_INDUSTRY_H_H2);
    init_icon_pixmap (CST_INDUSTRY_H_H3);
    init_icon_pixmap (CST_INDUSTRY_H_H4);
    init_icon_pixmap (CST_INDUSTRY_H_H5);
    init_icon_pixmap (CST_INDUSTRY_H_H6);
    init_icon_pixmap (CST_INDUSTRY_H_H7);
    init_icon_pixmap (CST_INDUSTRY_H_H8);
    /* water */
    prog_box ("", 42);
    init_icon_pixmap (CST_WATER);
    init_icon_pixmap (CST_WATER_D);
    init_icon_pixmap (CST_WATER_R);
    init_icon_pixmap (CST_WATER_U);
    init_icon_pixmap (CST_WATER_L);
    init_icon_pixmap (CST_WATER_LR);
    init_icon_pixmap (CST_WATER_UD);
    init_icon_pixmap (CST_WATER_LD);
    init_icon_pixmap (CST_WATER_RD);
    init_icon_pixmap (CST_WATER_LU);
    init_icon_pixmap (CST_WATER_UR);
    init_icon_pixmap (CST_WATER_LUD);
    init_icon_pixmap (CST_WATER_LRD);
    init_icon_pixmap (CST_WATER_LUR);
    init_icon_pixmap (CST_WATER_URD);
    init_icon_pixmap (CST_WATER_LURD);
    /* tracks */
    init_icon_pixmap (CST_TRACK_LR);
    prog_box ("", 48);
    init_icon_pixmap (CST_TRACK_LU);
    init_icon_pixmap (CST_TRACK_LD);
    init_icon_pixmap (CST_TRACK_UD);
    init_icon_pixmap (CST_TRACK_UR);
    init_icon_pixmap (CST_TRACK_DR);
    init_icon_pixmap (CST_TRACK_LUR);
    init_icon_pixmap (CST_TRACK_LDR);
    init_icon_pixmap (CST_TRACK_LUD);
    init_icon_pixmap (CST_TRACK_UDR);
    init_icon_pixmap (CST_TRACK_LUDR);
    /* roads */
    init_icon_pixmap (CST_ROAD_LR);
    init_icon_pixmap (CST_ROAD_LU);
    init_icon_pixmap (CST_ROAD_LD);
    init_icon_pixmap (CST_ROAD_UD);
    init_icon_pixmap (CST_ROAD_UR);
    init_icon_pixmap (CST_ROAD_DR);
    init_icon_pixmap (CST_ROAD_LUR);
    init_icon_pixmap (CST_ROAD_LDR);
    prog_box ("", 54);
    init_icon_pixmap (CST_ROAD_LUD);
    init_icon_pixmap (CST_ROAD_UDR);
    init_icon_pixmap (CST_ROAD_LUDR);
    /* rail */
    init_icon_pixmap (CST_RAIL_LR);
    init_icon_pixmap (CST_RAIL_LU);
    init_icon_pixmap (CST_RAIL_LD);
    init_icon_pixmap (CST_RAIL_UD);
    init_icon_pixmap (CST_RAIL_UR);
    init_icon_pixmap (CST_RAIL_DR);
    init_icon_pixmap (CST_RAIL_LUR);
    init_icon_pixmap (CST_RAIL_LDR);
    init_icon_pixmap (CST_RAIL_LUD);
    init_icon_pixmap (CST_RAIL_UDR);
    init_icon_pixmap (CST_RAIL_LUDR);
    /* potteries */
    prog_box ("", 60);
    init_icon_pixmap (CST_POTTERY_0);
    init_icon_pixmap (CST_POTTERY_1);
    init_icon_pixmap (CST_POTTERY_2);
    init_icon_pixmap (CST_POTTERY_3);
    init_icon_pixmap (CST_POTTERY_4);
    init_icon_pixmap (CST_POTTERY_5);
    init_icon_pixmap (CST_POTTERY_6);
    init_icon_pixmap (CST_POTTERY_7);
    init_icon_pixmap (CST_POTTERY_8);
    init_icon_pixmap (CST_POTTERY_9);
    init_icon_pixmap (CST_POTTERY_10);
    /* mills */
    init_icon_pixmap (CST_MILL_0);
    init_icon_pixmap (CST_MILL_1);
    init_icon_pixmap (CST_MILL_2);
    init_icon_pixmap (CST_MILL_3);
    init_icon_pixmap (CST_MILL_4);
    prog_box ("", 66);
    init_icon_pixmap (CST_MILL_5);
    init_icon_pixmap (CST_MILL_6);
    /* blacksmiths */
    init_icon_pixmap (CST_BLACKSMITH_0);
    init_icon_pixmap (CST_BLACKSMITH_1);
    init_icon_pixmap (CST_BLACKSMITH_2);
    init_icon_pixmap (CST_BLACKSMITH_3);
    init_icon_pixmap (CST_BLACKSMITH_4);
    init_icon_pixmap (CST_BLACKSMITH_5);
    init_icon_pixmap (CST_BLACKSMITH_6);
    /* residences */
    init_icon_pixmap (CST_RESIDENCE_LL);
    init_icon_pixmap (CST_RESIDENCE_ML);
    init_icon_pixmap (CST_RESIDENCE_HL);
    init_icon_pixmap (CST_RESIDENCE_LH);
    init_icon_pixmap (CST_RESIDENCE_MH);
    init_icon_pixmap (CST_RESIDENCE_HH);
    /* coal power */
    init_icon_pixmap (CST_POWERS_COAL_EMPTY);
    init_icon_pixmap (CST_POWERS_COAL_LOW);
    prog_box ("", 72);
    init_icon_pixmap (CST_POWERS_COAL_MED);
    init_icon_pixmap (CST_POWERS_COAL_FULL);
    /* substations */
    init_icon_pixmap (CST_SUBSTATION_R);
    init_icon_pixmap (CST_SUBSTATION_G);
    init_icon_pixmap (CST_SUBSTATION_RG);
    /* markets */
    init_icon_pixmap (CST_MARKET_EMPTY);
    init_icon_pixmap (CST_MARKET_LOW);
    init_icon_pixmap (CST_MARKET_MED);
    init_icon_pixmap (CST_MARKET_FULL);
    /* coal mines */
    init_icon_pixmap (CST_COALMINE_EMPTY);
    init_icon_pixmap (CST_COALMINE_LOW);
    init_icon_pixmap (CST_COALMINE_MED);
    init_icon_pixmap (CST_COALMINE_FULL);
    /* ore mines */
    prog_box ("", 78);
    init_icon_pixmap (CST_OREMINE_1);
    init_icon_pixmap (CST_OREMINE_2);
    init_icon_pixmap (CST_OREMINE_3);
    init_icon_pixmap (CST_OREMINE_4);
    init_icon_pixmap (CST_OREMINE_5);
    init_icon_pixmap (CST_OREMINE_6);
    init_icon_pixmap (CST_OREMINE_7);
    init_icon_pixmap (CST_OREMINE_8);
    /* tips */
    init_icon_pixmap (CST_TIP_0);
    init_icon_pixmap (CST_TIP_1);
    init_icon_pixmap (CST_TIP_2);
    init_icon_pixmap (CST_TIP_3);
    init_icon_pixmap (CST_TIP_4);
    init_icon_pixmap (CST_TIP_5);
    init_icon_pixmap (CST_TIP_6);
    init_icon_pixmap (CST_TIP_7);
    init_icon_pixmap (CST_TIP_8);
    /* rockets */
    prog_box ("", 84);
    init_icon_pixmap (CST_ROCKET_1);
    init_icon_pixmap (CST_ROCKET_2);
    init_icon_pixmap (CST_ROCKET_3);
    init_icon_pixmap (CST_ROCKET_4);
    init_icon_pixmap (CST_ROCKET_5);
    init_icon_pixmap (CST_ROCKET_6);
    init_icon_pixmap (CST_ROCKET_7);
    init_icon_pixmap (CST_ROCKET_FLOWN);
    /* fire stations */
    init_icon_pixmap (CST_FIRESTATION_1);
    init_icon_pixmap (CST_FIRESTATION_2);
    init_icon_pixmap (CST_FIRESTATION_3);
    init_icon_pixmap (CST_FIRESTATION_4);
    init_icon_pixmap (CST_FIRESTATION_5);
    init_icon_pixmap (CST_FIRESTATION_6);
    init_icon_pixmap (CST_FIRESTATION_7);
    init_icon_pixmap (CST_FIRESTATION_8);
    init_icon_pixmap (CST_FIRESTATION_9);
    init_icon_pixmap (CST_FIRESTATION_10);
    /* cricket */
    prog_box ("", 90);
    init_icon_pixmap (CST_CRICKET_1);
    init_icon_pixmap (CST_CRICKET_2);
    init_icon_pixmap (CST_CRICKET_3);
    init_icon_pixmap (CST_CRICKET_4);
    init_icon_pixmap (CST_CRICKET_5);
    init_icon_pixmap (CST_CRICKET_6);
    init_icon_pixmap (CST_CRICKET_7);
    /* fire */
    init_icon_pixmap (CST_FIRE_1);
    init_icon_pixmap (CST_FIRE_2);
    init_icon_pixmap (CST_FIRE_3);
    init_icon_pixmap (CST_FIRE_4);
    init_icon_pixmap (CST_FIRE_5);
    init_icon_pixmap (CST_FIRE_DONE1);
    init_icon_pixmap (CST_FIRE_DONE2);
    init_icon_pixmap (CST_FIRE_DONE3);
    init_icon_pixmap (CST_FIRE_DONE4);

    /* others */
    init_icon_pixmap (CST_PARKLAND_PLANE);
    init_icon_pixmap (CST_RECYCLE);
    init_icon_pixmap (CST_HEALTH);
    init_icon_pixmap (CST_BURNT);
    init_icon_pixmap (CST_MONUMENT_5);
    init_icon_pixmap (CST_SCHOOL);
    init_icon_pixmap (CST_SHANTY);
    prog_box ("", 96);
    init_icon_pixmap (CST_POWERS_SOLAR);
    init_icon_pixmap (CST_UNIVERSITY);
    init_icon_pixmap (CST_EX_PORT);
}

void
initialize_pixmap (void)
{
    int i;
    pixmap_height = display.winH - 2*borderx;
    pixmap_width = display.winW - 2*bordery;
    pixmap = (char*) malloc (pixmap_height * pixmap_width * sizeof(char));
    for (i = 0; i < pixmap_height * pixmap_width; i++) {
	pixmap[i] = 0;
    }
}

void
free_pixmap (void)
{
    free (pixmap);
    pixmap = 0;
}

void
resize_pixmap (int new_width, int new_height)
{
    int i;
    if (new_width > pixmap_width || new_height > pixmap_height) {
	char* new_pixmap;
	if (new_width < pixmap_width) new_width = pixmap_width;
	if (new_height < pixmap_height) new_height = pixmap_height;
	new_pixmap = (char*) malloc (new_height * new_width * sizeof(char));
	for (i = 0; i < pixmap_height * pixmap_width; i++) {
	    pixmap[i] = 0;
	}
	free (pixmap);
	pixmap = new_pixmap;
	pixmap_width = new_width;
	pixmap_height = new_height;
    }
}
/* KBR 10/19/2002 MSVC was not happy with this being inline for release build */
int 
pixmap_index (int x, int y)
{
    return y*pixmap_width + x;
}

int 
pixmap_getpixel (int x, int y)
{
    return (int) pixmap[pixmap_index(x,y)];
}

void
pixmap_setpixel (int x, int y, int col)
{
    pixmap[pixmap_index(x,y)] = col;
}

void
pixmap_hline (int x1, int y1, int x2, int col)
{
    int i, j;
    i = pixmap_index (x1, y1);
    for (j = 0; j <= x2 - x1; j++)
	pixmap[i+j] = col;
}

void
pixmap_vline (int x1, int y1, int y2, int col)
{
    int i, j;
    i = pixmap_index (x1, y1);
    for (j = 0; j <= y2 - y1; j++)
	pixmap[i+j*pixmap_width] = col;
}

void
pixmap_fillbox (int x, int y, int w, int h, int col)
{
    int xx, yy;
    int i = pixmap_index (x, y);
    for (yy = y; yy < y + h; yy++) {
	for (xx = x; xx < x + w; xx++) {
	    pixmap[i++] = col;
	}
	i += pixmap_width - w;
    }
}

void
pixmap_putbox (char* src, int src_x, int src_y, int src_w,
	       int dst_x, int dst_y, int w, int h)
{
    int x, y;
    int i = pixmap_index (dst_x, dst_y);
    int j = src_x + src_y * src_w;
    for (y = src_y; y < src_y + h; y++) {
	for (x = src_x; x < src_x + w; x++) {
	    pixmap[i++] = src[j++];
	}
	i += pixmap_width - w;
	j += src_w - w;
    }
}

void
update_pixmap (int x1, int y1, int sizex, int sizey, int dx, int dy,
	       int real_size, char *g)
{
    /* Copy graphic from g into pixmap */
    char *i, *j;
    int x, y;
    for (y = 0; y < sizey; y++) {
	i = pixmap + pixmap_index (dx, dy + y);
	j = (g + ((y1 + y) * real_size * 16 + x1));
	for (x = 0; x < sizex; x++)
	    *(i++) = *(j++);
    }
}

#endif /* USE_PIXMAPS */
