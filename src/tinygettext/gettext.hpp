/* ---------------------------------------------------------------------- *
 * src/tinygettext/gettext.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2025      David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#ifndef __LINCITYNG_TINYGETTEXT_GETTEXT_HPP__
#define __LINCITYNG_TINYGETTEXT_GETTEXT_HPP__

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

static inline std::string _(const std::string& message) {
  return std::string(_(message.c_str()));
}

#define N_(s)      s

#endif // __LINCITYNG_TINYGETTEXT_GETTEXT_HPP__

/** @file tinygettext/gettext.hpp */
