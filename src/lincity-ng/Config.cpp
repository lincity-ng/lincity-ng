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
    if(configPtr == 0)
        configPtr = new Config();

    return configPtr;
}

Config::Config()
{
    assert(configPtr == 0);
    
    //Default Values
    useOpenGL = false;
    useFullScreen = true;
    videoX = 800; 
    videoY = 600;

    soundVolume = 100;
    musicVolume = 50;
    soundEnabled = true;
    musicEnabled = true;

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
                    if( strcmp( name, "useOpenGL" ) == 0 ) {
                        useOpenGL  = parseBool(value, false);
                    } else if( strcmp(name, "x" ) == 0 ) {
                        videoX = parseInt( value, 800, 640 );
                    } else if(strcmp(name, "y") == 0 ) {
                        videoY = parseInt( value, 600, 480 );
                    } else if(strcmp(name, "fullscreen") == 0) {
                        useFullScreen = parseBool(value, false);
                    } else {
                        std::cerr << "Config::load# Unknown attribute '" << name;
                        std::cerr << "' in element '" << element << "' from " << filename << ".\n";
                    }
                }
            } else if ( element == "audio" ) {
                XmlReader::AttributeIterator iter(reader);
                while(iter.next()) 
                {
                    const char* name = (const char*) iter.getName();
                    const char* value = (const char*) iter.getValue();
                    if(strcmp(name, "soundVolume" ) == 0) {
                        soundVolume = parseInt(value, 100, 0, 100);
                    } else if(strcmp(name, "musicVolume") == 0) {
                        musicVolume = parseInt(value, 100, 0, 100);
                    } else if(strcmp(name, "soundEnabled") == 0) {
                        soundEnabled = parseBool(value, true);
                    } else if(strcmp(name, "musicEnabled") == 0) {
                        musicEnabled = parseBool(value, true);
                    } else {
                        std::cerr << "Config::load# Unknown attribute '" << name;
                        std::cerr << "' in element '" << element << "' from " << filename << ".\n";
                    }
                }
            } else if ( element == "env" ) {
                XmlReader::AttributeIterator iter(reader);
                while(iter.next()) 
                {
                    const char* name = (const char*) iter.getName();
                    const char* value = (const char*) iter.getValue();
                    if( strcmp(name, "LINCITY_HOME" ) == 0 )
                    {
                        lincityHome = value;
                    } else {
                        std::cerr << "Config::load# Unknown attribute '" << name;
                        std::cerr << "' in element '" << element << "' from " << filename << ".\n";
                    }
                }
            } else {
                std::cerr << "Config::load# Unknown element '" << element << "' in "<< filename << ".\n";
            }
        }
    }
}

bool
Config::parseBool(const char* value, bool defaultValue)
{
    if(strcmp(value, "no") == 0 || strcmp(value, "off") == 0
            || strcmp(value, "false") == 0 || strcmp(value, "NO") == 0
            || strcmp(value, "OFF") == 0 || strcmp(value, "FALSE") == 0) {
        return false;
    }
    if(strcmp(value, "yes") == 0 || strcmp(value, "on") == 0
            || strcmp(value, "true") == 0 || strcmp(value, "YES") == 0
            || strcmp(value, "ON") == 0 || strcmp(value, "TRUE") == 0) {
        return true;
    }

    std::cerr << "Couldn't parse boolean value '" << value << "'.\n";
    return defaultValue;
}

/*
 * Save configuration to File.
 * TODO: make it work.
 */
void
Config::save(){
    std::cerr << "Config::save() not implemented!\n";
}

