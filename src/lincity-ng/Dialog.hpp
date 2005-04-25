#ifndef __DIALOG_HPP__
#define __DIALOG_HPP__

#include "MapPoint.hpp"
#include "gui/Component.hpp"
#include "gui/Desktop.hpp"
#include "gui/Button.hpp"

#define BULLDOZE_MONUMENT 1
#define BULLDOZE_RIVER    2
#define BULLDOZE_SHANTY   3

extern bool blockingDialogIsOpen;

class Dialog
{
    public:
        Dialog( int type, int x, int y );
        ~Dialog();

    private:
        void Dialog::askBulldozeMonument();
        void Dialog::askBulldozeRiver();
        void Dialog::askBulldozeShanty();
        
        Desktop* desktop;
        Component* myDialogComponent;
        int pointX;
        int pointY;
        
        void okayBulldozeRiverButtonClicked( Button* );
        void okayBulldozeShantyButtonClicked( Button* );
        void okayBulldozeMonumentButtonClicked( Button* );
        void closeDialogButtonClicked( Button* );

};
#endif
