/* ---------------------------------------------------------------------- *
 * fileutil.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __fileutil_h__
#define __fileutil_h__

extern char *lc_save_dir;
extern char *lc_temp_file;
extern int lc_save_dir_len;


void gunzip_file (char *f1, char *f2);
int file_exists (char *filename);
int directory_exists (char *dir);
char* load_graphic(char *s);

#endif	/* __fileutil_h__ */
