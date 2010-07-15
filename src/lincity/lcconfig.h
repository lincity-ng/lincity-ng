/* ---------------------------------------------------------------------- *
 * lcconfig.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __lcconfig_h__
#define __lcconfig_h__

#if defined (WIN32)
#include "confw32.h"
#else
#include <config.h>
#endif

#ifndef _MSC_VER
//#warning deprecated header
#else
#pragma message(__FILE__ ": deprecated header")
#endif

#endif /* __lcconfig_h__ */

/** @file lincity/lcconfig.h */

