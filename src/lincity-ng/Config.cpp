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

    soundVolume = 128;

    lincityHome = "./data";
    
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
 * Read Integer Value from char-Array.
 * use defaultValue on Errors or if Value is not in given Interval.
 */
int Config::parseInt( const char* value, int defaultValue, int minValue, int maxValue ) {
    int tmp; 
    if(sscanf(value, "%i", &tmp) != 1) 
    {
        std::cerr << "Config::parseInt# Error parsing integer value '" << value << "'.\n";
        tmp = defaultValue;
    }
    if( ( tmp >= minValue ) && ( tmp <= maxValue ) ) {
        return tmp;
    } else {
        std::cerr << "Config::parseInt# Value '" << value << "' not in ";
        std::cerr << minValue << ".." << maxValue << "\n";
        return defaultValue;
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
                        videoX = parseInt( value, 800, 640 );
                    }
                    else if(strcmp(name, "y") == 0 ) 
                    {
                        videoY = parseInt( value, 600, 480 );
                    }
                }//while(iter.next()) 
            }//if element == "video" 
            else if ( element == "audio" ) {
                XmlReader::AttributeIterator iter(reader);
                while(iter.next()) 
                {
                    const char* name = (const char*) iter.getName();
                    const char* value = (const char*) iter.getValue();
                    if( strcmp(name, "soundVolume" ) == 0 )
                    {
                        soundVolume = parseInt( value, 128, 0, 128 );
                    }
                }//while(iter.next())                 
            }//if element == "audio" 
            else if ( element == "env" ) {
                XmlReader::AttributeIterator iter(reader);
                while(iter.next()) 
                {
                    const char* name = (const char*) iter.getName();
                    const char* value = (const char*) iter.getValue();
                    if( strcmp(name, "LINCITY_HOME" ) == 0 )
                    {
                        lincityHome = value;
                    }
                }//while(iter.next())                 
            }//if element == "env" 
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

