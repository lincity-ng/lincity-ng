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
    void loadMainMenu();
    void loadNewGameMenu();
    void loadLoadGameMenu();
    
    void quitButtonClicked(Button* );
    void continueButtonClicked(Button* );
    void newGameButtonClicked(Button* );
    void loadGameButtonClicked(Button* );

    void newGameBackButtonClicked(Button* );
    void newGameStartButtonClicked(Button* );

    void loadGameBackButtonClicked(Button* );
    void loadGameLoadButtonClicked(Button* );
    
    std::auto_ptr<Painter> painter;
    std::auto_ptr<Component> mainMenu;
    std::auto_ptr<Component> newGameMenu;
    std::auto_ptr<Component> loadGameMenu;
    Component* currentMenu;
    
    bool running;
    MainState quitState;               
};

#endif

