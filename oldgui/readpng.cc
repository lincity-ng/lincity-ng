/* ---------------------------------------------------------------------- *
 * readpng.c
 * This file is part of lincity (see COPYRIGHT for copyright information).
 * ---------------------------------------------------------------------- */
#include <stdlib.h>
#include "png.h"
#include "malloc.h"
#include "lin-city.h"
#include "lctypes.h"
#include "lintypes.h"

/* Read a PNG file.  You may want to return an error code if the read
 * fails (depending upon the failure).  There are two "prototypes" given
 * here - one where we are given the filename, and we need to open the
 * file, and the other where we are given an open file (possibly with
 * some or all of the magic bytes read - see comments above).
 */
#define ERROR -1
#define OK 0

/* Private functions */
static char* 
load_png_graphic (short type, short group, char* id,
		  FILE* txt_fp,png_bytep *row_pointers, 
		  png_uint_32 width, png_uint_32 height);

/* Let errors and warnings be handled by setjmp/longjmp */
void* user_error_ptr = 0;

void user_error_fn(png_structp png_ptr, png_const_charp error_msg)
{
}

void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
}

int
load_png_graphics (char *txt_filename, char *png_filename)
{
    png_uint_32 row;
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;
    FILE *fp, *txt_fp;
    png_bytep *row_pointers;

    if ((fp = fopen(png_filename, "rb")) == NULL)
	return (ERROR);

    if ((txt_fp = fopen(txt_filename, "r")) == NULL)
	return (ERROR);

    /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also supply the
    * the compiler header file version, so that we know if the application
    * was compiled with a compatible version of the library.  REQUIRED
    */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
	(png_voidp) user_error_ptr, user_error_fn, user_warning_fn);

    if (png_ptr == NULL) {
	fclose(fp);
	return (ERROR);
    }

    /* Allocate/initialize the memory for image information.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
	fclose(fp);
#if defined (commentout)
	png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
#endif
	png_destroy_read_struct(&png_ptr, NULL, NULL);
	return (ERROR);
    }

   /* Set error handling if you are using the setjmp/longjmp method (this is
    * the normal method of doing things with libpng).  REQUIRED unless you
    * set up your own error handlers in the png_create_read_struct() earlier.
    */
    if (setjmp(png_jmpbuf(png_ptr))) {
	/* Free all of the memory associated with the png_ptr and info_ptr */
#if defined (commentout)
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
#endif
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);
	/* If we get here, we had a problem reading the file */
	return (ERROR);
    }

    /* Set up the input control if you are using standard C streams */
    png_init_io(png_ptr, fp);

    /* If we have already read some of the signature */
    png_set_sig_bytes(png_ptr, sig_read);

    /* OK, you're doing it the hard way, with the lower-level functions */
    /* The call to png_read_info() gives us all of the information from the
    * PNG file before the first IDAT (image data chunk).  REQUIRED
    */
    png_read_info(png_ptr, info_ptr);

#if defined (commentout)
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
	&interlace_type, int_p_NULL, int_p_NULL);
#endif
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
	&interlace_type, NULL, NULL);

    printf ("PNG Header: %d x %d, bd=%d, ct=%d\n", (int)height, (int)width,
	bit_depth, color_type);

    /* Set up the data transformations you want.  Note that these are all
     * optional.  Only call them if you want/need them.  Many of the
     * transformations only work on specific types of images, and many
     * are mutually exclusive.
     */

   /* tell libpng to strip 16 bit/color files down to 8 bits/color */
//   png_set_strip_16(png_ptr);

   /* Strip alpha bytes from the input data without combining with the
    * background (not recommended).
    */
//   png_set_strip_alpha(png_ptr);

   /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
    * byte into separate bytes (useful for paletted and grayscale images).
    */
//   png_set_packing(png_ptr);

   /* Change the order of packed pixels to least significant bit first
    * (not useful if you are using png_set_packing). */
//   png_set_packswap(png_ptr);

   /* Require color fmt w/ palette */
   if (color_type != PNG_COLOR_TYPE_PALETTE) {
	printf ("Error - png image wasn't PNG_COLOR_TYPE_PALETTE\n");
	/* Free all of the memory associated with the png_ptr and info_ptr */
#if defined (commentout)
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
#endif
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);
	/* If we get here, we had a problem reading the file */
	return (ERROR);
   }

   /* Require 1 byte per pixel */
   if (bit_depth != 8) {
	printf ("Error - png image wasn't bit_depth = 8\n");
	/* Free all of the memory associated with the png_ptr and info_ptr */
#if defined (commentout)
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
#endif
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);
	/* If we get here, we had a problem reading the file */
	return (ERROR);
   }

    /* The easiest way to read the image: */
   row_pointers = (png_byte**)malloc (sizeof(void*)*height);
    for (row = 0; row < height; row++) {
      row_pointers[row] = (png_byte*)png_malloc(png_ptr, 
	    png_get_rowbytes(png_ptr, info_ptr));
    }

    png_read_image(png_ptr, row_pointers);

