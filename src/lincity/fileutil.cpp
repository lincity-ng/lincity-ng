 /* ---------------------------------------------------------------------- *
  * fileutil.c
  * This file is part of lincity.
  * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
  * ---------------------------------------------------------------------- */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>             /* XXX: GCS FIX: What does configure need to know? */
#include <string.h>
#include <physfs.h>
#include "engglobs.h"
#include "gui_interface/screen_interface.h"
#include "tinygettext/gettext.hpp"
#include "lincity-ng/ErrorInterface.hpp"

/* XXX: Where are SVGA specific includes? */

/* this is for OS/2 - RVI */
#ifdef __EMX__
#include <sys/select.h>
#include <X11/Xlibint.h>        /* required for __XOS2RedirRoot */
#define chown(x,y,z)
#define OS2_DEFAULT_LIBDIR "/XFree86/lib/X11/lincity"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef _MSC_VER
#include <direct.h>
#include <io.h>
#endif

#if defined (TIME_WITH_SYS_TIME)
#include <time.h>
#include <sys/time.h>
#else
#if defined (HAVE_SYS_TIME_H)
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif

/*
#if defined (WIN32)
#include <winsock.h>
#if defined (__BORLANDC__)
#include <dir.h>
#include <dirent.h>
#include <dos.h>
#endif
#include <io.h>
#include <direct.h>
#include <process.h>
#include "lcwin32.h"
#endif
*/
#if defined (HAVE_DIRENT_H)
#include <dirent.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#define dirent direct
#define NAMLEN(dirent) (dirent)->d_namlen
#if defined (HAVE_SYS_NDIR_H)
#include <sys/ndir.h>
#endif
#if defined (HAVE_SYS_DIR_H)
#include <sys/dir.h>
#endif
#if defined (HAVE_NDIR_H)
#include <ndir.h>
#endif
#endif

#include <ctype.h>
#if defined (HAVE_UNISTD_H)
#include <unistd.h>
#endif
#include <errno.h>
/*
#include "common.h"
#ifdef LC_X11
#include <X11/cursorfont.h>
#include <lcx11.h>
#endif
*/
#include "lctypes.h"
#include "lin-city.h"
//#include "cliglobs.h"
#include "engglobs.h"
#include "fileutil.h"
#include "loadsave.h"

/* GCS: This is from dcgettext.c in the gettext package.      */
/* XPG3 defines the result of `setlocale (category, NULL)' as:
   ``Directs `setlocale()' to query `category' and return the current
     setting of `local'.''
   However it does not specify the exact format.  And even worse: POSIX
   defines this not at all.  So we can use this feature only on selected
   system (e.g. those using GNU C Library).  */
#ifdef _LIBC
# define HAVE_LOCALE_NULL
#endif

#define DEBUG_PRINTF_TO_FILE 0
void debug_printf(char *fmt, ...);

/* ---------------------------------------------------------------------- *
 * Private Fn Prototypes
 * ---------------------------------------------------------------------- */
void dump_screen(void);
void verify_package(void);
static const char *guess_category_value(int category, const char *categoryname);

/* ---------------------------------------------------------------------- *
 * Public Global Variables
 * ---------------------------------------------------------------------- */
#ifdef LIBDIR
#undef LIBDIR
#endif
char LIBDIR[LC_PATH_MAX];

char *lc_save_dir;
int lc_save_dir_len;
static char *lc_temp_filename;

char given_scene[LC_PATH_MAX];
char colour_pal_file[LC_PATH_MAX];
char opening_pic[LC_PATH_MAX];
char graphic_path[LC_PATH_MAX];
char fontfile[LC_PATH_MAX];
char opening_path[LC_PATH_MAX];
char help_path[LC_PATH_MAX];
char message_path[LC_PATH_MAX];
char lc_textdomain_directory[LC_PATH_MAX];
char lincityrc_file[LC_PATH_MAX];

/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */
#if defined (__BORLANDC__)
int _access(const char *path, int mode)
{
    return access(path, mode)
}
#endif

int directory_exists(char *dir)
{
#if defined (WIN32)
    struct stat s;
    if (stat(dir, &s) != 0 || !(s.st_mode & S_IFDIR)) {
        return 0;
    }
#else /* UNIX */
    DIR *dp;
    if ((dp = opendir(dir)) == NULL) {
        return 0;
    }
    closedir(dp);
#endif
    return 1;
}

int file_exists(char *filename)
{
    FILE *fp;
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        return 0;
    }
    fclose(fp);
    return 1;
}

