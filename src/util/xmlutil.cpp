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
#include <cmath>                          // for fabs, isfinite, signbit
#include <cstdio>                         // for stderr
#include <filesystem>                     // for path
#include <stdexcept>                      // for runtime_error
#include <string>                         // for basic_string, operator==
#include <system_error>                   // for system_error, make_error_code
#include <fmt/std.h> // IWYU pragma: keep

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
  fmt::println(stderr, "warning: skipping unexpected attribute {:?}={:?}",
    reader.get_name(), reader.get_value());
  assert(false);
}

void missingXmlAttribute(xmlpp::TextReader& reader, const std::string& name) {
  reader.move_to_element();
  throw std::runtime_error(fmt::format("missing XML attribute {:?} in <{}>",
    name, reader.get_name()));
}

template<typename X>
const xmlStr xmlFormat(const X x) {
  return (xmlStr)fmt::format("{}", x).c_str();
}

template<typename X>
const xmlStr xmlFormatHex(const X x) {
  return (xmlStr)(std::isfinite(x) ?
    // annoyingly, std::to_chars ommits the "0x"
    fmt::format("{:s}0x{:a}",
      std::signbit(x) ? "-" : "",
      std::fabs(x)
    ) :
    fmt::format("{}", x)
  ).c_str();
}

template<typename X>
X xmlParse(const xmlpp::ustring& s) {
  X x;
  const char *cs = s.data();
  const char *ce = cs + s.size();
  std::from_chars_result r = std::from_chars(cs, ce, x);
  if(r.ec != std::errc{})
    throw std::system_error(std::make_error_code(r.ec));
  assert(r.ptr == ce);
  if(r.ptr != ce) {
    fmt::println(stderr,
      "error: detected extra characters after number: {:?}",
      s
    );
  }
  return x;
}

template<>
bool xmlParse<bool>(const xmlpp::ustring& s) {
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

template const xmlStr xmlFormat<int>(const int);
template const xmlStr xmlFormat<unsigned int>(const unsigned int);
template const xmlStr xmlFormat<unsigned short>(const unsigned short);
template const xmlStr xmlFormat<std::size_t>(const std::size_t);
template const xmlStr xmlFormat<float>(const float);
template const xmlStr xmlFormat<bool>(const bool);
template const xmlStr xmlFormat<std::string>(const std::string);
template const xmlStr
xmlFormat<std::filesystem::path>(const std::filesystem::path);

template const xmlStr xmlFormatHex<int>(const int);
template const xmlStr xmlFormatHex<unsigned int>(const unsigned int);
template const xmlStr xmlFormatHex<unsigned short>(const unsigned short);
template const xmlStr xmlFormatHex<std::size_t>(const std::size_t);
template const xmlStr xmlFormatHex<float>(const float);

template int xmlParse<int>(const xmlpp::ustring&);
template unsigned int xmlParse<unsigned int>(const xmlpp::ustring&);
template unsigned short xmlParse<unsigned short>(const xmlpp::ustring&);
template std::size_t xmlParse<std::size_t>(const xmlpp::ustring&);
template float xmlParse<float>(const xmlpp::ustring&);
template bool xmlParse<bool>(const xmlpp::ustring&);
template std::string xmlParse<std::string>(const xmlpp::ustring&);
template std::filesystem::path
xmlParse<std::filesystem::path>(const xmlpp::ustring&);
