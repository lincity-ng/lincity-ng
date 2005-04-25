/**
 *  Dialogs for Lincity-NG.
 *   
 *  April 2005, Wolfgang Becker <uafr@gmx.de>
 *
 */
#include <config.h>

#include "Dialog.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "GameView.hpp"
#include "Util.hpp"
#include "MapEdit.hpp"

#include "gui/ComponentLoader.hpp"
#include "gui/Button.hpp"
#include "gui/callback/Callback.hpp"

bool blockingDialogIsOpen = false;

Dialog::Dialog( int type, int x, int y ){
    Component* root = getGameView();
    desktop = 0;
    myDialogComponent = 0;
    pointX = x;
    pointY = y;
        
    if( root ) {
        while( root->getParent() )
            root = root->getParent();
        desktop = dynamic_cast<Desktop*> (root);
        if(!desktop) 
            std::cerr << "Root not a desktop!?!\n";
    } else {
        std::cerr << "Dialog: Root not found.\n";
    }

    switch( type ) {
        case BULLDOZE_MONUMENT:
            askBulldozeMonument();
            break;
        case BULLDOZE_RIVER:
            askBulldozeRiver();
            break;
        case BULLDOZE_SHANTY:
            askBulldozeShanty();
            break;
        default:
            std::stringstream msg;
            msg <<"Unknown Dialog type " << type << ".";
            throw std::runtime_error(msg.str());
    }
}

Dialog::~Dialog(){
    std::cout << "~Dialog()\n";
}

void Dialog::askBulldozeMonument() {
    if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    try {
        myDialogComponent = loadGUIFile( "gui/bulldoze_monument_yn.xml" );
        assert( myDialogComponent != 0);
        desktop->addChildComponent( myDialogComponent );
        blockingDialogIsOpen = true;
    } catch(std::exception& e) {
        std::cerr << "Couldn't display message 'bulldoze_monument_yn': "
            << e.what() << "\n";
        return;
    }
    // connect signals
    Button* yesButton = getButton( *myDialogComponent, "Yes" );
    yesButton->clicked.connect( makeCallback(*this, &Dialog::okayBulldozeMonumentButtonClicked ) );
    Button* noButton = getButton( *myDialogComponent, "No" );
    noButton->clicked.connect( makeCallback( *this, &Dialog::closeDialogButtonClicked ) );
}

void Dialog::askBulldozeRiver() {
    if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    try {
        myDialogComponent = loadGUIFile( "gui/bulldoze_river_yn.xml" );
        assert( myDialogComponent != 0);
        desktop->addChildComponent( myDialogComponent );
        blockingDialogIsOpen = true;
    } catch(std::exception& e) {
        std::cerr << "Couldn't display message 'bulldoze_river_yn.xml': "
            << e.what() << "\n";
        return;
    }
    // connect signals
    Button* yesButton = getButton( *myDialogComponent, "Yes" );
    yesButton->clicked.connect( makeCallback(*this, &Dialog::okayBulldozeRiverButtonClicked ) );
    Button* noButton = getButton( *myDialogComponent, "No" );
    noButton->clicked.connect( makeCallback( *this, &Dialog::closeDialogButtonClicked ) );
}

void Dialog::askBulldozeShanty() {
    if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    try {
        myDialogComponent = loadGUIFile( "gui/bulldoze_shanty_yn.xml" );
        assert( myDialogComponent != 0);
        desktop->addChildComponent( myDialogComponent );
        blockingDialogIsOpen = true;
    } catch(std::exception& e) {
        std::cerr << "Couldn't display message 'bulldoze_shanty_yn': "
            << e.what() << "\n";
        return;
    }
    // connect signals
    Button* yesButton = getButton( *myDialogComponent, "Yes" );
    yesButton->clicked.connect( makeCallback(*this, &Dialog::okayBulldozeShantyButtonClicked ) );
    Button* noButton = getButton( *myDialogComponent, "No" );
    noButton->clicked.connect( makeCallback( *this, &Dialog::closeDialogButtonClicked ) );
}

void Dialog::okayBulldozeRiverButtonClicked( Button* ){
    river_bul_flag = 1;
    desktop->remove( myDialogComponent );
    check_bulldoze_area( pointX, pointY );
    blockingDialogIsOpen = false;
    delete( this );
}

void Dialog::okayBulldozeShantyButtonClicked( Button* ){
    shanty_bul_flag = 1;
    desktop->remove( myDialogComponent );
    check_bulldoze_area( pointX, pointY );
    blockingDialogIsOpen = false;
    delete( this );
}

void Dialog::okayBulldozeMonumentButtonClicked( Button* ){
    monument_bul_flag = 1;
    desktop->remove( myDialogComponent );
    check_bulldoze_area( pointX, pointY );
    blockingDialogIsOpen = false;
    delete( this );
}

void Dialog::closeDialogButtonClicked( Button* ){
    desktop->remove( myDialogComponent );
    blockingDialogIsOpen = false;
    delete( this );
}
