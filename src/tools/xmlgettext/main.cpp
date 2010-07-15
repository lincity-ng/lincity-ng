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

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <string.h>

typedef std::map<std::string, std::vector<std::string> > Texts;
Texts texts;

void addTranslatableText(const std::string& text, const std::string& place)
{
    Texts::iterator i = texts.find(text);
    if(i != texts.end()) {
        i->second.push_back(place);
    } else {
        std::vector<std::string> places;
        places.push_back(place);
        texts.insert(std::make_pair(text, places));
    }
}

void parseFile(const std::string& filename)
{
    XmlReader reader(filename);
    
    bool translatable = false;
    std::string place;
    std::string text;
    while(reader.read()) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            if(translatable) {
                std::cerr 
                    << "Elements inside translatable element not supported!\n";
            }
            translatable = false;
            XmlReader::AttributeIterator iter(reader);
            while(iter.next()) {
                const char* attribute = (const char*) iter.getName();
                const char* value = (const char*) iter.getValue();
                
                if(strcmp(attribute, "translatable") == 0) {
                    if(strcmp(value, "yes") == 0) {
                        translatable = true;
                        place = filename;
                        text = "";
                    } else {
                        std::cerr << "Unknown value for translatable: '"
                            << value << "'\n";
                    }
                }
            }
        } else if(reader.getNodeType() == XML_READER_TYPE_TEXT) {
            if(translatable) {
                const char* p = (const char*) reader.getValue();
                // skip trailing spaces...
                while(*p != 0 && isspace(static_cast<unsigned char>(*p)))
                    ++p;

                bool lastspace = false;
                for( ; *p != 0; ++p) {
                    if(isspace(static_cast<unsigned char>(*p))) {
                        if(!lastspace) {
                            lastspace = true;
                            text += ' ';
                        }
                    } else {
                        lastspace = false;
                        text += *p;
                    }
                }
            }
        } else if(reader.getNodeType() == XML_READER_TYPE_END_ELEMENT) {
            if(translatable) {
                addTranslatableText(text, place);
            }
            translatable = false;
        }
    }
}

int main(int argc, char** argv)
{
    if(argc <= 2) {
        std::cerr << "Usage:\n"
            << "xmlgettext output.pot file1.xml file2.xml ...\n\n";
        return 1;
    }

    for(int i = 2; i < argc; ++i) {
        parseFile(argv[i]);
    }

    // OUTPUT file
    std::ofstream out(argv[1]);
    if(!out.good()) {
        std::cerr << "Couldn't open output file '" << argv[1] << "'.\n";
        return 1;
    }
    char buffer[128];
    time_t curtime = time(0);
    const char* strfformat = "%c"; // to avoid gcc warning...
    strftime(buffer, sizeof(buffer), strfformat, localtime(&curtime));
    out <<
"# SOME DESCRIPTIVE TITLE.\n"
"# Copyright (C) YEAR THE PACKAGE'S COPYRIGHT HOLDER\n"
"# This file is distributed under the same license as the PACKAGE package.\n"
"# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.\n"
"#\n"
"#, fuzzy\n"
"msgid \"\"\n"
"msgstr \"\"\n"
"\"Project-Id-Version: PACKAGE VERSION\\n\"\n"
"\"Report-Msgid-Bugs-To: \\n\"\n"
"\"POT-Creation-Date: " << buffer << "\\n\"\n"
"\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n\"\n"
"\"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n\"\n"
"\"Language-Team: LANGUAGE <LL@li.org>\\n\"\n"
"\"MIME-Version: 1.0\\n\"\n"
"\"Content-Type: text/plain; charset=CHARSET\\n\"\n"
"\"Content-Transfer-Encoding: 8bit\\n\"\n";

    for(Texts::iterator i = texts.begin(); i != texts.end(); ++i) {
    	if(!i->first.empty()){ //no need to translate empty Strings
            out << "\n";
            out << "#: ";
            for(std::vector<std::string>::iterator p = i->second.begin();
                p != i->second.end(); ++p) {
                out << *p << " ";
            }
            out << "\n";
            out << "msgid \"" << i->first << "\"\n";
            out << "msgstr \"\"\n";
        }
    }
}

/** @file tools/xmlgettext/main.cpp */

