/* ---------------------------------------------------------------------- *
 * animate.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include <stdlib.h>
#include "lin-city.h"
#include "cliglobs.h"
#include "engglobs.h"
#include "lctypes.h"
#include "clinet.h"
#include "animate.h"

/* ---------------------------------------------------------------------- *
 * Public Global Variables
 * ---------------------------------------------------------------------- */
Animation_Info anim_info[WORLD_SIDE_LEN][WORLD_SIDE_LEN];

/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */
void 
init_animation (void)
{
  int x, y;
  for (x = 0; x < WORLD_SIDE_LEN; x++) {
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
      anim_info[x][y].state = rand() % 32;
      anim_info[x][y].timeout = 0;
    }
  }
}

void 
animate_organic_farm (int x, int y)
{
  if (real_time < ANIM_TIMEOUT(x,y)) {
    return;
  }
  ANIM_TIMEOUT(x,y) = real_time + ORGANIC_FARM_ANIM_SPEED 
    + rand() % 20 * (ORGANIC_FARM_ANIM_SPEED / 10);

  if (MP_INFO(x,y).int_4 < MIN_FOOD_SOLD_FOR_ANIM) {
    MP_TYPE(x,y) = CST_FARM_O0;
    return;
  }

  switch (MP_TYPE(x,y))
    {
    case CST_FARM_O0:
      MP_TYPE(x,y) = CST_FARM_O3;
      break;
    case CST_FARM_O3:
      MP_TYPE(x,y) = CST_FARM_O7;
      break;
    case CST_FARM_O7:
      MP_TYPE(x,y) = CST_FARM_O11;
      break;
    case CST_FARM_O11:
      MP_TYPE(x,y) = CST_FARM_O15;
      break;
    case CST_FARM_O15:
      MP_TYPE(x,y) = CST_FARM_O0;
      break;
    }
}

void 
animate_market (int x, int y)
{
  if (real_time < ANIM_TIMEOUT(x,y)) {
    return;
  }
  ANIM_TIMEOUT(x,y) = real_time + MARKET_ANIM_SPEED;

  if (MP_INFO(x,y).int_1 <= 0) {
    if (MP_INFO(x,y).int_2 > 0) {
      MP_TYPE(x,y) = CST_MARKET_LOW;
    } else {
      MP_TYPE(x,y) = CST_MARKET_EMPTY;
    }
  } else if (MP_INFO(x,y).int_1 < (MARKET_FOOD_SEARCH_TRIGGER / 2)) {
    MP_TYPE(x,y) = CST_MARKET_LOW;
  } else if (MP_INFO(x,y).int_1 
	     < (MAX_FOOD_IN_MARKET - MAX_FOOD_IN_MARKET / 4)) {
    MP_TYPE(x,y) = CST_MARKET_MED;
  } else {
    MP_TYPE(x,y) = CST_MARKET_FULL;
  }
}

void 
animate_coalmine (int x, int y)
{

  if (MP_INFO(x,y).int_1 > (MAX_COAL_AT_MINE - (MAX_COAL_AT_MINE / 5)))
    MP_TYPE(x,y) = CST_COALMINE_FULL;
  else if (MP_INFO(x,y).int_1 > (MAX_COAL_AT_MINE / 2))
    MP_TYPE(x,y) = CST_COALMINE_MED;
  else if (MP_INFO(x,y).int_1 > 0)
    MP_TYPE(x,y) = CST_COALMINE_LOW;
  else
    MP_TYPE(x,y) = CST_COALMINE_EMPTY;
}

void 
animate_power_source_coal (int x, int y)
{
  if (MP_INFO(x,y).int_2 > (MAX_COAL_AT_POWER_STATION
			    - (MAX_COAL_AT_POWER_STATION / 5)))
    MP_TYPE(x,y) = CST_POWERS_COAL_FULL;
  else if (MP_INFO(x,y).int_2 > (MAX_COAL_AT_POWER_STATION / 2))
    MP_TYPE(x,y) = CST_POWERS_COAL_MED;
  else if (MP_INFO(x,y).int_2 > (MAX_COAL_AT_POWER_STATION / 10))
    MP_TYPE(x,y) = CST_POWERS_COAL_LOW;
  else
    MP_TYPE(x,y) = CST_POWERS_COAL_EMPTY;
}