#if defined (WIN32)
void find_libdir(void)
{
    const char searchfile[] = "Colour.pal";
    /* default_dir will be something like "C:\\LINCITY1.11" */
    const char default_dir[] = "C:\\LINCITY" PACKAGE_VERSION;
//    const char default_dir[] = "D:\\LINCITY"; /* For GCS's use */

    /* Check 1: environment variable */
    _searchenv(searchfile, "LINCITY_HOME", LIBDIR);
    if (*LIBDIR != '\0') {
        int endofpath_offset = strlen(LIBDIR) - strlen(searchfile) - 1;
        LIBDIR[endofpath_offset] = '\0';
        return;
    }

    /* Check 2: default location */
    if ((_access(default_dir, 0)) != -1) {
        strcpy(LIBDIR, default_dir);
        return;
    }

    /* Finally give up */
    HandleError(_("Error. Can't find LINCITY_HOME"), FATAL);
}

#elif defined (__EMX__)
void find_libdir(void)
{
    strcpy(LIBDIR, __XOS2RedirRoot(OS2_DEFAULT_LIBDIR));
}

#else /* Unix with configure */
void find_libdir(void)
{
    const char searchfile[] = "colour.pal";
    char *home_dir, *cwd;
    char cwd_buf[LC_PATH_MAX];
    char filename_buf[LC_PATH_MAX];

    /* Check 1: environment variable */
    home_dir = getenv("LINCITY_HOME");
    if (home_dir) {
        snprintf(filename_buf, LC_PATH_MAX, "%s%c%s", home_dir, PATH_SLASH, searchfile);
        if (file_exists(filename_buf)) {
            strncpy(LIBDIR, home_dir, LC_PATH_MAX);
            return;
        }
    }

    /* Check 2: current working directory */
    cwd = getcwd(cwd_buf, LC_PATH_MAX);
    if (cwd) {
        snprintf(filename_buf, LC_PATH_MAX, "%s%c%s", cwd_buf, PATH_SLASH, searchfile);
        if (file_exists(filename_buf)) {
            strncpy(LIBDIR, cwd_buf, LC_PATH_MAX);
            return;
        }
    }

    snprintf(filename_buf, LC_PATH_MAX, "%s%c%s", DEFAULT_LIBDIR, PATH_SLASH, searchfile);
    if (file_exists(filename_buf)) {
        strncpy(LIBDIR, DEFAULT_LIBDIR, LC_PATH_MAX);
        return;
    }

    /* Finally give up */
    HandleError(_("Error. Can't find LINCITY_HOME"), FATAL);
}
#endif

/* GCS:  This function comes from dcgettext.c in the gettext package.      */
/* Guess value of current locale from value of the environment variables.  */
/* GCS Feb 23, 2003.  This was updated in gettext, but I'm going with the  */
/* old version here. */
static const char *guess_category_value(int category, const char *categoryname)
{
    (void)category;
    const char *retval;

    /* The highest priority value is the `LANGUAGE' environment
       variable.  This is a GNU extension.  */
    retval = getenv("LANGUAGE");
    if (retval != NULL && retval[0] != '\0')
        return retval;

    /* `LANGUAGE' is not set.  So we have to proceed with the POSIX
       methods of looking to `LC_ALL', `LC_xxx', and `LANG'.  On some
       systems this can be done by the `setlocale' function itself.  */
#if defined HAVE_SETLOCALE && defined HAVE_LC_MESSAGES && defined HAVE_LOCALE_NULL
    retval = setlocale(category, NULL);
    if (retval != NULL)
        return retval;
    else
        return "C";
#else
    /* Setting of LC_ALL overwrites all other.  */
    retval = getenv("LC_ALL");
    if (retval != NULL && retval[0] != '\0')
        return retval;

    /* Next comes the name of the desired category.  */
    retval = getenv(categoryname);
    if (retval != NULL && retval[0] != '\0')
        return retval;

    /* Last possibility is the LANG environment variable.  */
    retval = getenv("LANG");
    if (retval != NULL && retval[0] != '\0')
        return retval;

    /* We use C as the default domain.  POSIX says this is implementation
       defined.  */
    return "C";
#endif
}

/* GCS:  This function is modified from gettext.  It finds the language
   portion of the locale. */
static void lincity_nl_find_language(char *name)
{
    while (name[0] != '\0' && name[0] != '_' && name[0] != '@' && name[0] != '+' && name[0] != ',')
        ++name;

    *name = '\0';
}

