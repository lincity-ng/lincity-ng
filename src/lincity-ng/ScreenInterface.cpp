#include <config.h>
#include <iostream>
#include <stdarg.h>

#include "gui_interface/screen_interface.h"
#include "gui_interface/pbar_interface.h"

#include "lincity/engglobs.h"
#include "lincity/lclib.h"

#include "gui/Component.hpp"
#include "gui/Paragraph.hpp"
#include "gui/Desktop.hpp"
#include "GameView.hpp"
#include "Util.hpp"
#include "gui/ComponentLoader.hpp"

void updateMessageText( const std::string text );

short mappointoldtype[WORLD_SIDE_LEN][WORLD_SIDE_LEN];
int selected_module_cost; // this must be changed, when module (or celltype-button) is changed


int getMainWindowWidth();
int getMainWindowHeight();


/* This is on in screen_full_refresh, used in *_refresh() */
char screen_refreshing;

char* current_month (int current_time);
void draw_cb_box (int row, int col, int checked);

int ask_launch_rocket_click (int x, int y);
int ask_launch_rocket_now (int , int )
{
    return yn_dial_box ("Rocket ready to launch",
	    "You can launch it now or wait until later.",
		"If you wait it will continue costing you money.",
		"Launch it later by clicking on the rocket area.");
}

void display_rocket_result_dialog (int result)
{
    switch (result) {
    case ROCKET_LAUNCH_BAD:
	ok_dial_box ("launch-fail.mes", BAD, 0L);
	break;
    case ROCKET_LAUNCH_GOOD:
	ok_dial_box ("launch-good.mes", GOOD, 0L);
	break;
    case ROCKET_LAUNCH_EVAC:
	ok_dial_box ("launch-evac.mes", GOOD, 0L);
	break;
    }
}
void draw_background (void);
void screen_full_refresh (void);
void init_fonts (void);
void initialize_monthgraph (void);
void rotate_mini_screen (void);
void advance_mps_style (void);
/*void update_main_screen (int full_refresh)
{
}*/
void connect_transport_main_screen (void);

void refresh_mps (void);
void advance_monthgraph_style (void);
void refresh_monthgraph (void);
void draw_small_yellow_bezel (int x, int y, int h, int w);
void init_pbars (void);
void mini_screen_help (void);
void status_message(char *m1, char* m2);
void print_time_for_year (void);
void rotate_main_screen (void);
void screen_setup (void);

/* Miniscreen */
void init_mini_map_mouse(void);
void mini_map_handler(int x, int y, int button);
void mini_aux_handler(int x, int y, int button);
/* Message area */
void display_info_message (int colour, char* ss, char* xs)
{
    std::ostringstream text;
    text << "display_info_message: '"<< colour << ss << "' + \"" << xs << "\"\n";
    updateMessageText( text.str() );
}

void reset_status_message (void);

/*
 * Display some Text in a Dialog Box with an OK Button.
 * 
 * see oldgui/screen.cpp: ok_dial_box (char *fn, int good_bad, char *xs)
 *
 * good_bad influences the color: GOOD, RESULTS in green
 *                                BAD in red
 *                                default white
 * fn is the FileName of the message
 *   if good_bad is RESULTS the fn is an absolute filename else
 *   it is a File in message_path.                              
 * 
 * xs is some additional Text, that is shown after the Message
 *    from the File. (maybe XtraString?)  
 */
 void ok_dial_box (char *fn, int, char *xs)
{
    std::ostringstream text;
    text << "ok_dial_box:'" << fn << "' + \"" << xs << "\"\n";
    updateMessageText( text.str() );
}

/* 
 * Update Message in Message Window
 */
void updateMessageText( const std::string text )
{
    //Dialog Test
    Component* root = getGameView();
    if(!root) {
        std::cerr << "Root not found!?!\n";
        return;
    }
    while( root->getParent() )
        root = root->getParent();
    Desktop* desktop = dynamic_cast<Desktop*> (root);
    if(!desktop) {
        std::cerr << "Root not a desktop!?!\n";
        return;
    }

    try {
        //test if message Windows is open
        Component* messageTextComponent = 0;
        messageTextComponent = root->findComponent( "messageText" );
        if(messageTextComponent == 0) {
            messageTextComponent = loadGUIFile("gui/messagearea.xml");
            desktop->addChildComponent(messageTextComponent);
            assert(messageTextComponent != 0);
        }
        Paragraph* messageText = getParagraph(*messageTextComponent, "messageText");
        
        messageText->setText( text );
    } catch(std::exception& e) {
        std::cerr << "Couldn't display message '" << text << "': "
            << e.what() << "\n";
        return;
    }
}

