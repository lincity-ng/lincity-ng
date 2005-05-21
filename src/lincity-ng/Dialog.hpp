#ifndef __DIALOG_HPP__
#define __DIALOG_HPP__

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

extern bool blockingDialogIsOpen;

class Dialog
{
    public:
        Dialog( int type, int x, int y );
        Dialog( int type );
        ~Dialog();

    private:
        void askBulldozeMonument();
        void askBulldozeRiver();
        void askBulldozeShanty();

        void editMarket();
        void editPort();

        void coalSurvey();
        void askRocket();
        void gameStats();
       
        void initDialog( int x = -1 , int y = -1 );
        Desktop* desktop;
        Component* myDialogComponent;
        int pointX;
        int pointY;
       
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
};
#endif
