#include <config.h>

#include <SDL.h>

long real_time=0;
long start_time=0;

void reset_start_time()
{
  start_time=SDL_GetTicks();
}

void get_real_time()
{
  real_time=SDL_GetTicks()-start_time;
}