void 
animate_industry_l (int x, int y)
{
    if (real_time < ANIM_TIMEOUT(x,y)) {
	return;
    }
    ANIM_TIMEOUT(x,y) = real_time + INDUSTRY_L_ANIM_SPEED;

    /* Choose animation series based on previous month's data. */
    if (MP_INFO(x,y).int_6 > 80) {
	switch (MP_TYPE(x,y))
	{
	case (CST_INDUSTRY_L_H1):
	case (CST_INDUSTRY_L_H2):
	case (CST_INDUSTRY_L_H3):
	case (CST_INDUSTRY_L_H4):
	    break;
	default:
	    MP_TYPE(x,y) = CST_INDUSTRY_L_H1;
	}
    }
    else if (MP_INFO(x,y).int_6 > 55)
    {
	switch (MP_TYPE(x,y))
	{
	case (CST_INDUSTRY_L_M1):
	case (CST_INDUSTRY_L_M2):
	case (CST_INDUSTRY_L_M3):
	case (CST_INDUSTRY_L_M4):
	    break;
	default:
	    MP_TYPE(x,y) = CST_INDUSTRY_L_M1;
	}
    }
    else if (MP_INFO(x,y).int_6 > 25)
    {
	switch (MP_TYPE(x,y))
	{
	case (CST_INDUSTRY_L_L1):
	case (CST_INDUSTRY_L_L2):
	case (CST_INDUSTRY_L_L3):
	case (CST_INDUSTRY_L_L4):
	    break;
	default:
	    MP_TYPE(x,y) = CST_INDUSTRY_L_L1;
	}
    }
    else if (MP_INFO(x,y).int_6 > 0)
    {
	switch (MP_TYPE(x,y))
	{
	case (CST_INDUSTRY_L_Q1):
	case (CST_INDUSTRY_L_Q2):
	case (CST_INDUSTRY_L_Q3):
	case (CST_INDUSTRY_L_Q4):
	    break;
	default:
	    MP_TYPE(x,y) = CST_INDUSTRY_L_Q1;
	}
    }
    else
    {
	MP_TYPE(x,y) = CST_INDUSTRY_L_C;
    }

    switch (MP_TYPE(x,y))
    {
    case (CST_INDUSTRY_L_Q1):
	MP_TYPE(x,y) = CST_INDUSTRY_L_Q2;
	break;
    case (CST_INDUSTRY_L_Q2):
	MP_TYPE(x,y) = CST_INDUSTRY_L_Q3;
	break;
    case (CST_INDUSTRY_L_Q3):
	MP_TYPE(x,y) = CST_INDUSTRY_L_Q4;
	break;
    case (CST_INDUSTRY_L_Q4):
	MP_TYPE(x,y) = CST_INDUSTRY_L_Q1;
	break;
    case (CST_INDUSTRY_L_L1):
	MP_TYPE(x,y) = CST_INDUSTRY_L_L2;
	break;
    case (CST_INDUSTRY_L_L2):
	MP_TYPE(x,y) = CST_INDUSTRY_L_L3;
	break;
    case (CST_INDUSTRY_L_L3):
	MP_TYPE(x,y) = CST_INDUSTRY_L_L4;
	break;
    case (CST_INDUSTRY_L_L4):
	MP_TYPE(x,y) = CST_INDUSTRY_L_L1;
	break;
    case (CST_INDUSTRY_L_M1):
	MP_TYPE(x,y) = CST_INDUSTRY_L_M2;
	break;
    case (CST_INDUSTRY_L_M2):
	MP_TYPE(x,y) = CST_INDUSTRY_L_M3;
	break;
    case (CST_INDUSTRY_L_M3):
	MP_TYPE(x,y) = CST_INDUSTRY_L_M4;
	break;
    case (CST_INDUSTRY_L_M4):
	MP_TYPE(x,y) = CST_INDUSTRY_L_M1;
	break;
    case (CST_INDUSTRY_L_H1):
	MP_TYPE(x,y) = CST_INDUSTRY_L_H2;
	break;
    case (CST_INDUSTRY_L_H2):
	MP_TYPE(x,y) = CST_INDUSTRY_L_H3;
	break;
    case (CST_INDUSTRY_L_H3):
	MP_TYPE(x,y) = CST_INDUSTRY_L_H4;
	break;
    case (CST_INDUSTRY_L_H4):
	MP_TYPE(x,y) = CST_INDUSTRY_L_H1;
	break;
    }
}

