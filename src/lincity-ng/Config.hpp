#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <string>
#include <limits.h>
#include <float.h>

class Config 
{
public:
    Config();
    ~Config();
    
    bool useOpenGL;
    bool useFullScreen;
    int videoX, videoY;
    int monthgraphW, monthgraphH;
    
    // sound volume 0..100 (0=silent)
    int soundVolume;
    // music volume 0..100
    int musicVolume;
    bool soundEnabled;
    bool musicEnabled;
    //std::string lincityHome;
    int skipMonthsFast;
    bool upgradeTransport;

    void save();
private:
    void load( const std::string& filename );
    int parseInt(const char* value, int defaultValue, int minValue = INT_MIN, 
            int maxValue = INT_MAX );
    bool parseBool(const char* value, bool defaultvalue);
};

Config* getConfig();

#endif