/*
 * Update the Title of the Message Window
 * $(Current Date) -- Messages -- $(Current Money) 
 */
void updateMessageTitle()
{
    std::ostringstream titleText;
 
    titleText << current_month( total_time );
    titleText << " "<< current_year( total_time ) << " -- ";
    titleText << "Messages";
    titleText << " -- " << total_money << "£";
    
    Component* root = getGameView();
    if( !root ) return;
    while( root->getParent() )
        root = root->getParent();
    //test if message Windows is open and TODO: create it on demand
    Component* messageTitleComponent = 0;
    messageTitleComponent = root->findComponent( "messageTitle" );
    if( messageTitleComponent == 0 ) {
        return;
    }
    Paragraph* messageTitle = getParagraph( *root, "messageTitle");
    if( !messageTitle ) return;
    
    messageTitle->setText( titleText.str() );
}

/*
 *  Draw a Dialogbox, in oldgui/screen.cpp it is called from the 
 *  other Dialog Functions to do the actual drawing.
 *  TODO: at the moment we just dump the Text in the MessageArea...
 *
 *  see oldgui/dialbox.cpp 
 *  -there must be just one DialogBox at a time.
 *  colour is the color of the Background of the Dialog
 *
 *  Do we use dialog_box at all? Unfortunately yes. There is a lot of legacy code...
 *  
 *  argc is the number of following argument triplets.
 *  Each triplet consists of
 *      type, key, string
 *      type is an integer that describes this entry
 *          0 = it is a text to display
 *          2 = a button 
 *      key 
 *        a char as hotkey for the button
 *        10 (LF), 13(CR) or ' '(blank)
 *        are used to mark the default button
 *        also key is returned as result of the Dialog if the corespondending Button is pressed.
 *           
 *      string is the text
 */
int dialog_box(int colour, int argc, ...)
{
    (void) colour;

    std::ostringstream text;
    va_list arg;
    va_start( arg, argc );
     for( int i = 0; i < argc; i++ ) {
        text << std::cout <<  va_arg( arg, int ) << " '"<<  va_arg( arg, int )  << "' " << va_arg( arg, char* ) <<"\n";
     }
    va_end( arg );    
    
    updateMessageText(  text.str() );
    
    return 0;
}

/*
 *  A DialogBox with a Progressbar.
 *  see oldgui/screen.cpp: prog_box (char *title, int percent)
 *  is used to open a Dialog with given Title an a Progressbar,
 *  showing percent completed, but is also used to update
 *  the current Progressbar.
 */
void prog_box (char *title, int percent)
{
    std::ostringstream text;
    text << "prog_box:'" << title << "' " << percent << "%\n";
    std::cout << text;
}

void print_total_money (void)
{
    updateMessageTitle();
}

void refresh_population_text (void)
{
  //  TRACE;
  update_pbar (PPOP, housed_population + people_pool, 0);

}

void update_avail_modules (int popup)
{
    (void) popup;
}

void refresh_main_screen()
{
}

void screen_full_refresh ()
{
  updateMessageTitle();
  update_main_screen (true);
}

void print_stats ()
{
  // this show update the financy window or mps

  if (total_time % NUMOF_DAYS_IN_MONTH == (NUMOF_DAYS_IN_MONTH - 1))
    update_pbars_monthly();

}
void update_main_screen_normal (int full_refresh)
{
    (void) full_refresh;
    return;
}

void update_main_screen (int full_refresh)
{
    update_main_screen_normal (full_refresh);
}

void print_time_for_year ()
{
}

int getMainWindowWidth()
{
  // FIXME: insert some function to return screen size
  return 800;
  //  return getScreen().getRect().w;
}
int getMainWindowHeight()
{
  // FIXME: insert some function to return screen size
  return 800;
  //  return getScreen().getRect().h;
}

int yn_dial_box (char * s1, char * s2, char * s3, char *s4)
{
    int result;
    result = dialog_box(red(10),7,
			0,0,s1,
			0,0,"",
			0,0,s2,
			0,0,s3,
			0,0,s4,
			1,'y',"Yes",
			1,'n',"No");

    return (result == 'y') ? 1 : 0;
}