void 
animate_industry_h (int x, int y)
{
    if (real_time < ANIM_TIMEOUT(x,y)) {
	return;
    }
    ANIM_TIMEOUT(x,y) = real_time + INDUSTRY_H_ANIM_SPEED;

    /* Choose animation series based on previous month's data. */
    if (MP_INFO(x,y).int_5 > 80) {
	switch (MP_TYPE(x,y)) {
	case (CST_INDUSTRY_H_H1):
	case (CST_INDUSTRY_H_H2):
	case (CST_INDUSTRY_H_H3):
	case (CST_INDUSTRY_H_H4):
	case (CST_INDUSTRY_H_H5):
	case (CST_INDUSTRY_H_H6):
	case (CST_INDUSTRY_H_H7):
	case (CST_INDUSTRY_H_H8):
	    break;
	default:
	    MP_TYPE(x,y) = CST_INDUSTRY_H_H1;
	}
    }
    else if (MP_INFO(x,y).int_5 > 30) {
	switch (MP_TYPE(x,y)) {
	case (CST_INDUSTRY_H_M1):
	case (CST_INDUSTRY_H_M2):
	case (CST_INDUSTRY_H_M3):
	case (CST_INDUSTRY_H_M4):
	case (CST_INDUSTRY_H_M5):
	case (CST_INDUSTRY_H_M6):
	case (CST_INDUSTRY_H_M7):
	case (CST_INDUSTRY_H_M8):
	    break;
	default:
	    MP_TYPE(x,y) = CST_INDUSTRY_H_M1;
	}
    }
    else if (MP_INFO(x,y).int_5 > 0) {
	switch (MP_TYPE(x,y)) {
	case (CST_INDUSTRY_H_L1):
	case (CST_INDUSTRY_H_L2):
	case (CST_INDUSTRY_H_L3):
	case (CST_INDUSTRY_H_L4):
	case (CST_INDUSTRY_H_L5):
	case (CST_INDUSTRY_H_L6):
	case (CST_INDUSTRY_H_L7):
	case (CST_INDUSTRY_H_L8):
	    break;
	default:
	    MP_TYPE(x,y) = CST_INDUSTRY_H_L1;
	}
    }
    else {
	MP_TYPE(x,y) = CST_INDUSTRY_H_C;
    }

    /* now animate */
    switch (MP_TYPE(x,y)) {
    case (CST_INDUSTRY_H_L1):
	MP_TYPE(x,y) = CST_INDUSTRY_H_L2;
	break;
    case (CST_INDUSTRY_H_L2):
	MP_TYPE(x,y) = CST_INDUSTRY_H_L3;
	break;
    case (CST_INDUSTRY_H_L3):
	MP_TYPE(x,y) = CST_INDUSTRY_H_L4;
	break;
    case (CST_INDUSTRY_H_L4):
	MP_TYPE(x,y) = CST_INDUSTRY_H_L5;
	break;
    case (CST_INDUSTRY_H_L5):
	MP_TYPE(x,y) = CST_INDUSTRY_H_L6;
	break;
    case (CST_INDUSTRY_H_L6):
	MP_TYPE(x,y) = CST_INDUSTRY_H_L7;
	break;
    case (CST_INDUSTRY_H_L7):
	MP_TYPE(x,y) = CST_INDUSTRY_H_L8;
	break;
    case (CST_INDUSTRY_H_L8):
	MP_TYPE(x,y) = CST_INDUSTRY_H_L1;
	break;

    case (CST_INDUSTRY_H_M1):
	MP_TYPE(x,y) = CST_INDUSTRY_H_M2;
	break;
    case (CST_INDUSTRY_H_M2):
	MP_TYPE(x,y) = CST_INDUSTRY_H_M3;
	break;
    case (CST_INDUSTRY_H_M3):
	MP_TYPE(x,y) = CST_INDUSTRY_H_M4;
	break;
    case (CST_INDUSTRY_H_M4):
	MP_TYPE(x,y) = CST_INDUSTRY_H_M5;
	break;
    case (CST_INDUSTRY_H_M5):
	MP_TYPE(x,y) = CST_INDUSTRY_H_M6;
	break;
    case (CST_INDUSTRY_H_M6):
	MP_TYPE(x,y) = CST_INDUSTRY_H_M7;
	break;
    case (CST_INDUSTRY_H_M7):
	MP_TYPE(x,y) = CST_INDUSTRY_H_M8;
	break;
    case (CST_INDUSTRY_H_M8):
	MP_TYPE(x,y) = CST_INDUSTRY_H_M1;
	break;

    case (CST_INDUSTRY_H_H1):
	MP_TYPE(x,y) = CST_INDUSTRY_H_H2;
	break;
    case (CST_INDUSTRY_H_H2):
	MP_TYPE(x,y) = CST_INDUSTRY_H_H3;
	break;
    case (CST_INDUSTRY_H_H3):
	MP_TYPE(x,y) = CST_INDUSTRY_H_H4;
	break;
    case (CST_INDUSTRY_H_H4):
	MP_TYPE(x,y) = CST_INDUSTRY_H_H5;
	break;
    case (CST_INDUSTRY_H_H5):
	MP_TYPE(x,y) = CST_INDUSTRY_H_H6;
	break;
    case (CST_INDUSTRY_H_H6):
	MP_TYPE(x,y) = CST_INDUSTRY_H_H7;
	break;
    case (CST_INDUSTRY_H_H7):
	MP_TYPE(x,y) = CST_INDUSTRY_H_H8;
	break;
    case (CST_INDUSTRY_H_H8):
	MP_TYPE(x,y) = CST_INDUSTRY_H_H1;
	break;
    }
}

