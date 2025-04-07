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
 * @file gui/XmlReader.cpp
 * @brief Implement a simple XML file reader (simple wrapper around libxml2 xmlreader interface).
 * @author Matthias Braun.
 */

#include "./XmlReader.hpp"

#include <libxml/parser.h>  // for XML_PARSE_NONET
#include <cstddef>         // for NULL
#include <filesystem>       // for operator<<, path

/**
 * Class constructor: parses a given XML file and parse it with libxml2.
 *
 * @param filename XML file to parse.
 */
XmlReader::XmlReader(const std::filesystem::path& filename) {
    reader = xmlReaderForFile(filename.string().c_str(), NULL, XML_PARSE_NONET);
    if(reader == 0) {
        std::stringstream msg;
        msg << "Couldn't parse file '" << filename << "'";
        throw std::runtime_error(msg.str());
    }
    while(read() && getNodeType() != XML_READER_TYPE_ELEMENT)
        ;
}

/**
 * Class destructor: destroy the object created when parsing a XML file.
 */
XmlReader::~XmlReader()
{
    xmlFreeTextReader(reader);
}


/** @file gui/XmlReader.cpp */
