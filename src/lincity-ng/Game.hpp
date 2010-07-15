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
#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "main.hpp"
#include <memory>
#include "gui/Button.hpp"

class Painter;
class Sound;
class Component;
class HelpWindow;

class Game
{
public:
    Game();
    ~Game();

    MainState run();
    void gameButtonClicked( Button* button );
    void showHelpWindow( std::string topic );

private:
    std::auto_ptr<Component> gui;
    
    bool running;
    MainState quitState;
    void backToMainMenu();
    void testAllHelpFiles();
    void quickLoad();
    void quickSave();
    std::auto_ptr<HelpWindow> helpWindow;
};

Game* getGame();

#endif


/** @file lincity-ng/Game.hpp */