#if defined (commentout)
    for (col = 0; col < 16; col++) {
	printf (" %02x ",row_pointers[0][col]);
    }
    printf ("\n");

    for (col = 0; col < 16; col++) {
	printf ("%3d ",row_pointers[0][col]);
    }
    printf ("\n");
#endif

    /* read rest of file, and get additional chunks in info_ptr - REQUIRED */
    png_read_end(png_ptr, info_ptr);

    /* close the png file */
    fclose(fp);

    /* get the icons out of the png */
#ifdef LG
#undef LG
#endif
#define LG(typ,grp,id) load_png_graphic(typ,grp,id,txt_fp,row_pointers,height,width)

    LG(CST_GREEN,GROUP_BARE,LCT_GREEN_G);

    LG(CST_FIRE_1,GROUP_FIRE,LCT_FIRE_1_G);
    LG(CST_FIRE_2,GROUP_FIRE,LCT_FIRE_2_G);
    LG(CST_FIRE_3,GROUP_FIRE,LCT_FIRE_3_G);
    LG(CST_FIRE_4,GROUP_FIRE,LCT_FIRE_4_G);
    LG(CST_FIRE_5,GROUP_FIRE,LCT_FIRE_5_G);

    LG(CST_FIRE_DONE1,GROUP_FIRE,LCT_FIRE_DONE1_G);
    LG(CST_FIRE_DONE2,GROUP_FIRE,LCT_FIRE_DONE2_G);
    LG(CST_FIRE_DONE3,GROUP_FIRE,LCT_FIRE_DONE3_G);
    LG(CST_FIRE_DONE4,GROUP_FIRE,LCT_FIRE_DONE4_G);

    LG(CST_BURNT,GROUP_BURNT,LCT_BURNT_G);

    LG(CST_PARKLAND_PLANE,GROUP_PARKLAND,LCT_PARKLAND_PLANE_G);
    LG(CST_PARKLAND_LAKE,GROUP_PARKLAND,LCT_PARKLAND_LAKE_G);

    LG(CST_POWERL_H_L,GROUP_POWER_LINE,LCT_POWERL_H_L_G);
    LG(CST_POWERL_V_L,GROUP_POWER_LINE,LCT_POWERL_V_L_G);
    LG(CST_POWERL_LD_L,GROUP_POWER_LINE,LCT_POWERL_LD_L_G);
    LG(CST_POWERL_RD_L,GROUP_POWER_LINE,LCT_POWERL_RD_L_G);
    LG(CST_POWERL_LU_L,GROUP_POWER_LINE,LCT_POWERL_LU_L_G);
    LG(CST_POWERL_RU_L,GROUP_POWER_LINE,LCT_POWERL_RU_L_G);
    LG(CST_POWERL_LDU_L,GROUP_POWER_LINE,LCT_POWERL_LDU_L_G);
    LG(CST_POWERL_LDR_L,GROUP_POWER_LINE,LCT_POWERL_LDR_L_G);
    LG(CST_POWERL_LUR_L,GROUP_POWER_LINE,LCT_POWERL_LUR_L_G);
    LG(CST_POWERL_UDR_L,GROUP_POWER_LINE,LCT_POWERL_UDR_L_G);
    LG(CST_POWERL_LUDR_L,GROUP_POWER_LINE,LCT_POWERL_LUDR_L_G);
    LG(CST_POWERL_H_D,GROUP_POWER_LINE,LCT_POWERL_H_D_G);
    LG(CST_POWERL_V_D,GROUP_POWER_LINE,LCT_POWERL_V_D_G);
    LG(CST_POWERL_LD_D,GROUP_POWER_LINE,LCT_POWERL_LD_D_G);
    LG(CST_POWERL_RD_D,GROUP_POWER_LINE,LCT_POWERL_RD_D_G);
    LG(CST_POWERL_LU_D,GROUP_POWER_LINE,LCT_POWERL_LU_D_G);
    LG(CST_POWERL_RU_D,GROUP_POWER_LINE,LCT_POWERL_RU_D_G);
    LG(CST_POWERL_LDU_D,GROUP_POWER_LINE,LCT_POWERL_LDU_D_G);
    LG(CST_POWERL_LDR_D,GROUP_POWER_LINE,LCT_POWERL_LDR_D_G);
    LG(CST_POWERL_LUR_D,GROUP_POWER_LINE,LCT_POWERL_LUR_D_G);
    LG(CST_POWERL_UDR_D,GROUP_POWER_LINE,LCT_POWERL_UDR_D_G);
    LG(CST_POWERL_LUDR_D,GROUP_POWER_LINE,LCT_POWERL_LUDR_D_G);

    LG(CST_RAIL_LR,GROUP_RAIL,LCT_RAIL_LR_G);
    LG(CST_RAIL_LU,GROUP_RAIL,LCT_RAIL_LU_G);
    LG(CST_RAIL_LD,GROUP_RAIL,LCT_RAIL_LD_G);
    LG(CST_RAIL_UD,GROUP_RAIL,LCT_RAIL_UD_G);
    LG(CST_RAIL_UR,GROUP_RAIL,LCT_RAIL_UR_G);
    LG(CST_RAIL_DR,GROUP_RAIL,LCT_RAIL_DR_G);
    LG(CST_RAIL_LUR,GROUP_RAIL,LCT_RAIL_LUR_G);
    LG(CST_RAIL_LDR,GROUP_RAIL,LCT_RAIL_LDR_G);
    LG(CST_RAIL_LUD,GROUP_RAIL,LCT_RAIL_LUD_G);
    LG(CST_RAIL_UDR,GROUP_RAIL,LCT_RAIL_UDR_G);
    LG(CST_RAIL_LUDR,GROUP_RAIL,LCT_RAIL_LUDR_G);

    LG(CST_ROAD_LR,GROUP_ROAD,LCT_ROAD_LR_G);
    LG(CST_ROAD_LU,GROUP_ROAD,LCT_ROAD_LU_G);
    LG(CST_ROAD_LD,GROUP_ROAD,LCT_ROAD_LD_G);
    LG(CST_ROAD_UD,GROUP_ROAD,LCT_ROAD_UD_G);
    LG(CST_ROAD_UR,GROUP_ROAD,LCT_ROAD_UR_G);
    LG(CST_ROAD_DR,GROUP_ROAD,LCT_ROAD_DR_G);
    LG(CST_ROAD_LUR,GROUP_ROAD,LCT_ROAD_LUR_G);
    LG(CST_ROAD_LDR,GROUP_ROAD,LCT_ROAD_LDR_G);
    LG(CST_ROAD_LUD,GROUP_ROAD,LCT_ROAD_LUD_G);
    LG(CST_ROAD_UDR,GROUP_ROAD,LCT_ROAD_UDR_G);
    LG(CST_ROAD_LUDR,GROUP_ROAD,LCT_ROAD_LUDR_G);

    LG(CST_TRACK_LR,GROUP_TRACK,LCT_TRACK_LR_G);
    LG(CST_TRACK_LU,GROUP_TRACK,LCT_TRACK_LU_G);
    LG(CST_TRACK_LD,GROUP_TRACK,LCT_TRACK_LD_G);
    LG(CST_TRACK_UD,GROUP_TRACK,LCT_TRACK_UD_G);
    LG(CST_TRACK_UR,GROUP_TRACK,LCT_TRACK_UR_G);
    LG(CST_TRACK_DR,GROUP_TRACK,LCT_TRACK_DR_G);
    LG(CST_TRACK_LUR,GROUP_TRACK,LCT_TRACK_LUR_G);
    LG(CST_TRACK_LDR,GROUP_TRACK,LCT_TRACK_LDR_G);
    LG(CST_TRACK_LUD,GROUP_TRACK,LCT_TRACK_LUD_G);
    LG(CST_TRACK_UDR,GROUP_TRACK,LCT_TRACK_UDR_G);
    LG(CST_TRACK_LUDR,GROUP_TRACK,LCT_TRACK_LUDR_G);

    LG(CST_WATER,GROUP_WATER,LCT_WATER_G);
    LG(CST_WATER_D,GROUP_WATER,LCT_WATER_D_G);
    LG(CST_WATER_R,GROUP_WATER,LCT_WATER_R_G);
    LG(CST_WATER_U,GROUP_WATER,LCT_WATER_U_G);
    LG(CST_WATER_L,GROUP_WATER,LCT_WATER_L_G);
    LG(CST_WATER_LR,GROUP_WATER,LCT_WATER_LR_G);
    LG(CST_WATER_UD,GROUP_WATER,LCT_WATER_UD_G);
    LG(CST_WATER_LD,GROUP_WATER,LCT_WATER_LD_G);
    LG(CST_WATER_RD,GROUP_WATER,LCT_WATER_RD_G);
    LG(CST_WATER_LU,GROUP_WATER,LCT_WATER_LU_G);
    LG(CST_WATER_UR,GROUP_WATER,LCT_WATER_UR_G);
    LG(CST_WATER_LUD,GROUP_WATER,LCT_WATER_LUD_G);
    LG(CST_WATER_LRD,GROUP_WATER,LCT_WATER_LRD_G);
    LG(CST_WATER_LUR,GROUP_WATER,LCT_WATER_LUR_G);
    LG(CST_WATER_URD,GROUP_WATER,LCT_WATER_URD_G);
    LG(CST_WATER_LURD,GROUP_WATER,LCT_WATER_LURD_G);

    LG(CST_BLACKSMITH_0,GROUP_BLACKSMITH,LCT_BLACKSMITH_0_G);
    LG(CST_BLACKSMITH_1,GROUP_BLACKSMITH,LCT_BLACKSMITH_1_G);
    LG(CST_BLACKSMITH_2,GROUP_BLACKSMITH,LCT_BLACKSMITH_2_G);
    LG(CST_BLACKSMITH_3,GROUP_BLACKSMITH,LCT_BLACKSMITH_3_G);
    LG(CST_BLACKSMITH_4,GROUP_BLACKSMITH,LCT_BLACKSMITH_4_G);
    LG(CST_BLACKSMITH_5,GROUP_BLACKSMITH,LCT_BLACKSMITH_5_G);
    LG(CST_BLACKSMITH_6,GROUP_BLACKSMITH,LCT_BLACKSMITH_6_G);

    LG(CST_CRICKET_1,GROUP_CRICKET,LCT_CRICKET_1_G);
    LG(CST_CRICKET_2,GROUP_CRICKET,LCT_CRICKET_2_G);
    LG(CST_CRICKET_3,GROUP_CRICKET,LCT_CRICKET_3_G);
    LG(CST_CRICKET_4,GROUP_CRICKET,LCT_CRICKET_4_G);
    LG(CST_CRICKET_5,GROUP_CRICKET,LCT_CRICKET_5_G);
    LG(CST_CRICKET_6,GROUP_CRICKET,LCT_CRICKET_6_G);
    LG(CST_CRICKET_7,GROUP_CRICKET,LCT_CRICKET_7_G);

    LG(CST_FIRESTATION_1,GROUP_FIRESTATION,LCT_FIRESTATION_1_G);
    LG(CST_FIRESTATION_2,GROUP_FIRESTATION,LCT_FIRESTATION_2_G);
    LG(CST_FIRESTATION_3,GROUP_FIRESTATION,LCT_FIRESTATION_3_G);
    LG(CST_FIRESTATION_4,GROUP_FIRESTATION,LCT_FIRESTATION_4_G);
    LG(CST_FIRESTATION_5,GROUP_FIRESTATION,LCT_FIRESTATION_5_G);
    LG(CST_FIRESTATION_6,GROUP_FIRESTATION,LCT_FIRESTATION_6_G);
    LG(CST_FIRESTATION_7,GROUP_FIRESTATION,LCT_FIRESTATION_7_G);
    LG(CST_FIRESTATION_8,GROUP_FIRESTATION,LCT_FIRESTATION_8_G);
    LG(CST_FIRESTATION_9,GROUP_FIRESTATION,LCT_FIRESTATION_9_G);
    LG(CST_FIRESTATION_10,GROUP_FIRESTATION,LCT_FIRESTATION_10_G);

    LG(CST_HEALTH,GROUP_HEALTH,LCT_HEALTH_G);

    LG(CST_MARKET_EMPTY,GROUP_MARKET,LCT_MARKET_EMPTY_G);
    LG(CST_MARKET_LOW,GROUP_MARKET,LCT_MARKET_LOW_G);
    LG(CST_MARKET_MED,GROUP_MARKET,LCT_MARKET_MED_G);
    LG(CST_MARKET_FULL,GROUP_MARKET,LCT_MARKET_FULL_G);

    LG(CST_MILL_0,GROUP_MILL,LCT_MILL_0_G);
    LG(CST_MILL_1,GROUP_MILL,LCT_MILL_1_G);
    LG(CST_MILL_2,GROUP_MILL,LCT_MILL_2_G);
    LG(CST_MILL_3,GROUP_MILL,LCT_MILL_3_G);
    LG(CST_MILL_4,GROUP_MILL,LCT_MILL_4_G);
    LG(CST_MILL_5,GROUP_MILL,LCT_MILL_5_G);
    LG(CST_MILL_6,GROUP_MILL,LCT_MILL_6_G);

    LG(CST_MONUMENT_0,GROUP_MONUMENT,LCT_MONUMENT_0_G);
    LG(CST_MONUMENT_1,GROUP_MONUMENT,LCT_MONUMENT_1_G);
    LG(CST_MONUMENT_2,GROUP_MONUMENT,LCT_MONUMENT_2_G);
    LG(CST_MONUMENT_3,GROUP_MONUMENT,LCT_MONUMENT_3_G);
    LG(CST_MONUMENT_4,GROUP_MONUMENT,LCT_MONUMENT_4_G);
    LG(CST_MONUMENT_5,GROUP_MONUMENT,LCT_MONUMENT_5_G);

    LG(CST_POTTERY_0,GROUP_POTTERY,LCT_POTTERY_0_G);
    LG(CST_POTTERY_1,GROUP_POTTERY,LCT_POTTERY_1_G);
    LG(CST_POTTERY_2,GROUP_POTTERY,LCT_POTTERY_2_G);
    LG(CST_POTTERY_3,GROUP_POTTERY,LCT_POTTERY_3_G);
    LG(CST_POTTERY_4,GROUP_POTTERY,LCT_POTTERY_4_G);
    LG(CST_POTTERY_5,GROUP_POTTERY,LCT_POTTERY_5_G);
    LG(CST_POTTERY_6,GROUP_POTTERY,LCT_POTTERY_6_G);
    LG(CST_POTTERY_7,GROUP_POTTERY,LCT_POTTERY_7_G);
    LG(CST_POTTERY_8,GROUP_POTTERY,LCT_POTTERY_8_G);
    LG(CST_POTTERY_9,GROUP_POTTERY,LCT_POTTERY_9_G);
    LG(CST_POTTERY_10,GROUP_POTTERY,LCT_POTTERY_10_G);

    LG(CST_RECYCLE,GROUP_RECYCLE,LCT_RECYCLE_G);

    LG(CST_SCHOOL,GROUP_SCHOOL,LCT_SCHOOL_G);

    LG(CST_SHANTY,GROUP_SHANTY,LCT_SHANTY_G);

    LG(CST_SUBSTATION_R,GROUP_SUBSTATION,LCT_SUBSTATION_R_G);
    LG(CST_SUBSTATION_G,GROUP_SUBSTATION,LCT_SUBSTATION_G_G);
    LG(CST_SUBSTATION_RG,GROUP_SUBSTATION,LCT_SUBSTATION_RG_G);

    LG(CST_WINDMILL_1_G,GROUP_WINDMILL,LCT_WINDMILL_1_G_G);
    LG(CST_WINDMILL_2_G,GROUP_WINDMILL,LCT_WINDMILL_2_G_G);
    LG(CST_WINDMILL_3_G,GROUP_WINDMILL,LCT_WINDMILL_3_G_G);
    LG(CST_WINDMILL_1_RG,GROUP_WINDMILL,LCT_WINDMILL_1_RG_G);
    LG(CST_WINDMILL_2_RG,GROUP_WINDMILL,LCT_WINDMILL_2_RG_G);
    LG(CST_WINDMILL_3_RG,GROUP_WINDMILL,LCT_WINDMILL_3_RG_G);
    LG(CST_WINDMILL_1_R,GROUP_WINDMILL,LCT_WINDMILL_1_R_G);
    LG(CST_WINDMILL_2_R,GROUP_WINDMILL,LCT_WINDMILL_2_R_G);
    LG(CST_WINDMILL_3_R,GROUP_WINDMILL,LCT_WINDMILL_3_R_G);
    LG(CST_WINDMILL_1_W,GROUP_WINDMILL,LCT_WINDMILL_1_W_G);
    LG(CST_WINDMILL_2_W,GROUP_WINDMILL,LCT_WINDMILL_2_W_G);
    LG(CST_WINDMILL_3_W,GROUP_WINDMILL,LCT_WINDMILL_3_W_G);

    LG(CST_INDUSTRY_L_C,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_C_G);
    LG(CST_INDUSTRY_L_Q1,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_Q1_G);
    LG(CST_INDUSTRY_L_Q2,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_Q2_G);
    LG(CST_INDUSTRY_L_Q3,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_Q3_G);
    LG(CST_INDUSTRY_L_Q4,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_Q4_G);
    LG(CST_INDUSTRY_L_L1,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_L1_G);
    LG(CST_INDUSTRY_L_L2,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_L2_G);
    LG(CST_INDUSTRY_L_L3,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_L3_G);
    LG(CST_INDUSTRY_L_L4,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_L4_G);
    LG(CST_INDUSTRY_L_M1,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_M1_G);
    LG(CST_INDUSTRY_L_M2,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_M2_G);
    LG(CST_INDUSTRY_L_M3,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_M3_G);
    LG(CST_INDUSTRY_L_M4,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_M4_G);
    LG(CST_INDUSTRY_L_H1,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_H1_G);
    LG(CST_INDUSTRY_L_H2,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_H2_G);
    LG(CST_INDUSTRY_L_H3,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_H3_G);
    LG(CST_INDUSTRY_L_H4,GROUP_INDUSTRY_L,LCT_INDUSTRY_L_H4_G);

    LG(CST_RESIDENCE_LL,GROUP_RESIDENCE_LL,LCT_RESIDENCE_LL_G);
    LG(CST_RESIDENCE_ML,GROUP_RESIDENCE_ML,LCT_RESIDENCE_ML_G);
    LG(CST_RESIDENCE_HL,GROUP_RESIDENCE_HL,LCT_RESIDENCE_HL_G);
    LG(CST_RESIDENCE_LH,GROUP_RESIDENCE_LH,LCT_RESIDENCE_LH_G);
    LG(CST_RESIDENCE_MH,GROUP_RESIDENCE_MH,LCT_RESIDENCE_MH_G);
    LG(CST_RESIDENCE_HH,GROUP_RESIDENCE_HH,LCT_RESIDENCE_HH_G);

    LG(CST_UNIVERSITY,GROUP_UNIVERSITY,LCT_UNIVERSITY_G);

    LG(CST_COALMINE_EMPTY,GROUP_COALMINE,LCT_COALMINE_EMPTY_G);
    LG(CST_COALMINE_LOW,GROUP_COALMINE,LCT_COALMINE_LOW_G);
    LG(CST_COALMINE_MED,GROUP_COALMINE,LCT_COALMINE_MED_G);
    LG(CST_COALMINE_FULL,GROUP_COALMINE,LCT_COALMINE_FULL_G);

    LG(CST_COMMUNE_1,GROUP_COMMUNE,LCT_COMMUNE_1_G);
    LG(CST_COMMUNE_2,GROUP_COMMUNE,LCT_COMMUNE_2_G);
    LG(CST_COMMUNE_3,GROUP_COMMUNE,LCT_COMMUNE_3_G);
    LG(CST_COMMUNE_4,GROUP_COMMUNE,LCT_COMMUNE_4_G);
    LG(CST_COMMUNE_5,GROUP_COMMUNE,LCT_COMMUNE_5_G);
    LG(CST_COMMUNE_6,GROUP_COMMUNE,LCT_COMMUNE_6_G);
    LG(CST_COMMUNE_7,GROUP_COMMUNE,LCT_COMMUNE_7_G);
    LG(CST_COMMUNE_8,GROUP_COMMUNE,LCT_COMMUNE_8_G);
    LG(CST_COMMUNE_9,GROUP_COMMUNE,LCT_COMMUNE_9_G);
    LG(CST_COMMUNE_10,GROUP_COMMUNE,LCT_COMMUNE_10_G);
    LG(CST_COMMUNE_11,GROUP_COMMUNE,LCT_COMMUNE_11_G);
    LG(CST_COMMUNE_12,GROUP_COMMUNE,LCT_COMMUNE_12_G);
    LG(CST_COMMUNE_13,GROUP_COMMUNE,LCT_COMMUNE_13_G);
    LG(CST_COMMUNE_14,GROUP_COMMUNE,LCT_COMMUNE_14_G);

    LG(CST_EX_PORT,GROUP_PORT,LCT_EX_PORT_G);

    LG(CST_FARM_O0,GROUP_ORGANIC_FARM,LCT_FARM_O0_G);
    LG(CST_FARM_O1,GROUP_ORGANIC_FARM,LCT_FARM_O1_G);
    LG(CST_FARM_O2,GROUP_ORGANIC_FARM,LCT_FARM_O2_G);
    LG(CST_FARM_O3,GROUP_ORGANIC_FARM,LCT_FARM_O3_G);
    LG(CST_FARM_O4,GROUP_ORGANIC_FARM,LCT_FARM_O4_G);
    LG(CST_FARM_O5,GROUP_ORGANIC_FARM,LCT_FARM_O5_G);
    LG(CST_FARM_O6,GROUP_ORGANIC_FARM,LCT_FARM_O6_G);
    LG(CST_FARM_O7,GROUP_ORGANIC_FARM,LCT_FARM_O7_G);
    LG(CST_FARM_O8,GROUP_ORGANIC_FARM,LCT_FARM_O8_G);
    LG(CST_FARM_O9,GROUP_ORGANIC_FARM,LCT_FARM_O9_G);
    LG(CST_FARM_O10,GROUP_ORGANIC_FARM,LCT_FARM_O10_G);
    LG(CST_FARM_O11,GROUP_ORGANIC_FARM,LCT_FARM_O11_G);
    LG(CST_FARM_O12,GROUP_ORGANIC_FARM,LCT_FARM_O12_G);
    LG(CST_FARM_O13,GROUP_ORGANIC_FARM,LCT_FARM_O13_G);
    LG(CST_FARM_O14,GROUP_ORGANIC_FARM,LCT_FARM_O14_G);
    LG(CST_FARM_O15,GROUP_ORGANIC_FARM,LCT_FARM_O15_G);
    LG(CST_FARM_O16,GROUP_ORGANIC_FARM,LCT_FARM_O16_G);

    LG(CST_INDUSTRY_H_C,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_C_G);
    LG(CST_INDUSTRY_H_L1,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_L1_G);
    LG(CST_INDUSTRY_H_L2,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_L2_G);
    LG(CST_INDUSTRY_H_L3,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_L3_G);
    LG(CST_INDUSTRY_H_L4,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_L4_G);
    LG(CST_INDUSTRY_H_L5,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_L5_G);
    LG(CST_INDUSTRY_H_L6,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_L6_G);
    LG(CST_INDUSTRY_H_L7,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_L7_G);
    LG(CST_INDUSTRY_H_L8,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_L8_G);
    LG(CST_INDUSTRY_H_M1,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_M1_G);
    LG(CST_INDUSTRY_H_M2,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_M2_G);
    LG(CST_INDUSTRY_H_M3,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_M3_G);
    LG(CST_INDUSTRY_H_M4,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_M4_G);
    LG(CST_INDUSTRY_H_M5,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_M5_G);
    LG(CST_INDUSTRY_H_M6,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_M6_G);
    LG(CST_INDUSTRY_H_M7,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_M7_G);
    LG(CST_INDUSTRY_H_M8,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_M8_G);
    LG(CST_INDUSTRY_H_H1,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_H1_G);
    LG(CST_INDUSTRY_H_H2,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_H2_G);
    LG(CST_INDUSTRY_H_H3,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_H3_G);
    LG(CST_INDUSTRY_H_H4,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_H4_G);
    LG(CST_INDUSTRY_H_H5,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_H5_G);
    LG(CST_INDUSTRY_H_H6,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_H6_G);
    LG(CST_INDUSTRY_H_H7,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_H7_G);
    LG(CST_INDUSTRY_H_H8,GROUP_INDUSTRY_H,LCT_INDUSTRY_H_H8_G);

    LG(CST_OREMINE_1,GROUP_OREMINE,LCT_OREMINE_1_G);
    LG(CST_OREMINE_2,GROUP_OREMINE,LCT_OREMINE_2_G);
    LG(CST_OREMINE_3,GROUP_OREMINE,LCT_OREMINE_3_G);
    LG(CST_OREMINE_4,GROUP_OREMINE,LCT_OREMINE_4_G);
    LG(CST_OREMINE_5,GROUP_OREMINE,LCT_OREMINE_5_G);
    LG(CST_OREMINE_6,GROUP_OREMINE,LCT_OREMINE_6_G);
    LG(CST_OREMINE_7,GROUP_OREMINE,LCT_OREMINE_7_G);
    LG(CST_OREMINE_8,GROUP_OREMINE,LCT_OREMINE_8_G);

    LG(CST_POWERS_COAL_EMPTY,GROUP_COAL_POWER,LCT_POWERS_COAL_EMPTY_G);
    LG(CST_POWERS_COAL_LOW,GROUP_COAL_POWER,LCT_POWERS_COAL_LOW_G);
    LG(CST_POWERS_COAL_MED,GROUP_COAL_POWER,LCT_POWERS_COAL_MED_G);
    LG(CST_POWERS_COAL_FULL,GROUP_COAL_POWER,LCT_POWERS_COAL_FULL_G);

    LG(CST_POWERS_SOLAR,GROUP_SOLAR_POWER,LCT_POWERS_SOLAR_G);

    LG(CST_ROCKET_1,GROUP_ROCKET,LCT_ROCKET_1_G);
    LG(CST_ROCKET_2,GROUP_ROCKET,LCT_ROCKET_2_G);
    LG(CST_ROCKET_3,GROUP_ROCKET,LCT_ROCKET_3_G);
    LG(CST_ROCKET_4,GROUP_ROCKET,LCT_ROCKET_4_G);
    LG(CST_ROCKET_5,GROUP_ROCKET,LCT_ROCKET_5_G);
    LG(CST_ROCKET_6,GROUP_ROCKET,LCT_ROCKET_6_G);
    LG(CST_ROCKET_7,GROUP_ROCKET,LCT_ROCKET_7_G);
    LG(CST_ROCKET_FLOWN,GROUP_ROCKET,LCT_ROCKET_FLOWN_G);

    LG(CST_TIP_0,GROUP_TIP,LCT_TIP_0_G);
    LG(CST_TIP_1,GROUP_TIP,LCT_TIP_1_G);
    LG(CST_TIP_2,GROUP_TIP,LCT_TIP_2_G);
    LG(CST_TIP_3,GROUP_TIP,LCT_TIP_3_G);
    LG(CST_TIP_4,GROUP_TIP,LCT_TIP_4_G);
    LG(CST_TIP_5,GROUP_TIP,LCT_TIP_5_G);
    LG(CST_TIP_6,GROUP_TIP,LCT_TIP_6_G);
    LG(CST_TIP_7,GROUP_TIP,LCT_TIP_7_G);
    LG(CST_TIP_8,GROUP_TIP,LCT_TIP_8_G);

    // main_t
