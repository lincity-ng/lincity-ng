/*
Copyright (C) 2024 David Bears <dbear4q@gmail.com>

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

// glibc already provides these functions
#ifndef UNIX

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void error_at_line(int status, int errnum, const char *fname,
  unsigned int lineno, const char *format, ...
) {
  va_list va;
  va_start(va, format);
  if(fname)
    fprintf(stderr, "lincity-ng:%s:%u: ", fname, lineno);
  else
    fprintf(stderr, "lincity-ng: ");
  if(format) {
    vfprintf(stderr, format, va) ;
    fprintf(stderr, ": ");
  }
  va_end(va);
  fprintf(stderr, "%s\n", strerror(errno));
  if(status) exit(status);
}

#endif
