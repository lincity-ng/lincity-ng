
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
