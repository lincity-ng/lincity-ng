/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef __MAINMENU_HPP__
#define __MAINMENU_HPP__

#include "main.hpp"

class Component;
class Desktop;
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
    void switchMenu(Component* component);
    
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
    void optionsMenuButtonClicked(CheckButton* button, int );
    
    std::auto_ptr<Component> mainMenu;
    std::auto_ptr<Component> newGameMenu;
    std::auto_ptr<Component> loadGameMenu;
    std::auto_ptr<Component> creditsMenu;
    std::auto_ptr<Component> optionsMenu;
    Desktop* currentMenu;
    
    bool running;
    MainState quitState;           
    int slotNr;
    
    std::string mFilename;    
    std::string baseName;
};

#endif

