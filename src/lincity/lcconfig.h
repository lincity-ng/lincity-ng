/* ---------------------------------------------------------------------- *
 * lcconfig.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __lcconfig_h__
#define __lcconfig_h__

#if defined (HAVE_CONFIG_H)
#include "config.h"
#elif defined (WIN32)
#include "confw32.h"
#endif

#if (SIZEOF_SHORT==4)
typedef short Int32;
#elif (SIZEOF_INT==4)
typedef int Int32;
#elif (SIZEOF_LONG==4)
typedef long Int32;
#else
#error  Int32 is not defined.
#endif

#if (SIZEOF_SHORT==2)
typedef short Int16;
#elif (SIZEOF_INT==2)
typedef int Int16;
#else
#error  Int16 is not defined.
#endif

#endif	/* __lcconfig_h__ */