void 
animate_commune (int x, int y)
{
  if (real_time < ANIM_TIMEOUT(x,y)) {
    return;
  }
  ANIM_TIMEOUT(x,y) = real_time + COMMUNE_ANIM_SPEED;

  if (MP_INFO(x,y).int_2 == 0) {
    if (MP_TYPE(x,y) > CST_COMMUNE_7) {
      MP_TYPE(x,y) -= 7;
    }
  } else if (MP_TYPE(x,y) <= CST_COMMUNE_7) {
    MP_TYPE(x,y) += 7;
  }

  switch (MP_TYPE(x,y))
    {
    case (CST_COMMUNE_1):
      MP_TYPE(x,y) = CST_COMMUNE_2;
      break;
    case (CST_COMMUNE_2):
      MP_TYPE(x,y) = CST_COMMUNE_3;
      break;
    case (CST_COMMUNE_3):
      MP_TYPE(x,y) = CST_COMMUNE_4;
      break;
    case (CST_COMMUNE_4):
      MP_TYPE(x,y) = CST_COMMUNE_5;
      break;
    case (CST_COMMUNE_5):
      MP_TYPE(x,y) = CST_COMMUNE_6;
      break;
    case (CST_COMMUNE_6):
      MP_TYPE(x,y) = CST_COMMUNE_1;
      break;
    case (CST_COMMUNE_8):
      MP_TYPE(x,y) = CST_COMMUNE_9;
      break;
    case (CST_COMMUNE_9):
      MP_TYPE(x,y) = CST_COMMUNE_10;
      break;
    case (CST_COMMUNE_10):
      MP_TYPE(x,y) = CST_COMMUNE_11;
      break;
    case (CST_COMMUNE_11):
      MP_TYPE(x,y) = CST_COMMUNE_12;
      break;
    case (CST_COMMUNE_12):
      MP_TYPE(x,y) = CST_COMMUNE_13;
      break;
    case (CST_COMMUNE_13):
      MP_TYPE(x,y) = CST_COMMUNE_8;
      break;
    }
}

