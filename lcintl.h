/* ---------------------------------------------------------------------- *
 * lcintl.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __lcintl_h__
#define __lcintl_h__

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#ifdef ENABLE_NLS
#include <libintl.h>
#define _(String) gettext(String)
#define N_(String) gettext_noop(String)
#else
#define _(String) String
#define N_(String) String
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)
#endif

#endif	/* __lcintl_h__ */
