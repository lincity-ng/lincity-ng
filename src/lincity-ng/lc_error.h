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

#ifndef __LC_ERROR_H__
#define __LC_ERROR_H__

#include <errno.h>
#include "config.h"

#ifdef __cplusplus
#define EXTERN_C extern "C"
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif


#ifdef DEBUG
#define lc_error(STATUS, ERRNO, FORMAT...) \
  error_at_line(STATUS, ERRNO, __FILE__, __LINE__, FORMAT)
#else
#define lc_error(STATUS, ERRNO, FORMAT...) \
  error(STATUS, ERRNO, FORMAT)
#endif

#ifdef UNIXX
#include <error.h>
#else
#define error(STATUS, ERRNO, FORMAT...) \
  error_at_line(STATUS, ERRNO, NULL, 0, FORMAT)
EXTERN_C void error_at_line(int status, int errnum, const char *fname,
  unsigned int lineno, const char *format, ...);
#endif

#define HANDLE_ERRNO(COMMAND, CONDITION, STATUS, FORMAT...) do { \
  errno = 0; \
  COMMAND; \
  if(CONDITION) \
    lc_error(STATUS, errno, FORMAT); \
  errno = 0; \
} while(0)

#endif // __LC_ERROR_H__
