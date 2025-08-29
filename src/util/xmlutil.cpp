/* ---------------------------------------------------------------------- *
 * src/util/xmlutil.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2024-2025 David Bears <dbear4q@gmail.com>
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

#include "xmlutil.hpp"

#include <fmt/base.h>                     // for println
#include <fmt/format.h>                   // for format
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <cassert>                        // for assert
#include <charconv>                       // for from_chars_result, from_chars
#include <cstdio>                         // for stderr
#include <filesystem>                     // for path
#include <stdexcept>                      // for runtime_error
#include <string>                         // for basic_string, operator==
#include <system_error>                   // for system_error, make_error_code
#include <fmt/std.h> // IWYU pragma: keep
#include <type_traits>
#include <typeinfo>

void unexpectedXmlElement(xmlpp::TextReader& reader) {
  fmt::println(stderr, "warning: skipping unexpected element <{}>",
    reader.get_name());
  assert(false);
}

void missingXmlElement(xmlpp::TextReader& reader, const std::string& name) {
  throw std::runtime_error(fmt::format("missing XML element <{}> in <{}>",
    name, reader.get_name()));
}

void unexpectedXmlAttribute(xmlpp::TextReader& reader) {
  fmt::println(stderr, "warning: skipping unexpected attribute {}={:?} in <{}>",
    reader.get_name(), reader.get_value(),
    (reader.move_to_element(), reader.get_name())
  );
  assert(false);
}

void missingXmlAttribute(xmlpp::TextReader& reader, const std::string& name) {
  reader.move_to_element();
  throw std::runtime_error(fmt::format("missing XML attribute {:?} in <{}>",
    name, reader.get_name()));
}


template<typename X, bool hex = false>
static const xmlStrF xmlFormatNumber(const X x) {
  xmlpp::ustring str(16, '\0');
  std::to_chars_result r;
  while(true) {
    char *cs = str.data();
    char *ce = cs + str.size();
    if constexpr (hex)
      cs += 2; // space for '0x' prefix
    if constexpr (std::is_integral_v<X>) {
      r = std::to_chars(cs, ce, x, hex ? 16 : 10);
    }
    else {
      r = std::to_chars(cs, ce, x,
        hex ? std::chars_format::hex : std::chars_format::general);
    }
    if(r.ec == std::errc{})
      break;
    else if(r.ec == std::errc::value_too_large && str.size() < str.max_size()) {
      auto newsize = str.size() * 2;
      if(newsize > str.max_size() || newsize < str.size())
        newsize = str.max_size();
      str.resize(newsize);
    }
    else {
      throw std::system_error(std::make_error_code(r.ec), fmt::format(
        "failed to format XML value of type {:?}: {:?}", typeid(X).name(), x));
    }
  }
  str.resize(r.ptr - str.data());
  if constexpr (hex) {
    if(str[2] == '-')
      str.replace(0, 3, "-0x");
    else
      str.replace(0, 2, "0x");
  }

  return xmlStrF(str);
}

template<typename X>
const xmlStrF xmlFormat(const X x) {
  return xmlFormatNumber<X, false>(x);
}

template<typename X>
const xmlStrF xmlFormatHex(const X x) {
  return xmlFormatNumber<X, true>(x);
}

template<>
const xmlStrF xmlFormat<bool>(const bool x) {
  return xmlStrF(x ? "true" : "false");
}

template<>
const xmlStrF xmlFormat<std::string>(const std::string x) {
  return xmlStrF(x);
}

template<>
const xmlStrF xmlFormat<std::filesystem::path>(const std::filesystem::path x) {
  return xmlStrF(x.generic_string());
}

template<typename X>
X xmlParse(const xmlpp::ustring& s) {
  X x;
  std::string str(s);
  char *cs = str.data();
  char *ce = cs + str.size();

  // cut out the '0x' prefix
  bool hex = false;
  if(*cs == '-' || *cs == '+') cs++;
  if(*cs == '0' && (cs[1] == 'x' || cs[1] == 'X')) {
    cs += 2;
    hex = true;
  }
  if(str[0] == '-') {
    cs--;
    *cs = '-';
  }

  std::from_chars_result r;
  if constexpr (std::is_integral_v<X>) {
    r = std::from_chars(cs, ce, x, hex ? 16 : 10);
  }
  else {
    r = std::from_chars(cs, ce, x,
      hex ? std::chars_format::hex : std::chars_format::general);
  }

  if(r.ec != std::errc{})
    throw std::system_error(std::make_error_code(r.ec), fmt::format(
      "failed to parse XML value of type {:?}: {:?}", typeid(X).name(), s));
  if(r.ptr != ce) {
    fmt::println(stderr,
      "error: detected extra characters after number: {:?}",
      s
    );
  }
  assert(r.ptr == ce);
  return x;
}

template<>
bool xmlParse<bool>(const xmlpp::ustring& s) {
#ifdef DEBUG
  if(s == "0" || s == "1")
    fmt::println(stderr, "warning: casting int to bool in xmlParse<bool>");
#endif

  if(s == "1"
    || s == "true" || s == "TRUE"
    || s == "yes"  || s == "YES"
    || s == "y"    || s == "Y"
  )
    return true;
  else if(s == "0"
    || s == "false" || s == "FALSE"
    || s == "no"    || s == "NO"
    || s == "n"     || s == "N"
  )
    return false;
  else
    throw std::runtime_error(fmt::format("failed to parse bool {:?}", s).c_str());
}

template<>
std::string xmlParse<std::string>(const xmlpp::ustring& s) {
  return s;
}

template<>
std::filesystem::path xmlParse<std::filesystem::path>(const xmlpp::ustring& s) {
  return std::filesystem::path(s);
}

template const xmlStrF xmlFormat<int>(const int);
template const xmlStrF xmlFormat<unsigned int>(const unsigned int);
template const xmlStrF xmlFormat<unsigned short>(const unsigned short);
template const xmlStrF xmlFormat<std::size_t>(const std::size_t);
template const xmlStrF xmlFormat<float>(const float);
template const xmlStrF xmlFormat<bool>(const bool);
template const xmlStrF xmlFormat<std::string>(const std::string);
template const xmlStrF
xmlFormat<std::filesystem::path>(const std::filesystem::path);

template const xmlStrF xmlFormatHex<int>(const int);
template const xmlStrF xmlFormatHex<unsigned int>(const unsigned int);
template const xmlStrF xmlFormatHex<unsigned short>(const unsigned short);
template const xmlStrF xmlFormatHex<std::size_t>(const std::size_t);
template const xmlStrF xmlFormatHex<float>(const float);

template int xmlParse<int>(const xmlpp::ustring&);
template unsigned int xmlParse<unsigned int>(const xmlpp::ustring&);
template unsigned short xmlParse<unsigned short>(const xmlpp::ustring&);
template std::size_t xmlParse<std::size_t>(const xmlpp::ustring&);
template float xmlParse<float>(const xmlpp::ustring&);
template bool xmlParse<bool>(const xmlpp::ustring&);
template std::string xmlParse<std::string>(const xmlpp::ustring&);
template std::filesystem::path
xmlParse<std::filesystem::path>(const xmlpp::ustring&);
