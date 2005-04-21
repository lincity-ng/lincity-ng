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
    void gotoMainMenu();

private:
    void loadMainMenu();
    void loadNewGameMenu();
    void loadLoadGameMenu();
    void loadCreditsMenu();
    
    void fillLoadMenu();
    void fillNewGameMenu();

    void creditsBackButtonClicked(Button* );
    
    void quitButtonClicked(Button* );
    void continueButtonClicked(Button* );
    void creditsButtonClicked(Button* );
    void newGameButtonClicked(Button* );
    void loadGameButtonClicked(Button* );

    void newGameBackButtonClicked(Button* );
    void newGameStartButtonClicked(Button* );
    void newGameStartBareButtonClicked(Button* );
    void newGameStartVillageClicked(Button* );
        
    void loadGameBackButtonClicked(Button* );
    void loadGameLoadButtonClicked(Button* );
    void loadGameSaveButtonClicked(Button* );
    
    void selectLoadGameButtonClicked(CheckButton*,int);
    
    std::auto_ptr<Component> mainMenu;
    std::auto_ptr<Component> newGameMenu;
    std::auto_ptr<Component> loadGameMenu;
    std::auto_ptr<Component> creditsMenu;
    Component* currentMenu;
    
    bool running;
    MainState quitState;           
    int slotNr;
    
    std::string mFilename;    
};

#endif

