#ifndef __main_lincity_hpp__
#define __main_lincity_hpp__

#include <string>

void initLincity();
void doLincityStep();
void setLincitySpeed( int speed );

//get Data form Lincity NG and Save City
void saveCityNG( std::string newFilename );

//Load City and do setup for Lincity NG.
bool loadCityNG( std::string filename );

#endif
