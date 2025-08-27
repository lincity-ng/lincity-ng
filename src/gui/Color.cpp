/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>

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

/**
 * @author Matthias Braun
 * @file Color.cpp
 */

#include "Color.hpp"

#include <fmt/format.h>  // for format
#include <charconv>      // for from_chars, from_chars_result
#include <stdexcept>     // for runtime_error
#include <string>        // for operator==, basic_string, allocator, char_tr...
#include <system_error>  // for errc

void
Color::parse(xmlpp::ustring value) {
  if(value.substr(0,1) == "#") {
    unsigned int red, green, blue, alpha = 0xff;
    const char *p = value.data();
    if(value.size() != 7 && value.size() != 9) goto fail;
    if(std::from_chars(p + 1, p + 3, red, 16).ec != std::errc{}) goto fail;
    if(std::from_chars(p + 3, p + 5, green, 16).ec != std::errc{}) goto fail;
    if(std::from_chars(p + 5, p + 7, blue, 16).ec != std::errc{}) goto fail;
    if(value.size() >= 9
      && std::from_chars(p + 7, p + 9, alpha, 16).ec != std::errc{}
    ) {
      fail:
      throw std::runtime_error(fmt::format(
        "failed to parse hex color {:?}", value));
    }

    r = red;
    g = green;
    b = blue;
    a = alpha;
  } else if(value == "black") {
    r = 0;
    g = 0;
    b = 0;
    a = 0xff;
  } else if(value == "green") {
    r = 0;
    g = 0x80;
    b = 0;
    a = 0xff;
  } else if(value == "white") {
    r = 0xff;
    g = 0xff;
    b = 0xff;
    a = 0xff;
  } else if(value == "blue") {
    r = 0x00;
    g = 0x00;
    b = 0xff;
    a = 0xff;
  } else if(value == "yellow") {
    r = 0xff;
    g = 0xff;
    b = 0;
    a = 0xff;
  } else if(value == "red") {
    r = 0xff;
    g = 0;
    b = 0;
    a = 0xff;
  } else if(value == "brown") {
    r = 165;
    g = 42;
    b = 42;
    a = 0xff;
  } else if(value == "orange") {
    r = 255;
    g = 165;
    b = 0;
  } else if(value == "gray") {
		r = 127;
		g = 127;
		b = 127;
  } else {
    throw std::runtime_error(fmt::format("unknown color {:?}", value));
  }
}

/** @file gui/Color.cpp */