void find_localized_paths(void)
{
    int messages_done = 0;
    int help_done = 0;

    const char *intl_suffix = "";
    char intl_lang[128];

    /* First, try the locale "as is" */
#if defined (ENABLE_NLS) && defined (HAVE_LC_MESSAGES)
    intl_suffix = guess_category_value(LC_MESSAGES, "LC_MESSAGES");
#else
    intl_suffix = guess_category_value(0, "LC_MESSAGES");
#endif
    debug_printf((char*)"GUESS 1 -- intl_suffix is %s\n", intl_suffix);
    if (strcmp(intl_suffix, "C") && strcmp(intl_suffix, "")) {
        snprintf(message_path, sizeof(message_path), "%s%c%s%c%s%c", LIBDIR,
                 PATH_SLASH, "messages", PATH_SLASH, intl_suffix, PATH_SLASH);
        debug_printf((char*)"Trying Message Path %s\n", message_path);
        if (directory_exists(message_path)) {
            debug_printf((char*)"Set Message Path %s\n", message_path);
            messages_done = 1;
        }
        snprintf(help_path, sizeof(help_path), "%s%c%s%c%s%c", LIBDIR, PATH_SLASH,
                 "help", PATH_SLASH, intl_suffix, PATH_SLASH);
        debug_printf((char*)"Trying Help Path %s\n", help_path);
        if (directory_exists(help_path)) {
            debug_printf((char*)"Set Help Path %s\n", help_path);
            help_done = 1;
        }
    }
    if (messages_done && help_done)
        return;

    /* Next, try stripping off the country suffix */
    strncpy(intl_lang, intl_suffix, 128);
    intl_lang[127] = '\0';
    lincity_nl_find_language(intl_lang);
    intl_suffix = intl_lang;
    debug_printf((char*)"GUESS 2 -- intl_suffix is %s\n", intl_suffix);
    if (strcmp(intl_suffix, "C") && strcmp(intl_suffix, "")) {
        if (!messages_done) {
            sprintf(message_path, "%s%c%s%c%s%c", LIBDIR, PATH_SLASH, "messages", PATH_SLASH, intl_suffix, PATH_SLASH);
            debug_printf((char*)"Trying Message Path %s\n", message_path);
            if (directory_exists(message_path)) {
                debug_printf((char*)"Set Message Path %s\n", message_path);
                messages_done = 1;
            }
        }
        if (!help_done) {
            sprintf(help_path, "%s%c%s%c%s%c", LIBDIR, PATH_SLASH, "help", PATH_SLASH, intl_suffix, PATH_SLASH);
            debug_printf((char*)"Trying Help Path %s\n", help_path);
            if (directory_exists(help_path)) {
                debug_printf((char*)"Set Help Path %s\n", help_path);
                help_done = 1;
            }
        }
    }
    if (messages_done && help_done)
        return;

    /* Finally, settle for default English messages */
    if (!messages_done) {
        sprintf(message_path, "%s%c%s%c", LIBDIR, PATH_SLASH, "messages", PATH_SLASH);
        debug_printf((char*)"Settling for message Path %s\n", message_path);
    }
    if (!help_done) {
        sprintf(help_path, "%s%c%s%c", LIBDIR, PATH_SLASH, "help", PATH_SLASH);
        debug_printf((char*)"Settling for help Path %s\n", help_path);
    }
}

void init_path_strings(void)
{
    find_libdir();
    //TODO: use, remove unused vars.
    const char* homedir = PHYSFS_getUserDir();

    /* Various dirs and files */
    lc_save_dir_len = strlen(homedir) + strlen(LC_SAVE_DIR) + 1;
    if ((lc_save_dir = (char *)malloc(lc_save_dir_len + 1)) == 0)
        malloc_failure();
    sprintf(lc_save_dir, "%s%c%s", homedir, PATH_SLASH, LC_SAVE_DIR);
    sprintf(colour_pal_file, "%s%c%s", LIBDIR, PATH_SLASH, "colour.pal");
    sprintf(opening_path, "%s%c%s", LIBDIR, PATH_SLASH, "opening");
#if defined (WIN32)
    sprintf(opening_pic, "%s%c%s", opening_path, PATH_SLASH, "open.tga");
#else
    sprintf(opening_pic, "%s%c%s", opening_path, PATH_SLASH, "open.tga.gz");
#endif
    sprintf(graphic_path, "%s%c%s%c", LIBDIR, PATH_SLASH, "icons", PATH_SLASH);
    sprintf(lincityrc_file, "%s%c%s", homedir, PATH_SLASH, LINCITYRC_FILENAME);

    /* Paths for message & help files, etc */
    find_localized_paths();

    /* Font stuff */
    sprintf(fontfile, "%s%c%s", opening_path, PATH_SLASH, "iso8859-1-8x8.raw");

    /* Temp file for results */
    lc_temp_filename = (char *)malloc(lc_save_dir_len + 16);
    if (lc_temp_filename == 0) {
        malloc_failure();
    }
    sprintf(lc_temp_filename, "%s%c%s", lc_save_dir, PATH_SLASH, "tmp-file");

    /* Path for localization */
#if defined (ENABLE_NLS)
#if defined (WIN32)
    sprintf(lc_textdomain_directory, "%s%c%s", LIBDIR, PATH_SLASH, "locale");
#else
    strcpy(lc_textdomain_directory, LOCALEDIR);
#endif
    char *dm = NULL;
    char *td = NULL;
    dm = bindtextdomain(PACKAGE, lc_textdomain_directory);
    debug_printf("Bound textdomain directory is %s\n", dm);
    td = textdomain(PACKAGE);
    debug_printf("Textdomain is %s\n", td);
#endif
}

