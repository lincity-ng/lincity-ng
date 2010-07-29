/** Tiny Land Generator
 *
 * @file 	ground_sea.cpp
 * @author 	alain.baeckeroot@laposte.net
 * @copyright 	2010 Alain BAECKEROOT
 * @license 	GPL v3 or later
 */

#include "lctypes.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

extern map_struct map;

int alt_min;
int alt_max;
int alt_step;

int global_mountainity = 4000;	//  nearly useless to have a random one (only impacts do_rand_ecology through ALT(x,y))

const int mask_size = 7;	// useless to be larger than 3*sigma && Must be < SHIFT
float norm, gauss_mask[2 * mask_size + 1][2 * mask_size + 1];

void build_gaussian_mask()
{
	const float sigma = 2.5;
	const float ods2 = 1. / (2. * sigma * sigma);

	norm = 0;
	for (int i = 0; i < 2 * mask_size + 1; i++) {
		for (int j = 0; j < 2 * mask_size + 1; j++) {
			float r2 = (i - mask_size) * (i - mask_size) + (j - mask_size) * (j - mask_size);
			gauss_mask[i][j] = exp(-r2 * ods2);
			norm += gauss_mask[i][j];
		}
	}
	norm = 1. / norm;
}

void setup_ground(void)
{
	/* Principle of land generation :
	 *
	 * we start with large blocks of land SZ x SZ, and take random height for each
	 *     at each iteration we divide the block size by 2, and Altitude += rand() * mountainity
	 *
	 * then we smooth the land, in order to have several local maxima and minima, but not hundreds
	 *     (else we go crazy with river path)
	 */

	const int NLOOP = 5; 	// we stop before reaching 1x1 blocks (which would be for NLOOP=7)
				// because we smooth with a gaussian mask, so details would be erased

	const int SZ = 128;	// must be >= 2^NLOOP
	const int SHIFT = (SZ - WORLD_SIDE_LEN) / 2;	// center the visible map in the big one

	/* good values:
	 *      sigma = 3.5   // sigma =2.5 => lots of local minima = small lakes  ;
	 */

	float tmp[SZ][SZ];
	float t2[SZ][SZ];
	float h;
	int i, j, k, l, m, n, size;

	for (i = 0; i < SZ; i++) {
		for (j = 0; j < SZ; j++) {
			t2[i][j] = 0;
			tmp[i][j] = 0;
		}
	}

	/* fractal iteration for height */
	n = 1;
	for (k = 1; k < NLOOP; k++) {
		n *= 2;
		size = SZ / n;
		// n x n block of size
		for (l = 0; l < n; l++) {
			for (m = 0; m < n; m++) {
				// one block
				h = rand() % global_mountainity;
				for (i = 0; i < size; i++)
					for (j = 0; j < size; j++)
						t2[l * size + i][m * size + j] += h;
			}
		}
	}

	// put east side at altitude 0
	/*
	   for (i = WORLD_SIDE_LEN; i < SZ; i++)
	   for (j = 0; j < SZ; j++)
	   t2[i][j]=0.;
	 */

	//smooth with gaussian mask
	for (i = mask_size; i < SZ - mask_size; i++)
		for (j = mask_size; j < SZ - mask_size; j++) {
			tmp[i][j] = 0;
			for (k = -mask_size; k <= mask_size; k++)
				for (l = -mask_size; l <= mask_size; l++)
					tmp[i][j] += t2[i + k][j - l] \
						     * gauss_mask[mask_size + k][mask_size + l];
		}

	float a_max = 0;
	float a_min = 1.e9;
	// pick our map in the fractal one  and normalize it 
	for (i = 0; i < WORLD_SIDE_LEN; i++)
		for (j = 0; j < WORLD_SIDE_LEN; j++) {
			ALT(i, j) = (int)(tmp[SHIFT + i][SHIFT + j] * norm);
			if (ALT(i, j) > a_max)
				a_max = ALT(i, j);
			if (ALT(i, j) > 0 && ALT(i, j) < a_min)
				a_min = ALT(i, j);
		}
	// take visible value for maximum color dynamic
	a_min -= 1.;
	a_max += 1.;
	alt_min = (int)a_min;
	alt_max = (int)a_max;
	alt_step = (alt_max - alt_min) / 10;

	for (i = 0; i < WORLD_SIDE_LEN; i++)
		for (j = 0; j < WORLD_SIDE_LEN; j++)
			MP_COLOR(i, j) = (unsigned long)((ALT(i, j) - a_min) / (a_max - a_min) * 16777216.);

	//fprintf(stderr, " alt min = %i; max = %i\n", alt_min, alt_max);

}
