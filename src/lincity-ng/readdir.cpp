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
#ifdef _MSC_VER
#include <config.h>

#include "readdir.hpp"

#include <stdio.h>

/* This is an MSVC implementation of the posix opendir/readdir/closedir
 * interfaces
 */

DIR *opendir(const char *name)
{
    DIR *dir = (DIR *)malloc(sizeof(DIR));
    if (!dir) {
        return NULL;
    }
    dir->first = 1;
    snprintf(dir->path, sizeof(dir->path), "%s\\*.*", name);
    dir->path[sizeof(dir->path) - 1] = '\0';
    if ((dir->handle = _findfirst(dir->path, &dir->info)) == -1) {
        free(dir);
        return NULL;
    }
    return dir;
}

struct dirent *readdir(DIR *dir)
{
    if (dir->first) {
        dir->first = 0;
    } else if (_findnext(dir->handle, &dir->info) == -1) {
        return NULL;
    }
    dir->entry.d_reclen = (unsigned short)strlen(dir->info.name);
    strncpy(dir->entry.d_name, dir->info.name, sizeof(dir->entry.d_name));
    dir->entry.d_name[sizeof(dir->entry.d_name) - 1] = '\0';
    return &dir->entry;
}

void rewinddir(DIR *dir)
{
    _findclose(dir->handle);
    dir->first = 1;
    dir->handle = _findfirst(dir->path, &dir->info);
}

int closedir(DIR *dir)
{
    _findclose(dir->handle);
    free(dir);
    return 0;
}

#endif


/** @file lincity-ng/readdir.cpp */