#undef LG

#if defined (commentout)
    /* PROCESS IMAGE HERE */
    while (!feof(txt_fp)) {
	char buf[128];
	char *fnp,*rip,*cip;
	int ri,ci;

	/* Get line from text file */
	fgets (buf,128,txt_fp);

	/* Tokenize */
	fnp = strtok(buf," \t");
	if (!fnp || *fnp == '#') continue;
	if (*fnp == '@') break;
	rip = strtok(NULL," \t");
	if (!rip) continue;
	cip = strtok(NULL," \t");
	if (!cip) continue;
	ri = atoi(rip);
	ci = atoi(cip);

	/* Copy icon */
	if (!strcmp(fnp,LCT_GREEN_G)) {
	    int r,c;
	    char* p;
            main_types[CST_GREEN].graphic=malloc(16*16);
	    p = main_types[CST_GREEN].graphic;
	    for (r=ri*16;r<ri*16+16;r++) {
	    for (c=ci*16;c<ci*16+16;c++) {
		*p++ = row_pointers[r][c];
	    } }
	}
    }
#endif

    /* Free the memory */
    for (row = 0; row < height; row++) {
	png_free(png_ptr, row_pointers[row]);
    }
    free(row_pointers);

    /* clean up after the read, and free any memory allocated - REQUIRED */
