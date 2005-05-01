#ifndef __MAINMENU_HPP__
#define __MAINMENU_HPP__

#include "main.hpp"

class Component;
class Button;
class CheckButton;

//get Data form Lincity NG and Save City
void saveCityNG( std::string newFilename );
//Load City and do setup for Lincity NG.
bool loadCityNG( std::string filename );

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
    void loadOptionsMenu();
    
    void fillLoadMenu();
    void fillNewGameMenu();

    void creditsBackButtonClicked(Button* );
    
    void quitButtonClicked(Button* );
    void continueButtonClicked(Button* );
    void creditsButtonClicked(Button* );
    void newGameButtonClicked(Button* );
    void loadGameButtonClicked(Button* );
    void optionsButtonClicked(Button* );

    void newGameBackButtonClicked(Button* );
    void newGameStartButtonClicked(Button* );
    void newGameStartBareButtonClicked(Button* );
    void newGameStartVillageClicked(Button* );
        
    void loadGameBackButtonClicked(Button* );
    void loadGameLoadButtonClicked(Button* );
    void loadGameSaveButtonClicked(Button* );

    void selectLoadGameButtonClicked(CheckButton*,int);
    
    void optionsMenuButtonClicked(Button* button);
    
    std::auto_ptr<Component> mainMenu;
    std::auto_ptr<Component> newGameMenu;
    std::auto_ptr<Component> loadGameMenu;
    std::auto_ptr<Component> creditsMenu;
    std::auto_ptr<Component> optionsMenu;
    Component* currentMenu;
    
    bool running;
    MainState quitState;           
    int slotNr;
    
    std::string mFilename;    
};

#endif

