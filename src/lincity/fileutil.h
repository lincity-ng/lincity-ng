/* ---------------------------------------------------------------------- *
 * fileutil.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __fileutil_h__
#define __fileutil_h__

void init_path_strings(void);
void check_savedir(void);
void check_endian(void);
void eswap32(int *);
void eswap16(unsigned short *);
void malloc_failure(void);


extern char *lc_save_dir;
extern char *lc_temp_file;
extern int lc_save_dir_len;
//extern int make_dir_ok_flag;

/* Try to find the maximum file name from OS */
#if defined (_POSIX_PATH_MAX)   /* Posix */
#define LC_PATH_MAX _POSIX_PATH_MAX
#elif defined (_MAX_PATH)       /* Win32 */
#define LC_PATH_MAX _MAX_PATH
#elif defined (PATH_MAX)        /* X Windows */
#define LC_PATH_MAX PATH_MAX
#else
#define LC_PATH_MAX 4096
#endif

extern char colour_pal_file[LC_PATH_MAX];
extern char opening_pic[LC_PATH_MAX];
extern char graphic_path[LC_PATH_MAX];
extern char fontfile[LC_PATH_MAX];
extern char opening_path[LC_PATH_MAX];
extern char help_path[LC_PATH_MAX];
extern char message_path[LC_PATH_MAX];
extern char lc_textdomain_directory[LC_PATH_MAX];
extern char lincityrc_file[LC_PATH_MAX];
extern char given_scene[LC_PATH_MAX];

//void gunzip_file (char *f1, char *f2); //unused function
int file_exists(char *filename);
int directory_exists(char *dir);
char *load_graphic(char *s);
void undosify_string(char *s);
void load_lincityrc(void);
void save_lincityrc(void);

void debug_printf(char *fmt, ...);

#endif /* __fileutil_h__ */

/** @file lincity/fileutil.h */

