/* ---------------------------------------------------------------------- *
 * residence.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#ifndef __residence_h__
#define __residence_h__

/* RESIDENCE?_BRM is the birth rate modifier */
#define RESIDENCE_BASE_BR     100
#define RESIDENCE_BASE_DR     (68*12)
#define RESIDENCE_BRM_HEALTH  5
#define RESIDENCE1_BRM (RESIDENCE_BASE_BR*12)
#define RESIDENCE2_BRM 0
#define RESIDENCE3_BRM (RESIDENCE_BASE_BR+RESIDENCE_BASE_BR/4)
#define RESIDENCE4_BRM (RESIDENCE_BASE_BR*18)
#define RESIDENCE5_BRM (RESIDENCE_BASE_BR/2)
#define RESIDENCE6_BRM 0

/* RESIDENCE?_DRM is the death rate modifier */
/* AL1 unused in ng. computed in residence.cpp 
#define RESIDENCE1_DRM ((RESIDENCE_BASE_DR*3)/4)
#define RESIDENCE2_DRM 0
#define RESIDENCE3_DRM ((RESIDENCE_BASE_DR)/2)
#define RESIDENCE4_DRM ((RESIDENCE_BASE_DR*3)/4)
#define RESIDENCE5_DRM ((RESIDENCE_BASE_DR)/4)
#define RESIDENCE6_DRM ((RESIDENCE_BASE_DR)/4)
*/

/* RESIDENCE_PPM is the people_pool mobitily. Higher number=less mobile. */
#define RESIDENCE_PPM   20

#define GROUP_RESIDENCE_LL_COLOUR (cyan(24))
#define GROUP_RESIDENCE_LL_COST 1000
#define GROUP_RESIDENCE_LL_COST_MUL 25
#define GROUP_RESIDENCE_LL_BUL_COST 1000
#define GROUP_RESIDENCE_LL_TECH 0
#define GROUP_RESIDENCE_LL_FIREC 75

#define GROUP_RESIDENCE_ML_COLOUR (cyan(24))
#define GROUP_RESIDENCE_ML_COST 2000
#define GROUP_RESIDENCE_ML_COST_MUL 25
#define GROUP_RESIDENCE_ML_BUL_COST 1000
#define GROUP_RESIDENCE_ML_TECH 0
#define GROUP_RESIDENCE_ML_FIREC 75

#define GROUP_RESIDENCE_HL_COLOUR (cyan(24))
#define GROUP_RESIDENCE_HL_COST 4000
#define GROUP_RESIDENCE_HL_COST_MUL 25
#define GROUP_RESIDENCE_HL_BUL_COST 1000
#define GROUP_RESIDENCE_HL_TECH 0
#define GROUP_RESIDENCE_HL_FIREC 75

#define GROUP_RESIDENCE_LH_COLOUR (cyan(24))
#define GROUP_RESIDENCE_LH_COST 800
#define GROUP_RESIDENCE_LH_COST_MUL 25
#define GROUP_RESIDENCE_LH_BUL_COST 1000
#define GROUP_RESIDENCE_LH_TECH 0       /* ?? */
#define GROUP_RESIDENCE_LH_FIREC 75

#define GROUP_RESIDENCE_MH_COLOUR (cyan(24))
#define GROUP_RESIDENCE_MH_COST 1600
#define GROUP_RESIDENCE_MH_COST_MUL 25
#define GROUP_RESIDENCE_MH_BUL_COST 1000
#define GROUP_RESIDENCE_MH_TECH 0
#define GROUP_RESIDENCE_MH_FIREC 75

#define GROUP_RESIDENCE_HH_COLOUR (cyan(24))
#define GROUP_RESIDENCE_HH_COST 3200
#define GROUP_RESIDENCE_HH_COST_MUL 25
#define GROUP_RESIDENCE_HH_BUL_COST 1000
#define GROUP_RESIDENCE_HH_TECH 0
#define GROUP_RESIDENCE_HH_FIREC 75



#endif /* __residence_h__ */

/** @file lincity/modules/residence.h */

