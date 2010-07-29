
#include <stdio.h>
#include <zlib.h>

#include "lctypes.h"

extern map_struct map;
extern int global_mountainity,  global_aridity;

void save_city_2(char *cname)
{
    int x, y, p, ldsv_version;
    int dumbint = 0;

    int monthgraph_size = 190;
    
    gzFile ofile = gzopen(cname, "wb");
    if (ofile == NULL) {
        fprintf(stderr,"error Saving file <%s> -\n ", cname);
    }
    /* Now we have upgraded game */
    ldsv_version = WATERWELL_V2;
    gzprintf(ofile, "%d\n", ldsv_version);

    for (x = 0; x < WORLD_SIDE_LEN; x++)
        for (y = 0; y < WORLD_SIDE_LEN; y++) {
            /*               TY po fl cr or i1 i2 i3 i4 i5 i6 i7 PL al ec ws gp wa wp ww wn g1 g2 g3 g4 DA TK AN d4 d5 d6 d7 d8 d9 */
            gzprintf(ofile, "%u %d %d %u %u %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n" 
                       , MP_TYPE(x, y)
                       , 0	//housed population
                       , MP_INFO(x, y)
                       , 0
                       , 0
                       , 0
                       , 0
                       , 0
                       , 0
                       , 0
                       , 0
                       , 0
                       , 0
                       , (long int) map.altitude[x][y]
                       , map.ecotable[x][y]
                       , 0
                       , 0
                       , (long int) map.altitude[x][y]
                       , 0
                       , 0
                       , 0 	// ? TODO map[x][y].water_next ?
		       		// do the water of this tile go to next (one) or to (several) lower neighbours ?
                       , 0 // int1 TODO distance to river (for vegetation)
                       , 0
                       , 0
                       , 0
                       , 0
                       , 0
                       , 0
                       , dumbint        // d4  could be         image index for smooth animation, cf windmill anim_tile
                       , dumbint        // d5                   percentage of activity to choose family of pic
                       , dumbint        // d6
                       , dumbint        // d7
                       , dumbint        // d8
                       , dumbint        // d9
                       );
        }

    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);

    gzprintf(ofile, "%d\n", 0);
    for (x = 0; x < MAX_NUMOF_SUBSTATIONS; x++) {
        gzprintf(ofile, "%d\n", 0);
        gzprintf(ofile, "%d\n", 0);
    }
    gzprintf(ofile, "%d\n", 0);
    for (x = 0; x < MAX_NUMOF_MARKETS; x++) {
        gzprintf(ofile, "%d\n", 0);
        gzprintf(ofile, "%d\n", 0);
    }
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 200); // people_pool : needed to start a game
    gzprintf(ofile, "%o\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0); /* waste_goods is obsolete */
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0); /* Removed diff_old_population, version 1.12 */

    /* Changed, version 1.12 */
    gzprintf(ofile, "%d\n", monthgraph_size);
    for (x = 0; x < monthgraph_size; x++) {
        gzprintf(ofile, "%d\n", 0);
        gzprintf(ofile, "%d\n", 0);
        gzprintf(ofile, "%d\n", 0);
        gzprintf(ofile, "%d\n", 0);
    }
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);

    for (x = 0; x < PBAR_DATA_SIZE; x++)
        for (p = 0; p < NUM_PBARS; p++)
            gzprintf(ofile, "%d\n", 0);

    for (p = 0; p < NUM_PBARS; p++) {
        gzprintf(ofile, "%d\n", 0);
        gzprintf(ofile, "%d\n", 0);
    }

    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%f\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%f\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%f\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);
    gzprintf(ofile, "%d\n", 0);

    for (x = 0; x < NUMOF_MODULES; x++)
        gzprintf(ofile, "%d\n", 0);

    gzprintf(ofile, "random land generator\n"); /* 1 */

    gzprintf(ofile, "%d\n", 0);        /* 2 */

    gzprintf(ofile, "sust %d %d %d %d %d %d %d %d %d %d\n", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); /* 3 */

    gzprintf(ofile, "arid %d %d\n", global_aridity, global_mountainity); /* 4 */

    gzprintf(ofile, "dummy\n"); /* 5 */

    gzprintf(ofile, "dummy\n"); /* 6 */

    gzprintf(ofile, "dummy\n"); /* 7 */

    gzprintf(ofile, "dummy\n"); /* 8 */

    gzprintf(ofile, "dummy\n"); /* 9 */

    gzprintf(ofile, "dummy\n"); /* 10 */

    gzclose(ofile);
}


