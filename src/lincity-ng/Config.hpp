#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <string>
#include <values.h>

class Config 
{
public:
    Config();
    ~Config();
    
    bool useOpenGL;
    int videoX, videoY;
    
    // sound volume 0..100 (0=silent)
    int soundVolume;
    // music volume 0..100
    int musicVolume;
    bool soundEnabled;
    bool musicEnabled;
    std::string lincityHome;

private:
    void load( const std::string& filename );
    void save();
    int parseInt(const char* value, int defaultValue, int minValue = MININT, 
            int maxValue = MAXINT );
    bool parseBool(const char* value, bool defaultvalue);
};

Config* getConfig();

#endif
