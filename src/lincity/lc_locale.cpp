#include "lc_locale.h"
#include <stdlib.h>

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
        debug_printf("Environment variable LANGUAGE is %s\n", language);
        snprintf(language_buf, MAX_LANG_BUF, "LANGUAGE=%s", language);
        gettext_putenv(language_buf);
    } else {
        debug_printf("Environment variable LANGUAGE not set.\n");
    }
#else
    locale = setlocale(LC_ALL, "");
    debug_printf("Setting entire locale to %s\n", locale);
    locale = setlocale(LC_MESSAGES, "");
    debug_printf("Setting messages locale to %s\n", locale);
    localem = setlocale(LC_MESSAGES, NULL);
    debug_printf("Query locale is %s\n", localem);
#endif
#endif /* ENABLE_NLS */
    return;
}

/** @file lincity/lc_locale.cpp */

