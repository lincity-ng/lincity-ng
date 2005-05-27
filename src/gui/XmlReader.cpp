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
#include <config.h>

#include "XmlReader.hpp"

#include <physfs.h>

XmlReader::XmlReader(const std::string& filename)
{
    PHYSFS_file* file = PHYSFS_openRead(filename.c_str());
    if(file == 0) {
        std::stringstream msg;
        msg << "Couldn't open file '" << filename << "': " 
            << PHYSFS_getLastError();
        throw std::runtime_error(msg.str());
    }
    
    reader = xmlReaderForIO(readCallback, closeCallback, file, 
            0, 0, XML_PARSE_NONET);
    if(reader == 0) {
        PHYSFS_close(file);
        std::stringstream msg;
        msg << "Couldn't parse file '" << filename << "'";
        throw std::runtime_error(msg.str());
    }
    while(read() && getNodeType() != XML_READER_TYPE_ELEMENT)
        ;
}

XmlReader::~XmlReader()
{
    xmlFreeTextReader(reader);
}

int
XmlReader::readCallback(void* context, char* buffer, int len)
{
    PHYSFS_file* file = (PHYSFS_file*) context;
    PHYSFS_sint64 result = PHYSFS_read(file, buffer, 1, len);
    if(result < 0) {
        std::cerr << "Read error: " << PHYSFS_getLastError() << "\n";
    }
    return (int) result;
}

int
XmlReader::closeCallback(void* context)
{
    PHYSFS_file* file = (PHYSFS_file*) context; 
    int res = PHYSFS_close(file);
    if(res < 0) {
        std::cerr << "Close error: " << PHYSFS_getLastError() << "\n";
    }
    return res;
}

