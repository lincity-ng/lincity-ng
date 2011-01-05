#ifndef __GETTEXT_H__
#define __GETTEXT_H__

#include "tinygettext/tinygettext.hpp"

extern tinygettext::DictionaryManager* dictionaryManager;

#ifdef NEED_GETTEXT_CHARHACK
static inline char* _(const char* message)
{
    return const_cast<char*>
        (dictionaryManager->get_dictionary().translate(message));
}
#else
static inline const char* _(const char* message)
{
    return dictionaryManager->get_dictionary().translate(message);
}
#endif

#define N_(s)      s

#endif

/** @file tinygettext/gettext.hpp */