void 
animate_oremine (int x, int y)
{
  int xx = 7 * (MP_INFO(x,y).int_2 + (3 * ORE_RESERVE / 2))
    / (16 * ORE_RESERVE);
  switch (xx)
    {
    case (0):
      MP_TYPE(x,y) = CST_OREMINE_8;
      break;
    case (1):
      MP_TYPE(x,y) = CST_OREMINE_7;
      break;
    case (2):
      MP_TYPE(x,y) = CST_OREMINE_6;
      break;
    case (3):
      MP_TYPE(x,y) = CST_OREMINE_5;
      break;
    case (4):
      MP_TYPE(x,y) = CST_OREMINE_4;
      break;
    case (5):
      MP_TYPE(x,y) = CST_OREMINE_3;
      break;
    case (6):
      MP_TYPE(x,y) = CST_OREMINE_2;
      break;
    case (7):
      MP_TYPE(x,y) = CST_OREMINE_1;
      break;
    }
}

void 
animate_tip (int x, int y)
{
  int i;
  if (MP_TYPE(x,y) == CST_TIP_8) {
    return;
  }

  i = (MP_INFO(x,y).int_1 * 7) / MAX_WASTE_AT_TIP;
  if (MP_INFO(x,y).int_1 > 0)
    i++;
  switch (i)
    {
    case (0):
      MP_TYPE(x,y) = CST_TIP_0;
      break;
    case (1):
      MP_TYPE(x,y) = CST_TIP_1;
      break;
    case (2):
      MP_TYPE(x,y) = CST_TIP_2;
      break;
    case (3):
      MP_TYPE(x,y) = CST_TIP_3;
      break;
    case (4):
      MP_TYPE(x,y) = CST_TIP_4;
      break;
    case (5):
      MP_TYPE(x,y) = CST_TIP_5;
      break;
    case (6):
      MP_TYPE(x,y) = CST_TIP_6;
      break;
    case (7):
      MP_TYPE(x,y) = CST_TIP_7;
      break;
    case (8):
      MP_TYPE(x,y) = CST_TIP_8;
      MP_INFO(x,y).int_2 = 0;
      break;
    }
}

void 
animate_rocket (int x, int y)
{
    if (MP_TYPE(x,y) == CST_ROCKET_FLOWN) {
	return;			/* The rocket has been launched. */
    }
    if (real_time < ANIM_TIMEOUT(x,y)) {
	return;
    }
    ANIM_TIMEOUT(x,y) = real_time + ROCKET_ANIMATION_SPEED;

    /* Rocket finished, but said no to launch. */
    if (MP_INFO(x,y).int_4 < (25 * ROCKET_PAD_LAUNCH) / 100)
	MP_TYPE(x,y) = CST_ROCKET_1;
    else if (MP_INFO(x,y).int_4 < (60 * ROCKET_PAD_LAUNCH) / 100)
	MP_TYPE(x,y) = CST_ROCKET_2;
    else if (MP_INFO(x,y).int_4 < (90 * ROCKET_PAD_LAUNCH) / 100)
	MP_TYPE(x,y) = CST_ROCKET_3;
    else if (MP_INFO(x,y).int_4 < (100 * ROCKET_PAD_LAUNCH) / 100)
	MP_TYPE(x,y) = CST_ROCKET_4;
    else if (MP_INFO(x,y).int_4 >= (100 * ROCKET_PAD_LAUNCH) / 100)
    {
	switch (MP_TYPE(x,y))
	{
	case CST_ROCKET_5:
	    MP_TYPE(x,y) = CST_ROCKET_6;
	    break;
	case CST_ROCKET_6:
	    MP_TYPE(x,y) = CST_ROCKET_7;
	    break;
	case CST_ROCKET_7:
	default:
	    MP_TYPE(x,y) = CST_ROCKET_5;
	    break;
	}
    }
}

