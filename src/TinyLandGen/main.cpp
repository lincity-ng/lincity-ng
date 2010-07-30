/** Tiny Land Generator
 *
 * @file	main.cpp
 * @author 	alain.baeckeroot@laposte.net
 * @copyright 	2010 Alain BAECKEROOT
 * @license 	GPL v3 or later
 */

#if defined(_MSC_VER)
#include "SDL.h"
#else
#include "SDL/SDL.h"
#endif

#include <time.h>
#include <math.h>

#include "lctypes.h"

SDL_Surface *screen;

extern void build_gaussian_mask();
extern void setup_ground(void);
extern void setup_lake_river(void);
extern void setup_land(void);

extern void save_city_2(char *cname);
extern int alt_min, alt_max, alt_step;

lmap_struct lmap;

int key = 0;
unsigned long i = 0;
int redraw = true;

void erase_map()
{

	for (int i = 0; i < WORLD_SIDE_LEN; i++)
		for (int j = 0; j < WORLD_SIDE_LEN; j++) {
			MP_FLAG(i, j) = 0;
			MP_TYPE(i, j) = 0;
		}
}

void render()
{
	SDL_Rect rect;

	// Lock surface if needed
	if (SDL_MUSTLOCK(screen))
		if (SDL_LockSurface(screen) < 0)
			return;

	// Ask SDL for the time in milliseconds
	int tick = SDL_GetTicks();

	// Draw to screen
	//SDL_FillRect(screen, NULL, 65536 * i++ );
	for (int i = 0; i < WORLD_SIDE_LEN; i++) {
		for (int j = 0; j < WORLD_SIDE_LEN; j++) {
			rect.x = (int)(i * SIZE);
			rect.y = j * SIZE + 1;
			rect.w = SIZE;
			rect.h = SIZE;
			SDL_FillRect(screen, &rect, MP_COLOR(i, j));
			//SDL_FillRect(screen, &rect, lmap.dist2w[i][j] * 260000);
		}
	}

	// Unlock if needed
	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	// Tell SDL to update the whole screen
	SDL_UpdateRect(screen, 0, 0, SCREEN_SIZE, SCREEN_SIZE);
}

int main(void)
{
    char *fname;
    fname = (char *) "random_land.scn";

#ifdef DEBUG
	// Fix random seed for easier debug
	srand(1234);
#else
	srand(time(0));
#endif

	// Initialize SDL's subsystems - in this case, only video.
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	// Register SDL_Quit to be called at exit; makes sure things are
	// cleaned up when we quit.
	atexit(SDL_Quit);

	// Attempt to create a 600x600 window with 32 bit per pixels.
	screen = SDL_SetVideoMode(SCREEN_SIZE, SCREEN_SIZE, 24, SDL_SWSURFACE);

	// If we fail, return error.
	if (screen == NULL) {
		fprintf(stderr, "Unable to set %ix%i video: %s\n", SCREEN_SIZE, SCREEN_SIZE, SDL_GetError());
		exit(1);
	}
	// gaussian mask of fixed size used for ground smoothing
	build_gaussian_mask();

	// Main loop: loop forever.
	while (1) {
		// Poll for events, and handle the ones we care about.
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				key = event.key.keysym.sym;
				switch (key) {
				case SDLK_r:
					// 1.0 : generate a new Random field
					redraw = true;
					break;

			    // 1.1 : put sea on this side: North, East, South, West
				case SDLK_n:
					break;
				case SDLK_e:
					break;
				case SDLK_s:
					break;
				case SDLK_w:
					break;

				case SDLK_UP:
					// 1.2 : adjust max altitude
					break;
				case SDLK_DOWN:
					break;

				case SDLK_l:
					// 2.0 : lakes and rivers
					break;

				case SDLK_RIGHT:
					// 3.0 : climate (dryness)
					break;
				case SDLK_LEFT:
					break;

				case SDLK_t:
					// 3.1 : trees
					break;

				case SDLK_i:
					// 4 : isometric view (toggle iso/top)
					break;
				case SDLK_f:
					// 5 : file (save)
                    			save_city_2(fname);
					redraw = false;
					break;
				}
				if (redraw) {
					erase_map();
					setup_ground();
					setup_lake_river();
					setup_land();
				}
				break;

			case SDL_QUIT:
				return (0);
			}
		}
		// Render stuff
		render();
		SDL_Delay(10);
	}
	return 0;
}
