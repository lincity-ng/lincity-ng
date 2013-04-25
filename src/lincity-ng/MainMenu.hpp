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
#include "gui/Paragraph.hpp"
#include <memory>

#include <map>

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
    void loadSaveGameMenu();
    void loadCreditsMenu();
    void loadOptionsMenu();

    void fillLoadMenu( bool save = false );
    void fillNewGameMenu();

    void creditsBackButtonClicked(Button* );
    void optionsBackButtonClicked(Button* );

    void quitButtonClicked(Button* );
    void continueButtonClicked(Button* );
    void creditsButtonClicked(Button* );
    void newGameButtonClicked(Button* );
    void loadGameButtonClicked(Button* );
    void saveGameButtonClicked(Button* );
    void optionsButtonClicked(Button* );

    void newGameBackButtonClicked(Button* );
    void newGameStartButtonClicked(Button* );

    void loadGameBackButtonClicked(Button* );
    void loadGameLoadButtonClicked(Button* );
    void loadGameSaveButtonClicked(Button* );

    void selectLoadGameButtonClicked(CheckButton*,int i);
    void selectSaveGameButtonClicked(CheckButton*,int i);
    void selectLoadSaveGameButtonClicked(CheckButton*,int, bool save );
    void optionsMenuButtonClicked(CheckButton* button, int );

    std::auto_ptr<Component> mainMenu;
    std::auto_ptr<Component> newGameMenu;
    std::auto_ptr<Component> loadGameMenu;
    std::auto_ptr<Component> saveGameMenu;
    std::auto_ptr<Component> creditsMenu;
    std::auto_ptr<Component> optionsMenu;
    Desktop* currentMenu;

    bool running;
    MainState quitState;
    int slotNr;

    std::string mFilename;
    std::string baseName;
    static const Uint32 doubleClickTime = 1000;
    Uint32 lastClickTick;
    std::string doubleClickButtonName;

    Paragraph* musicParagraph;
    void changeTrack( bool next);

    void changeResolution( bool next);
    void changeWorldLen(bool next);

    Paragraph* languageParagraph;
    void changeLanguage( bool next);
    std::string currentLanguage;
    std::set<std::string> languages;

    std::map<std::string, std::string> fileMap;
};

#endif


/** @file lincity-ng/MainMenu.hpp */

