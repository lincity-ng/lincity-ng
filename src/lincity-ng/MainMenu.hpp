#ifndef __MAINMENU_HPP__
#define __MAINMENU_HPP__

#include "main.hpp"

class Painter;
class Component;
class Button;

class MainMenu
{
public:
    MainMenu();
    ~MainMenu();

    MainState run();

private:
    void quitButtonClicked(Button* );
    void continueButtonClicked(Button* );
    void newGameButtonClicked(Button* );
    
    std::auto_ptr<Painter> painter;
    std::auto_ptr<Component> gui;
    
    bool running;
    MainState quitState;               
};

#endif

