/**
 *  Config Handler for Lincity-NG.
 *   
 *  March 2005, Wolfgang Becker <uafr@gmx.de>
 *
 */
#include <config.h>

#include "Config.hpp"
#include "gui/XmlReader.hpp"

#include <assert.h>
#include <iostream>

Config* configPtr = 0;

Config *getConfig()
{
  return configPtr;
}

Config::Config()
{
    assert( configPtr == 0);
    configPtr = this;
    //Default Values
    useOpenGL = false;
    videoX = 800; 
    videoY = 600;
    
    //First we load the global File which should contain
    //sane defaults for the local system.
    load( "lincityconfig.xml" );
    //Then load another file where to overwrite some values.
    load( "userconfig.xml" );
}

Config::~Config()
{
    if( configPtr == this )
    {
        configPtr = 0;
    }
}

/*
 * Load configuration from File.
 */
void Config::load( const std::string& filename ){
    std::cout << "Load Config from " << filename <<"\n";
    XmlReader reader( filename );
    while( reader.read() ) {
        if( reader.getNodeType() == XML_READER_TYPE_ELEMENT) 
        {
            const std::string& element = (const char*) reader.getName();
            if( element == "video" ) {
                XmlReader::AttributeIterator iter(reader);
                while(iter.next()) 
                {
                    const char* name = (const char*) iter.getName();
                    const char* value = (const char*) iter.getValue();
                    if( strcmp( name, "useOpenGL" ) == 0 ) 
                    {
                        if( strcmp( value, "true" ) == 0 ) 
                        {
                           useOpenGL  = true;
                        }
                    }
                    else if( strcmp(name, "x" ) == 0 )
                    {
                        if(sscanf(value, "%i", &videoX) != 1) 
                        {
                            std::cerr << "Config::load# Error parsing integer value '" << value << "' in x attribute.\n";
                            videoX = 800;
                        }
                    }
                    else if(strcmp(name, "y") == 0 ) 
                    {
                        if(sscanf(value, "%i", &videoY) != 1) 
                        {
                            std::cerr << "Config::load# Error parsing integer value '" << value << "' in y attribute.\n";
                            videoY = 600;
                        }
                    }
                }//while(iter.next()) 
            }//if element == "video" 
            else {
                std::cerr << "Config::load# Unknown element '" << element << "' in lincityconfig.xml.\n";
            }
        }//if( reader.getNodeType() == XML_READER_TYPE_ELEMENT) 
    }//while( reader.read() ) 
}

/*
 * Save configuration to File.
 * TODO: make it work.
 */
void Config::save(){
    std::cerr << "Config::save() not implemented!\n";
}