void 
animate_monument (int x, int y)
{
  if (real_time < ANIM_TIMEOUT(x,y)) {
    return;
  }
  ANIM_TIMEOUT(x,y) = real_time + MONUMENT_ANIM_SPEED;

  if (MP_INFO(x,y).int_1 >= BUILD_MONUMENT_JOBS)
    MP_TYPE(x,y) = CST_MONUMENT_5;
  else if (MP_INFO(x,y).int_1 >= ((BUILD_MONUMENT_JOBS * 4) / 5))
    MP_TYPE(x,y) = CST_MONUMENT_4;
  else if (MP_INFO(x,y).int_1 >= ((BUILD_MONUMENT_JOBS * 3) / 5))
    MP_TYPE(x,y) = CST_MONUMENT_3;
  else if (MP_INFO(x,y).int_1 >= ((BUILD_MONUMENT_JOBS * 2) / 5))
    MP_TYPE(x,y) = CST_MONUMENT_2;
  else if (MP_INFO(x,y).int_1 >= (BUILD_MONUMENT_JOBS / 20))
    MP_TYPE(x,y) = CST_MONUMENT_1;
  else
    MP_TYPE(x,y) = CST_MONUMENT_0;
}

void 
animate_windmill (int x, int y)
{
    if (real_time < ANIM_TIMEOUT(x,y)) {
	return;
    }

    /* Antique windmill */
    if (MP_INFO(x,y).int_2 < MODERN_WINDMILL_TECH)
    {
	ANIM_TIMEOUT(x,y) = real_time + ANTIQUE_WINDMILL_ANIM_SPEED;
	switch (MP_TYPE(x,y)) {
	case CST_WINDMILL_1_W:
	    MP_TYPE(x,y) = CST_WINDMILL_2_W;
	    break;
	case CST_WINDMILL_2_W:
	    MP_TYPE(x,y) = CST_WINDMILL_3_W;
	    break;
	case CST_WINDMILL_3_W:
	default:
	    MP_TYPE(x,y) = CST_WINDMILL_1_W;
	    break;
	}
    }

    /* Modern windmill */
    else {
	int flag = 0;
	int advance = (MP_TYPE(x,y) - CST_WINDMILL_1_G + 1) % 3;
	ANIM_TIMEOUT(x,y) = real_time + MODERN_WINDMILL_ANIM_SPEED;

	/* Do this computation locally for better animation */
	if (MP_INFO(x,y).int_1 < POWER_LINE_CAPACITY) {
	    if (get_jobs (x, y, WINDMILL_JOBS) != 0) {
		MP_INFO(x,y).int_1 += MP_INFO(x,y).int_5;
		flag = 1;
	    }
	}

	if (MP_INFO(x,y).int_1 > POWER_LINE_CAPACITY / 2) {
	    MP_TYPE(x,y) = CST_WINDMILL_1_G + advance;
	} else if (flag) {
	    MP_TYPE(x,y) = CST_WINDMILL_1_RG + advance;
	} else {
	    advance = (advance + 2) % 3;  /* Don't advance graphic */
	    MP_TYPE(x,y) = CST_WINDMILL_1_R + advance;
	}
    }
}

void 
animate_blacksmith (int x, int y)
{
  if (real_time < ANIM_TIMEOUT(x,y)) {
    return;
  }
  ANIM_TIMEOUT(x,y) = real_time + BLACKSMITH_ANIM_SPEED;

  /* Animate based on previous month. */
  if (MP_INFO(x,y).int_6 < BLACKSMITH_ANIM_THRESHOLD) {
    MP_TYPE(x,y) = CST_BLACKSMITH_0;
    return;
  }
    
  switch (MP_TYPE(x,y))
    {
    case (CST_BLACKSMITH_0):
      MP_TYPE(x,y) = CST_BLACKSMITH_1;
      break;
    case (CST_BLACKSMITH_1):
      MP_TYPE(x,y) = CST_BLACKSMITH_2;
      break;
    case (CST_BLACKSMITH_2):
      MP_TYPE(x,y) = CST_BLACKSMITH_3;
      break;
    case (CST_BLACKSMITH_3):
      MP_TYPE(x,y) = CST_BLACKSMITH_4;
      break;
    case (CST_BLACKSMITH_4):
      MP_TYPE(x,y) = CST_BLACKSMITH_5;
      break;
    case (CST_BLACKSMITH_5):
      MP_TYPE(x,y) = CST_BLACKSMITH_6;
      break;
    case (CST_BLACKSMITH_6):
      MP_TYPE(x,y) = CST_BLACKSMITH_1;
      break;
    }
}

