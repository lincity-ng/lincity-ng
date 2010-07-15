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
/** simple wrapper around libxml2 xmlreader interface */

#ifndef __XMLREADER_HPP__
#define __XMLREADER_HPP__

#include <libxml/xmlreader.h>
#include <stdexcept>
#include <sstream>
#include <iostream>

class XmlReader
{
public:
    XmlReader(const std::string& filename);
    ~XmlReader();

    int getDepth()
    {
        return xmlTextReaderDepth(reader);
    }

    int getNodeType()
    {
        return xmlTextReaderNodeType(reader);
    }

    bool hasValue()
    {
        return xmlTextReaderHasValue(reader);
    }

    bool isEmptyElement()
    {
        return xmlTextReaderIsEmptyElement(reader);
    }

    const xmlChar* getName()
    {
        return xmlTextReaderConstName(reader);
    }

    const xmlChar* getValue()
    {
        return xmlTextReaderConstValue(reader);
    }

    void nextNode()
    {
        xmlTextReaderNext(reader);
    }

    class AttributeIterator 
    {
    public:
        AttributeIterator(XmlReader& reader)
        {
            this->reader = reader.reader;
            first = true;
            last = false;
        }

        ~AttributeIterator()
        {
            if(!last)
                xmlTextReaderMoveToElement(reader);
        }

        bool next()
        {
            int res;
            if(first) {
                res = xmlTextReaderMoveToFirstAttribute(reader);
            } else {
                res = xmlTextReaderMoveToNextAttribute(reader);
                
            }

            if(res < 0)
                throw std::runtime_error("parse error.");
            if(res == 0) {
                last = true;
                xmlTextReaderMoveToElement(reader);               
                return false;
            }
            first = false;

            return true;
        }
        const xmlChar* getName()
        {
            return xmlTextReaderConstName(reader);
        }
        const xmlChar* getValue()
        {
            return xmlTextReaderConstValue(reader);
        }

    private:
        bool first;
        bool last;
        xmlTextReaderPtr reader;
    };

    bool read()
    {
        int ret = xmlTextReaderRead(reader);
        if(ret < 0) {
            std::stringstream msg;
            /* This only works in libxml2.6.17 it seems... I have to upgrade
             * sometime...
             * msg << "Parse error at line " << xmlTextReaderGetParserLineNumber()...
             */
            msg << "Parser error";
            throw std::runtime_error(msg.str());
        }

        // Usefull for debug sometimes...
#if 0
        for(int i = 0; i < getDepth(); ++i)
            std::cout << " ";
        std::cout << "T: " << getNodeType();
        if(getName())
            std::cout << " N:" << (const char*) getName();
        if(getValue())
            std::cout << " - " << (const char*) getValue();
        std::cout << "\n";
#endif
        return ret == 1;
    }
       
private:
    xmlTextReaderPtr reader;
};

#endif


/** @file tools/xmlgettext/XmlReader.hpp */

