#include <error.h>

#ifdef DEBUG
  #define HANDLE_ERRNO(COMMAND, CONDITION, STATUS, FORMAT...) do { \
    errno = 0; \
    COMMAND; \
    if(CONDITION) \
      error_at_line(STATUS, errno, __FILE__, __LINE__, FORMAT); \
    errno = 0; \
  } while(0)
#else
  #define HANDLE_ERRNO(COMMAND, CONDITION, STATUS, FORMAT...) do { \
    errno = 0; \
    COMMAND; \
    if(CONDITION) \
      error(STATUS, errno, FORMAT); \
    errno = 0; \
  } while(0)
#endif