void verify_package(void)
{
    FILE *fp = fopen(colour_pal_file, "rb");
    if (!fp) {
        do_error(_("Error verifying package. Can't find colour.pal."));
    }
    fclose(fp);
}

void make_savedir(void)
{
#if !defined (WIN32)
    DIR *dp;
#endif

#if defined (WIN32)
    if (_mkdir(lc_save_dir) == -1 && errno != EEXIST) {
        printf("%s '%s'\n", _("Couldn't create the save directory"), lc_save_dir);
        exit(-1);
    }
#else
    mkdir(lc_save_dir, 0755);
    if (chown(lc_save_dir, getuid(), getgid()) == -1) {
        printf("%s '%s'\n", _("Failed to chown"), lc_save_dir);
    }
    if ((dp = opendir(lc_save_dir)) == NULL) {
        /* change this to a screen message. */
        printf("%s '%s'\n", _("Couldn't create the save directory"), lc_save_dir);
        exit(1);
    }
    closedir(dp);
#endif

}

void check_savedir(void)
{
    if (!directory_exists(lc_save_dir)) {
        make_savedir();
    }
}

void malloc_failure(void)
{
    printf("%s",_("Out of memory: malloc failure\n"));
    exit(1);
}

char *load_graphic(char *s)
{
    int x, l;
    char ss[LC_PATH_MAX], *graphic;
    FILE *inf;
    strcpy(ss, graphic_path);
    strcat(ss, s);
    if ((inf = fopen(ss, "rb")) == NULL) {
        strcat(ss, " -- UNABLE TO LOAD");
        do_error(ss);
    }
    fseek(inf, 0L, SEEK_END);
    l = ftell(inf);
    fseek(inf, 0L, SEEK_SET);
    graphic = (char *)malloc(l);
    for (x = 0; x < l; x++)
        *(graphic + x) = fgetc(inf);
    fclose(inf);
    return (graphic);
}

void undosify_string(char *s)
{
    /* Convert '\r\n' to '\n' in string */
    char prev_char = 0;
    char *p = s, *q = s;
    while (*p) {
        if (*p != '\r') {
            if (prev_char == '\r' && *p != '\n') {
                *q++ = '\n';
            }
            *q++ = *p;
        }
        prev_char = *p;
        p++;
    }
    if (prev_char == '\r') {
        *q++ = '\n';
    }
    *q = '\0';
}

void debug_printf(char *fmt, ...)
{
    (void)fmt;
#if (DEBUG_PRINTF_TO_FILE)
    static int initialized = 0;
    char *filename = "debug.txt";
    FILE *fp;
#endif
    va_list argptr;
    (void)argptr;

#if (DEBUG_PRINTF_TO_FILE)
    va_start(argptr, fmt);
    fp = fopen(filename, "a");
    if (!initialized) {
        initialized = 1;
        fprintf(fp, "=========================\n");
    }
    vfprintf(fp, fmt, argptr);
#endif

    /* FIXME: maybe uncomment this
       if (command_line_debug) {
       #if (!DEBUG_PRINTF_TO_FILE)
       va_start (argptr, fmt);
       #endif
       vprintf (fmt, argptr);
       #if (!DEBUG_PRINTF_TO_FILE)
       va_end (argptr);
       #endif
       }
     */
#if (DEBUG_PRINTF_TO_FILE)
    va_end(argptr);
    fclose(fp);
#endif
}

/** @file lincity/fileutil.cpp */

