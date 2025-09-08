/* ---------------------------------------------------------------------- *
 * src/lincity-ng/lc_gettext.hpp
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

#ifndef __LINCITYNG_LINCITYNG_LCGETTEXT_HPP__
#define __LINCITYNG_LINCITYNG_LCGETTEXT_HPP__

#ifndef ENABLE_NLS
#define ENABLE_NLS false
#endif

#include <gettext.h>
#include <string>

#define N_(MSG) gettext_noop(MSG)

static const char *
_(const char *msg) {
  return gettext(msg);
}

static const std::string
_(const std::string& msg) {
  return gettext(msg.c_str());
}

#endif // __LINCITYNG_LINCITYNG_LCGETTEXT_HPP__