void 
animate_firestation (int x, int y)
{
  if (real_time < ANIM_TIMEOUT(x,y)) {
    return;
  }
  ANIM_TIMEOUT(x,y) = real_time + FIRESTATION_ANIMATION_SPEED;

  if (MP_INFO(x,y).int_3 == 0 && MP_TYPE(x,y) == CST_FIRESTATION_1) {
    return;
  }

  switch (MP_TYPE(x,y)) {
  case (CST_FIRESTATION_1):
    MP_TYPE(x,y) = CST_FIRESTATION_2;
    break;
  case (CST_FIRESTATION_2):
    MP_TYPE(x,y) = CST_FIRESTATION_3;
    break;
  case (CST_FIRESTATION_3):
    MP_TYPE(x,y) = CST_FIRESTATION_4;
    break;
  case (CST_FIRESTATION_4):
    MP_TYPE(x,y) = CST_FIRESTATION_5;
    break;
  case (CST_FIRESTATION_5):
    MP_TYPE(x,y) = CST_FIRESTATION_6;
    break;
  case (CST_FIRESTATION_6):
    MP_TYPE(x,y) = CST_FIRESTATION_7;
    ANIM_TIMEOUT(x,y) += 10 * FIRESTATION_ANIMATION_SPEED; /* pause */
    break;
  case (CST_FIRESTATION_7):
    MP_TYPE(x,y) = CST_FIRESTATION_8;
    break;
  case (CST_FIRESTATION_8):
    MP_TYPE(x,y) = CST_FIRESTATION_9;
    break;
  case (CST_FIRESTATION_9):
    MP_TYPE(x,y) = CST_FIRESTATION_10;
    break;
  case (CST_FIRESTATION_10):
    MP_TYPE(x,y) = CST_FIRESTATION_1;
    MP_INFO(x,y).int_3 = 0;		/* stop */
    break;
  }
}

void 
animate_cricket (int x, int y)
{
  if (real_time < ANIM_TIMEOUT(x,y)) {
    return;
  }
  ANIM_TIMEOUT(x,y) = real_time + CRICKET_ANIMATION_SPEED;

  if (MP_INFO(x,y).int_3 == 0 && MP_TYPE(x,y) == CST_CRICKET_1) {
    return;
  }

  switch (MP_TYPE(x,y)) {
  case (CST_CRICKET_1):
    MP_TYPE(x,y) = CST_CRICKET_2;
    break;
  case (CST_CRICKET_2):
    MP_TYPE(x,y) = CST_CRICKET_3;
    break;
  case (CST_CRICKET_3):
    MP_TYPE(x,y) = CST_CRICKET_4;
    break;
  case (CST_CRICKET_4):
    MP_TYPE(x,y) = CST_CRICKET_5;
    break;
  case (CST_CRICKET_5):
    MP_TYPE(x,y) = CST_CRICKET_6;
    break;
  case (CST_CRICKET_6):
    MP_TYPE(x,y) = CST_CRICKET_7;
    break;
  case (CST_CRICKET_7):
    MP_TYPE(x,y) = CST_CRICKET_1;
    MP_INFO(x,y).int_3 = 0;	/* disable anim */
    break;
  }
}

