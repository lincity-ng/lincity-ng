#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define COLOUR_PAL_FILE "colour.pal"

typedef struct rgb_struct {
    int red;
    int grn;
    int blu;
} RGB;


int
main (int argc, char *argv[])
{
    char s[100];
    unsigned char c;
    int n;
    int r, g, b;
    int size;
    RGB pal[256];
    char *infile, *outfile;
    FILE *fp, *fpi;
    struct stat statbuf;

    if (argc != 3) {
	fprintf (stderr, "Usage: csi2ppm infile outfile\n");
	exit (-1);
    }
    infile = argv[1];
    outfile = argv[2];

    /* Load palette file */
    if ((fp = fopen (COLOUR_PAL_FILE, "r")) == 0) {
	fprintf (stderr, "Can't find the colour pallet file");
	exit (-1);
    }
    n = 0;
    while (feof (fp) == 0 && n < 256) {
	fgets (s, 99, fp);
	if (sscanf (s, "%d %d %d %d", &n, &r, &g, &b) == 4) {
	    pal[n].red = r;
	    pal[n].grn = g;
	    pal[n].blu = b;
	    n++;
	}
    }
    if (n != 256) {
	fprintf (stderr, "Not enough palette entries\n");
    }
    fclose (fp);

    /* Get size of icon */
    if (stat (infile, &statbuf) != 0) {
	fprintf (stderr, "Error getting size of input file: %s\n", infile);
	exit (-1);
    }
    size = statbuf.st_size;
    switch (size) {
    case 256:
	size = 1*16;
	break;
    case 1024:
	size = 2*16;
	break;
    case 2304:
	size = 3*16;
	break;
    case 4096:
	size = 4*16;
	break;
    default:
	fprintf (stderr, "Size of input file is wrong: %s\n", infile);
	exit (-1);
    }

    /* Read icon and write ppm */
    fpi = fopen (infile, "r");
    if (!fpi) {
	fprintf (stderr, "Error opening input file: %s\n", infile);
	exit (-1);
    }
    fp = fopen (outfile, "w");
    if (!fp) {
	fprintf (stderr, "Error opening output file: %s\n", outfile);
	exit (-1);
    }
    fprintf (fp,"P3\n# Created by csi2ppm from lincity file %s\n", infile);
//    fprintf (fp,"%d %d\n255\n",size,size);
    fprintf (fp,"%d %d\n62\n",size,size);
    while (fread(&c,1,1,fpi)) {
	fprintf (fp, "%d %d %d\n", pal[c].red, pal[c].grn, pal[c].blu);
    }
    fclose (fpi);
    fclose (fp);
}
