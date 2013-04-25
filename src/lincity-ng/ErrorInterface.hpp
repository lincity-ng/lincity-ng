#ifndef __ErrorInterface_h__
#define __ErrorInterface_h__

#define DO_ERROR(MESSAGE) \
    std::cerr << __FILE__ << "," << __LINE__ << ": " << MESSAGE << std::endl

void do_error (const char *s);
void HandleError (const char *s, int i);

#endif

/** @file lincity-ng/ErrorInterface.hpp */

