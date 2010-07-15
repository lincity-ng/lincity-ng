/*
Copyright (C) 2005 Wolfgang Becker <uafr@gmx.de>

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
#ifndef __DIALOG_HPP__
#define __DIALOG_HPP__

#include <vector>
#include "MapPoint.hpp"
#include "gui/Component.hpp"
#include "gui/Desktop.hpp"
#include "gui/Button.hpp"

#define BULLDOZE_MONUMENT 1
#define BULLDOZE_RIVER    2
#define BULLDOZE_SHANTY   3
#define EDIT_MARKET       4
#define EDIT_PORT         5
#define ASK_COAL_SURVEY   6 
#define ASK_LAUNCH_ROCKET 7 
#define GAME_STATS        8            
#define MSG_DIALOG        9            

extern bool blockingDialogIsOpen;

class Dialog;
void closeAllDialogs();

class Dialog
{
    public:
        Dialog( int type, std::string message, std::string extraString);
        Dialog( int type, int x, int y );
        Dialog( int type );
        ~Dialog();
        void closeDialog();

    private:
        void askBulldozeMonument();
        void askBulldozeRiver();
        void askBulldozeShanty();

        void editMarket();
        void editPort();

        void coalSurvey();
        void askRocket();
        void gameStats();
        void saveGameStats();

        void msgDialog( std::string message, std::string extraString);
       
        void initDialog( int x = -1 , int y = -1 );
        Desktop* desktop;
        Component* myDialogComponent;
        int pointX;
        int pointY;
 
        template<typename T> void setTableRC( const std::string basename, const int row, const int column, const std::string text, const T value );

        void setParagraphN( const std::string basename, const int number, const std::string text );
        void setParagraph( const std::string paragraphName, const std::string text );
        void okayBulldozeRiverButtonClicked( Button* );
        void okayBulldozeShantyButtonClicked( Button* );
        void okayBulldozeMonumentButtonClicked( Button* );
        void okayCoalSurveyButtonClicked( Button* );
        void okayLaunchRocketButtonClicked( Button* );
        void closeDialogButtonClicked( Button* );
        void gotoButtonClicked( Button* );
        void applyMarketButtonClicked( Button* );
        void applyPortButtonClicked( Button* );
        bool iAmBlocking;
        void registerDialog();
        void unRegisterDialog();
};

#endif

/** @file lincity-ng/Dialog.hpp */

