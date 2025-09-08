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

#ifndef __LINCITYNG_UTIL_XMLUTIL_HPP__
#define __LINCITYNG_UTIL_XMLUTIL_HPP__

#include <libxml++/ustring.h>  // for ustring
#include <string>

namespace xmlpp {
class TextReader;
}  // namespace xmlpp

typedef const unsigned char *xmlStr;
typedef unsigned char *xmlMStr;

extern void unexpectedXmlElement(xmlpp::TextReader& reader);
extern void missingXmlElement(xmlpp::TextReader& reader,
  const xmlpp::ustring& name);
extern void unexpectedXmlAttribute(xmlpp::TextReader& reader);
extern void missingXmlAttribute(xmlpp::TextReader& reader,
  const xmlpp::ustring& name);


// Helps extend the lifetime of a xmlStr beyond the return of xmlFormat
class xmlStrF {
public:
  explicit xmlStrF(xmlpp::ustring str) : str(str) { }

  operator xmlStr() const { return (xmlStr)str.c_str(); }
  operator xmlpp::ustring() const { return str; }

private:
  const xmlpp::ustring str;
};

template<typename X>
extern const xmlStrF xmlFormat(const X x);

template<typename X>
extern const xmlStrF xmlFormatHex(const X x);

template<typename X>
extern X xmlParse(const xmlpp::ustring& s);

#endif // __LINCITYNG_UTIL_XMLUTIL_HPP__
