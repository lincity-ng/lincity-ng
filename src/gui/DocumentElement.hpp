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
 * @file DocumentElement.hpp
 */

#ifndef __DOCUMENTELEMENT_HPP__
#define __DOCUMENTELEMENT_HPP__

#include <libxml++/parsers/textreader.h>  // for TextReader

class Style;

/**
 * @class DocumentElement
 * @brief Interface for a component that can be part of a text component.
 *
 * This interface gives access to the defined formatting style.
 */
class DocumentElement
{
public:
    virtual ~DocumentElement()
    {}

    virtual const Style& getStyle() const = 0;
    virtual void parse(xmlpp::TextReader& reader, const Style& parentstyle) = 0;
};

#endif


/** @file gui/DocumentElement.hpp */