#if defined (commentout)
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
#endif
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    /* that's it */
    return (OK);
}

static char* 
load_png_graphic (short type, short group, char* id,
		  FILE* txt_fp,png_bytep *row_pointers, 
		  png_uint_32 width, png_uint_32 height)
{
    char buf[128];
    char *fnp,*rip,*cip;
    int ri,ci;
    char *grphc = 0;

    while (!feof(txt_fp)) {
	/* Get line from text file */
	fgets (buf,128,txt_fp);

	/* Tokenize */
	fnp = strtok(buf," \t");
	if (!fnp || *fnp == '#') continue;
	if (*fnp == '@') break;
	rip = strtok(NULL," \t");
	if (!rip) continue;
	cip = strtok(NULL," \t");
	if (!cip) continue;
	ri = atoi(rip);
	ci = atoi(cip);

	/* Copy icon */
	if (!strcmp(fnp,id)) {
	    int r,c;
	    int nr,nc;
	    char* p;
	    nr = nc = main_groups[group].size;
	    p = grphc = (char*)malloc(nr*16*nc*16);
	    if (!grphc) malloc_failure();
	    for (r=ri*16;r<(ri+nr)*16;r++) {
		for (c=ci*16;c<(ci+nc)*16;c++) {
		    *p++ = row_pointers[r][c];
		}
	    }
	    break;
	} else {
	    fprintf (stderr,"Error, wrong id string");
	    exit(-1);
	}
    }

    /* Set the main_type info */
    if (grphc) {
        main_types[type].group = group;
	main_types[type].graphic = grphc;
    } else {
	fprintf (stderr,"Error, couldn't find id string");
	exit(-1);
    }
    return 0;
}
