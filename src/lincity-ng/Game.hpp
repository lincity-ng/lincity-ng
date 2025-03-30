/* ---------------------------------------------------------------------- *
 * src/lincity-ng/Game.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Matthias Braun <matze@braunis.de>
 * Copyright (C) 2025      David Bears <dbear4q@gmail.com>
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

#ifndef __GAME_HPP__
#define __GAME_HPP__

#include <SDL.h>  // for SDL_Window
#include <memory>       // for unique_ptr
#include <string>       // for string

#include "main.hpp"     // for MainState
#include "lincity/world.h"
#include "UserOperation.hpp"

class Button;
class Component;
class HelpWindow;
class UserOperation;
class GameView;
class MiniMap;
class EconomyGraph;
class MpsMap;
class MpsFinance;
class LCPBar;
class WindowManager;
class ButtonPanel;

class Game
{
public:
    Game(SDL_Window *window);
    ~Game();

    MainState run();

    void showHelpWindow(std::string topic);

    World& getWorld() const;
    void setWorld(std::unique_ptr<World>&& world);
    UserOperation& getUserOperation();
    const UserOperation& getUserOperation() const;

    Component& getGui() const;
    WindowManager& getWindowManager() const;
    GameView& getGameView() const;
    MiniMap& getMiniMap() const;
    EconomyGraph& getEconomyGraph() const;
    MpsMap& getMpsMap() const;
    MpsFinance& getMpsFinance() const;
    LCPBar& getPBar1() const;
    LCPBar& getPBar2() const;
    ButtonPanel& getButtonPanel() const;

    bool warnBullWater, warnBullShanty, warnBullMonument;

private:
    std::unique_ptr<World> world;

    std::unique_ptr<Component> gui;
    GameView *gameview = nullptr;
    MiniMap *minimap = nullptr;
    EconomyGraph *economygraph = nullptr;
    WindowManager *windowmanager = nullptr;
    MpsMap *mpsmap = nullptr;
    MpsFinance *mpsfinance = nullptr;
    LCPBar *pbar1 = nullptr;
    LCPBar *pbar2 = nullptr;
    ButtonPanel *buttonpanel = nullptr;

    UserOperation userOperation;
    void setUserOperation(const UserOperation& op);

    void loadGui();
    void processMessageQueue();

    bool running;
    MainState quitState;
    void backToMainMenu();
    void testAllHelpFiles();
    void quickLoad();
    void quickSave();
    std::unique_ptr<HelpWindow> helpWindow;
    SDL_Window *window;

    void updateDate() const;
    void updateMoney() const;
};

#endif


/** @file lincity-ng/Game.hpp */
