#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <string>

class Config 
{
public:
    Config();
    ~Config();
    
    bool useOpenGL;
    int videoX, videoY;
private:
    void load( const std::string& filename );
    void save();
};

Config* getConfig();

#endif
