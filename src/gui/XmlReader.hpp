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

    std::string getAttribute(const char* name)
    {
        xmlChar* res = xmlTextReaderGetAttribute(reader, (const xmlChar*) name);
        if(res == 0) {
            std::stringstream msg;
            msg << "Couldn't find attribute '" << name << "'.";
            throw std::runtime_error(msg.str());
        }
        std::string result( (const char*) res);
        xmlFree(res);

        return result;
    }

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

