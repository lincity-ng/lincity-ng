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
    int soundVolume;
    std::string lincityHome;
private:
    void load( const std::string& filename );
    void save();
    int parseInt( const char* value, const int defaultValue, const int minValue = MININT, const int maxValue = MAXINT );
};

Config* getConfig();

#endif
