#ifndef __MAINMENU_HPP__
#define __MAINMENU_HPP__

#include "main.hpp"

class Component;
class Button;
class CheckButton;

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
    
    void fillLoadMenu();
    void fillNewGameMenu();
    
    void quitButtonClicked(Button* );
    void continueButtonClicked(Button* );
    void newGameButtonClicked(Button* );
    void loadGameButtonClicked(Button* );

    void newGameBackButtonClicked(Button* );
    void newGameStartButtonClicked(Button* );

    void loadGameBackButtonClicked(Button* );
    void loadGameLoadButtonClicked(Button* );
    
    void selectLoadGameButtonClicked(CheckButton*,int);
    
    std::auto_ptr<Component> mainMenu;
    std::auto_ptr<Component> newGameMenu;
    std::auto_ptr<Component> loadGameMenu;
    Component* currentMenu;
    
    bool running;
    MainState quitState;           
    
    std::string mFilename;    
};

#endif