void 
animate_fire (int x, int y)
{
    if (MP_TYPE(x,y) > CST_FIRE_DONE1) {
	return;
    }
    if (real_time < ANIM_TIMEOUT(x,y)) {
	return;
    }
    ANIM_TIMEOUT(x,y) = real_time + FIRE_ANIMATION_SPEED;

    if (MP_TYPE(x,y) == CST_FIRE_1)
	MP_TYPE(x,y) = CST_FIRE_2;
    else if (MP_TYPE(x,y) == CST_FIRE_2)
	MP_TYPE(x,y) = CST_FIRE_3;
    else if (MP_TYPE(x,y) == CST_FIRE_3)
	MP_TYPE(x,y) = CST_FIRE_4;
    else if (MP_TYPE(x,y) == CST_FIRE_4)
	MP_TYPE(x,y) = CST_FIRE_5;
    else if (MP_TYPE(x,y) == CST_FIRE_5)
	MP_TYPE(x,y) = CST_FIRE_1;
}

void 
animate_mill (int x, int y)
{
  if (real_time < ANIM_TIMEOUT(x,y)) {
    return;
  }
  ANIM_TIMEOUT(x,y) = real_time + MILL_ANIM_SPEED;

  /* Animate based on previous month. */
  if (MP_INFO(x,y).int_6 < MILL_ANIM_THRESHOLD) {
    MP_TYPE(x,y) = CST_MILL_0;
    return;
  }

  switch (MP_TYPE(x,y))
    {
    case (CST_MILL_0):
      MP_TYPE(x,y) = CST_MILL_1;
      break;
    case (CST_MILL_1):
      MP_TYPE(x,y) = CST_MILL_2;
      break;
    case (CST_MILL_2):
      MP_TYPE(x,y) = CST_MILL_3;
      break;
    case (CST_MILL_3):
      MP_TYPE(x,y) = CST_MILL_4;
      break;
    case (CST_MILL_4):
      MP_TYPE(x,y) = CST_MILL_5;
      break;
    case (CST_MILL_5):
      MP_TYPE(x,y) = CST_MILL_6;
      break;
    case (CST_MILL_6):
      MP_TYPE(x,y) = CST_MILL_1;
      break;
    }
}

void 
animate_pottery (int x, int y)
{
  if (real_time < ANIM_TIMEOUT(x,y)) {
    return;
  }
  ANIM_TIMEOUT(x,y) = real_time + POTTERY_ANIM_SPEED;

  if (MP_INFO(x,y).int_5 < 0 ||
      MP_INFO(x,y).int_2 < POTTERY_ORE_MAKE_GOODS ||
      MP_INFO(x,y).int_3 < POTTERY_COAL_MAKE_GOODS ||
      MP_INFO(x,y).int_7 < POTTERY_JOBS) {
    MP_TYPE(x,y) = CST_POTTERY_1;
    return;
  }

  switch (MP_TYPE(x,y))
    {
    case (CST_POTTERY_0):
      MP_TYPE(x,y) = CST_POTTERY_1;
      break;
    case (CST_POTTERY_1):
      MP_TYPE(x,y) = CST_POTTERY_2;
      break;
    case (CST_POTTERY_2):
      MP_TYPE(x,y) = CST_POTTERY_3;
      break;
    case (CST_POTTERY_3):
      MP_TYPE(x,y) = CST_POTTERY_4;
      break;
    case (CST_POTTERY_4):
      MP_TYPE(x,y) = CST_POTTERY_5;
      break;
    case (CST_POTTERY_5):
      MP_TYPE(x,y) = CST_POTTERY_6;
      break;
    case (CST_POTTERY_6):
      MP_TYPE(x,y) = CST_POTTERY_7;
      break;
    case (CST_POTTERY_7):
      MP_TYPE(x,y) = CST_POTTERY_8;
      break;
    case (CST_POTTERY_8):
      MP_TYPE(x,y) = CST_POTTERY_9;
      break;
    case (CST_POTTERY_9):
      MP_TYPE(x,y) = CST_POTTERY_10;
      break;
    case (CST_POTTERY_10):
      MP_TYPE(x,y) = CST_POTTERY_1;
      break;
    }
}


