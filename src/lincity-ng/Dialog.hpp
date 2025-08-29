/* ---------------------------------------------------------------------- *
 * src/lincity-ng/Dialog.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Wolfgang Becker <uafr@gmx.de>
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

#ifndef __DIALOG_HPP__
#define __DIALOG_HPP__

#include <memory>
#include <string>  // for string

#include "lincity/MapPoint.hpp"

class Button;
class Window;
class WindowManager;
class Game;

#define EDIT_MARKET       4
#define EDIT_PORT         5
#define ASK_COAL_SURVEY   6
#define ASK_LAUNCH_ROCKET 7
#define GAME_STATS        8

extern bool blockingDialogIsOpen;

void closeAllDialogs();

class Dialog
{
    public:
        Dialog(Game& game, int type, MapPoint point);
        Dialog(Game& game, int type);
        ~Dialog();
        void closeDialog();

    private:
        void editMarket();
        void editPort();

        void coalSurvey();
        void askRocket();
        void gameStats();
        void saveGameStats();

        void initDialog();
        WindowManager* windowManager;
        Window *myDialogComponent;
        MapPoint point;
        Game& game;

        template<typename T> void setTableRC( const std::string basename, const int row, const int column, const std::string text, const T value );

        void setParagraphN( const std::string basename, const int number, const std::string text );
        void setParagraph( const std::string paragraphName, const std::string text );
        void okayCoalSurveyButtonClicked( Button* );
        void okayLaunchRocketButtonClicked( Button* );
        void closeDialogButtonClicked( Button* );
        void gotoButtonClicked( Button* );
        void applyMarketButtonClicked( Button* );
        void applyPortButtonClicked( Button* );
        bool iAmBlocking;
        void registerDialog(std::unique_ptr<Window>&&);
        void unRegisterDialog();
};

#endif

/** @file lincity-ng/Dialog.hpp */
