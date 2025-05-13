/* ---------------------------------------------------------------------- *
 * src/lincity/lc_locale.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#include "lc_locale.hpp"

#ifdef WIN32
#include <stdlib.h>
#endif

void lincity_set_locale(void)
{
//    char *locale = NULL;
//    char *localem = NULL;
#if defined (WIN32)
#define MAX_LANG_BUF 1024
    char *language = NULL;
    char language_buf[MAX_LANG_BUF];
#endif

#if defined (ENABLE_NLS)
#if defined (WIN32)
    /* Some special stoopid way of setting locale for microsoft gettext */
    language = getenv("LANGUAGE");
    if (language) {
        fprintf(stderr, "Environment variable LANGUAGE is %s\n", language);
        snprintf(language_buf, MAX_LANG_BUF, "LANGUAGE=%s", language);
        gettext_putenv(language_buf);
    } else {
        fprintf(stderr, "Environment variable LANGUAGE not set.\n");
    }
#else
    locale = setlocale(LC_ALL, "");
    fprintf(stderr, "Setting entire locale to %s\n", locale);
    locale = setlocale(LC_MESSAGES, "");
    fprintf(stderr, "Setting messages locale to %s\n", locale);
    localem = setlocale(LC_MESSAGES, NULL);
    fprintf(stderr, "Query locale is %s\n", localem);
#endif
#endif /* ENABLE_NLS */
    return;
}

/** @file lincity/lc_locale.cpp */
