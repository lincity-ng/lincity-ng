/* ---------------------------------------------------------------------- *
 * src/lincity-ng/MainMenu.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Matthias Braun <matze@braunis.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#ifndef __MAINMENU_HPP__
#define __MAINMENU_HPP__

#include <SDL.h>                     // for SDL_Window, Uint32
#include <filesystem>                // for path
#include <functional>                // for function
#include <memory>                    // for unique_ptr
#include <set>                       // for set
#include <string>                    // for basic_string, string
#include <unordered_map>             // for unordered_map

#include "gui/RadioButtonGroup.hpp"  // for RadioButtonGroup

class Button;
class CheckButton;
class Component;
class Desktop;
class Game;
class Paragraph;
class SwitchComponent;

class MainMenu
{
public:
    MainMenu(SDL_Window* window);
    ~MainMenu();

    void run();
    void gotoMainMenu();

private:
    void switchMenu(Component* component);

    void loadMainMenu();
    void loadNewGameMenu();
    void loadLoadGameMenu();
    void loadSaveGameMenu();
    void loadCreditsMenu();
    void loadOptionsMenu();

    void updateLoadSaveMenus();
    void updateNewGameMenu();
    void updateOptionsMenu();

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

    void optionsMenuButtonClicked(CheckButton* button, int );
    void optionsBackButtonClicked(Button* );

    void creditsBackButtonClicked(Button* );

    std::unique_ptr<Game> game;
    void launchGame();

    SDL_Window* window;
    std::unique_ptr<Desktop> menu;
    SwitchComponent *menuSwitch;
    Component *mainMenu;
    Component *newGameMenu;
    Component *loadGameMenu;
    Component *saveGameMenu;
    Component *creditsMenu;
    Component *optionsMenu;

    RadioButtonGroup newGameSelection;
    RadioButtonGroup loadGameSelection;
    RadioButtonGroup saveGameSelection;
    std::unordered_map<CheckButton *, std::filesystem::path> loadFiles;

    enum class State {
      MENU, GAME, QUIT, RESTART
    };
    State state;

    static const Uint32 doubleClickTime = 1000;
    Uint32 doubleClickTick = 0;
    Component *doubleClickButton = nullptr;
    void doubleClick(Component *button, std::function<void()> action);

    Paragraph* musicParagraph;
    void changeTrack( bool next);

    void changeResolution( bool next);
    void changeWorldLen(bool next);

#if ENABLE_NLS
    Paragraph* languageParagraph;
    void changeLanguage( bool next);
    std::string currentLanguage;
    std::set<std::string> languages;
#endif
};

#endif


/** @file lincity-ng/MainMenu.hpp */
