/*
Copyright (C) 2005 Jimmy Salmon

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef __READDIR_HPP__
#define __READDIR_HPP__

#if defined(HAVE_DIRENT_H) && defined(HAVE_SYS_TYPES_H)
#include <dirent.h>
#include <sys/types.h>
#else
#ifdef WIN32

#include <windows.h>
#include <io.h>

struct dirent {
    unsigned short d_reclen;
    char d_name[MAX_PATH];
};
typedef struct DIR {
    long handle;
    char path[MAX_PATH];
    struct _finddata_t info;
    int first;
    struct dirent entry;
} DIR;

DIR *opendir(const char *name);
struct dirent *readdir(DIR *dir);
void rewinddir(DIR *dir);
int closedir(DIR *dir);
#else
// Neither dirent.h, sys/types.h are present nor can we use the win32
// implementation
#error No readdir implementation 
#endif

#endif

#endif

/** @file lincity-ng/readdir.hpp */

