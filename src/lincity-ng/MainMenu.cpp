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
#include <config.h>

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <physfs.h>
#include <stdio.h>
#include <time.h>

#include "gui/TextureManager.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/Component.hpp"
#include "gui/Event.hpp"
#include "gui/Desktop.hpp"
#include "gui/Button.hpp"
#include "gui/callback/Callback.hpp"

#include "gui_interface/shared_globals.h"
#include "lincity/lin-city.h"
#include "lincity/fileutil.h"

#include "CheckButton.hpp"

#include "MainMenu.hpp"
#include "Util.hpp"
#include "Config.hpp"
#include "Sound.hpp"
#include "GameView.hpp"
#include "MainLincity.hpp"
#include "readdir.hpp"

#include "tinygettext/gettext.hpp"

extern void new_city(int *originx, int *originy, int random_village);

MainMenu::MainMenu()
{
    std::cout << "XXX MainMenu() " << SDL_GetTicks() << "\n";
    loadMainMenu();
    switchMenu(mainMenu.get());
    baseName = "";
    lastClickTick = 0;
    doubleClickButtonName = "";
    std::cout << "XXX MainMenu() done" << SDL_GetTicks() << "\n";
}

MainMenu::~MainMenu()
{
}

void
MainMenu::loadMainMenu()
{
      std::cout << "XXX load main " << SDL_GetTicks() << "\n";
    if(mainMenu.get() == 0) {
        mainMenu.reset(loadGUIFile("gui/mainmenu.xml"));
        // connect signals
        Button* quitButton = getButton(*mainMenu, "QuitButton");
        quitButton->clicked.connect(
                makeCallback(*this, &MainMenu::quitButtonClicked));
        Button* continueButton = getButton(*mainMenu, "ContinueButton"); 
        continueButton->clicked.connect(
                makeCallback(*this, &MainMenu::continueButtonClicked));
        Button* newGameButton = getButton(*mainMenu, "NewGameButton");
        newGameButton->clicked.connect(
                makeCallback(*this, &MainMenu::newGameButtonClicked));
        Button* loadGameButton = getButton(*mainMenu, "LoadButton");
        loadGameButton->clicked.connect(
                makeCallback(*this, &MainMenu::loadGameButtonClicked));
        Button* saveGameButton = getButton(*mainMenu, "SaveButton");
        saveGameButton->clicked.connect(
                makeCallback(*this, &MainMenu::saveGameButtonClicked));
        Button* creditsButton = getButton(*mainMenu, "CreditsButton");
        creditsButton->clicked.connect(
                makeCallback(*this, &MainMenu::creditsButtonClicked));
        Button* optionsButton = getButton(*mainMenu, "OptionsButton");
        optionsButton->clicked.connect(
                makeCallback(*this, &MainMenu::optionsButtonClicked));
 
    }

    mainMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
      std::cout << "XXX done connecting " << SDL_GetTicks() << "\n";
}

void MainMenu::fillNewGameMenu()
{
  const std::string buttonNames[]={"File0","File1","File2","File3","File4","File5"};
  
  char **files= PHYSFS_enumerateFiles("opening");
  
  char **fptr=files;

  fileMap.clear(); 
 
  for(int i=0;i<6;i++)
  {
    CheckButton *button=getCheckButton(*newGameMenu.get(),buttonNames[i]);
    
    button->clicked.connect(makeCallback(*this,&MainMenu::selectLoadGameButtonClicked));
    while(*fptr)
    {
      if(std::string(*fptr).find(".scn")!=std::string::npos)
        break;
      fptr++;
    }
    if(*fptr)
    {
      std::string f=*fptr;
      if(f.length()>5){
        f=f.substr(0,f.length()-4); // truncate .scn
      }
      // save real name
      fileMap.insert(std::pair<std::string, std::string>(buttonNames[i], f ));
      // use translated name for caption 
      button->setCaptionText(_(f.c_str()));
      fptr++;
    }
    else
      button->setCaptionText("");
  }
  PHYSFS_freeList(files);
  return;
  /* Is there a better way to add filenames to the directory? */
  _("good_times");
  _("bad_times");
  _("Beach");
  _("extreme_arid");
  _("extreme_wetland");
}

void MainMenu::fillLoadMenu( bool save /*= false*/ )
{
    const std::string buttonNames[]={"File0","File1","File2","File3","File4","File5"};
  
    char** rc = PHYSFS_enumerateFiles("/");

    char* curfile;
    CheckButton *button;

    for(int i=0;i<6;i++) {
    	char* recentfile = NULL;
        PHYSFS_sint64 t = 0;

        std::stringstream filestart;
        filestart << i+1 << "_";
        if( save ){
            button = getCheckButton(*saveGameMenu.get(),buttonNames[i]);
        } else {
            button = getCheckButton(*loadGameMenu.get(),buttonNames[i]);
        }
        //make sure Button is connected only once 
        button->clicked.clear(); 
        if( save )
            button->clicked.connect(makeCallback(*this,&MainMenu::selectSaveGameButtonClicked));
        else {
            button->clicked.connect(makeCallback(*this,&MainMenu::selectLoadGameButtonClicked));
        }
        for(char** i = rc; *i != 0; i++){
            curfile = *i;
            if(std::string( curfile ).find( filestart.str() ) == 0 ) {
                // && !( curfile->d_type & DT_DIR  ) ) is not portable. So
                // don't create a directoy named 2_ in a savegame-directory or
                // you can no longer load from slot 2.
    	        if (t == 0) {
                    recentfile = curfile;
                    t = PHYSFS_getLastModTime(recentfile);
              } else {
                    if (PHYSFS_getLastModTime(curfile) > t) {
#ifdef DEBUG
                        fprintf(stderr," %s is more recent than previous %s\n",
                                          curfile, recentfile);
#endif
                        recentfile = curfile;
                        t = PHYSFS_getLastModTime(recentfile);
                    }
                }
            }
        }
#ifdef DEBUG
        fprintf(stderr,"Most recent file: %s\n\n",recentfile);
#endif

        if(t != 0) {
            std::string f= recentfile;
            button->setCaptionText(f);
        } else {
            button->setCaptionText(_("empty"));
        }
    }
}

void
MainMenu::loadNewGameMenu()
{
    if(newGameMenu.get() == 0) {
        newGameMenu.reset(loadGUIFile("gui/newgame.xml"));

        // connect signals
        Button* startButton = getButton(*newGameMenu, "StartButton");
        startButton->clicked.connect(
                makeCallback(*this, &MainMenu::newGameStartButtonClicked));
        Button* backButton = getButton(*newGameMenu, "BackButton");
        backButton->clicked.connect(
                makeCallback(*this, &MainMenu::newGameBackButtonClicked));
        Button* startBareButton = getButton(*newGameMenu, "StartBareButton");
        startBareButton->clicked.connect(
                makeCallback(*this, &MainMenu::newGameStartBareButtonClicked));
        Button* startVillageButton = getButton(*newGameMenu, "StartVillageButton");
        startVillageButton->clicked.connect(
                makeCallback(*this, &MainMenu::newGameStartVillageClicked));
        fillNewGameMenu();
    }

    newGameMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}

void
MainMenu::loadCreditsMenu()
{
    if(creditsMenu.get() == 0) {
        creditsMenu.reset(loadGUIFile("gui/credits.xml"));
        Button* backButton = getButton(*creditsMenu, "BackButton");
        backButton->clicked.connect(
                makeCallback(*this, &MainMenu::creditsBackButtonClicked));
    }

    creditsMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}

void
MainMenu::loadOptionsMenu()
{
    if(optionsMenu.get() == 0) {
        optionsMenu.reset(loadGUIFile("gui/options.xml"));
        CheckButton* currentCheckButton = getCheckButton(*optionsMenu, "BackgroundMusic");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "SoundFX");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "Fullscreen");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "MusicVolumePlus");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "MusicVolumeMinus");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "FXVolumePlus");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "FXVolumeMinus");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "TrackPrev");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "TrackNext");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        
        Button* currentButton = getButton(*optionsMenu, "BackButton");
        currentButton->clicked.connect( makeCallback(*this, &MainMenu::creditsBackButtonClicked));
    }
    //adjust checkbutton-states
    if( getConfig()->musicEnabled ){
        getCheckButton(*optionsMenu, "BackgroundMusic")->check();
    } else {
        getCheckButton(*optionsMenu, "BackgroundMusic")->uncheck();
    }
    if( getConfig()->soundEnabled ){
        getCheckButton(*optionsMenu, "SoundFX")->check();
    } else {
        getCheckButton(*optionsMenu, "SoundFX")->uncheck();
    }
    if( getConfig()->useFullScreen ){
        getCheckButton(*optionsMenu, "Fullscreen")->check();
    } else {
        getCheckButton(*optionsMenu, "Fullscreen")->uncheck();
    }
    //current background track
    musicParagraph = getParagraph( *optionsMenu, "musicParagraph");
    musicParagraph->setText(getConfig()->playSongName);

    optionsMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}

void
MainMenu::loadLoadGameMenu()
{
    if(loadGameMenu.get() == 0) {
        loadGameMenu.reset(loadGUIFile("gui/loadgame.xml"));

        // connect signals
        Button* loadButton = getButton(*loadGameMenu, "LoadButton");
        loadButton->clicked.connect(
                makeCallback(*this, &MainMenu::loadGameLoadButtonClicked));
        Button* backButton = getButton(*loadGameMenu, "BackButton");
        backButton->clicked.connect(
                makeCallback(*this, &MainMenu::loadGameBackButtonClicked));
    }

    // fill in file-names into slots
    fillLoadMenu();
    loadGameMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}

void
MainMenu::loadSaveGameMenu()
{
    if(saveGameMenu.get() == 0) {
        saveGameMenu.reset(loadGUIFile("gui/savegame.xml"));

        // connect signals
        Button* saveButton = getButton(*saveGameMenu, "SaveButton");
        saveButton->clicked.connect(
                makeCallback(*this, &MainMenu::loadGameSaveButtonClicked));
        Button* backButton = getButton(*saveGameMenu, "BackButton");
        backButton->clicked.connect(
                makeCallback(*this, &MainMenu::loadGameBackButtonClicked));
        // fill in file-names into slots
        fillLoadMenu( true );
    }

    saveGameMenu->resize(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}


void
MainMenu::selectLoadGameButtonClicked(CheckButton* button, int i){
    selectLoadSaveGameButtonClicked( button , i,  false );
}

void 
MainMenu::selectSaveGameButtonClicked(CheckButton* button, int i){
    selectLoadSaveGameButtonClicked( button , i, true );
}

void 
MainMenu::selectLoadSaveGameButtonClicked(CheckButton* button , int, bool save )
{
    std::string fc=button->getCaptionText();
    if( newGameMenu.get()==currentMenu ) {
        std::map<std::string, std::string>::iterator iter;
        iter = fileMap.find( button->getName() );
        if( iter != fileMap.end() ){
            fc = iter->second;
        }
    }
  
    std::string file="";
    
    /* I guess this should be the proper way of selecting in the menu.
       Everytime we check a new button the last one gets unchecked.
       If the button checked is an empty one, nothing should be opened 
       Could be done the other way around: the first time an existing item
       is selected in the menu, an empty one could never be checked again.
       Anyway I don't think both should be checked, when an empty is checked
       after an existing one.
    */
       
    const std::string bs[]={"File0","File1","File2","File3","File4","File5",""};
    for(int i=0;std::string(bs[i]).length();i++) {
        CheckButton *b=getCheckButton(*currentMenu,bs[i]);
        if(b->getName()!=button->getName()){
            b->uncheck();
        } else {
            b->check();
        }
    }

    if( !fc.length()) {
        mFilename = "";
        return;
    }
    
    baseName = fc;
    if(newGameMenu.get()==currentMenu ) {
        file=std::string("opening/")+fc+".scn";
    } else {
        file=fc;
    }
    
    mFilename="";
    if(newGameMenu.get()!=currentMenu) {
        slotNr = 1 + atoi( 
                const_cast<char*>(button->getName().substr(4).c_str()) );
        if( file.length() == 0){
            mFilename = "";
            return;
        }
    }
    
    mFilename+=file;
    Uint32 now = SDL_GetTicks();

    //doubleclick on Filename loads File
    if( ( fc == doubleClickButtonName ) &&  ( !save ) &&
            ( now - lastClickTick < doubleClickTime ) ) {

        lastClickTick = 0;
        doubleClickButtonName = "";
        if( newGameMenu.get() == currentMenu ) {
            //load scenario 
            newGameStartButtonClicked( 0 );
        } else {
            //load game
            loadGameLoadButtonClicked( 0 );
        }
    } else {
        lastClickTick = now;
        doubleClickButtonName = fc;
    }
}

void MainMenu::optionsMenuButtonClicked( CheckButton* button, int ){
    std::string buttonName = button->getName();
    if( buttonName == "BackgroundMusic"){
        getSound()->playSound("Click");
        getSound()->enableMusic( !getConfig()->musicEnabled );
    } else if( buttonName == "MusicVolumePlus"){
        int newVolume = getConfig()->musicVolume + 5;
        if( newVolume > 100 ){
           newVolume = 100; 
        }
        if( getConfig()->musicVolume != newVolume ){
            getSound()->setMusicVolume( newVolume ); 
            getSound()->playSound("Click");
        }
    } else if( buttonName == "MusicVolumeMinus"){
        int newVolume = getConfig()->musicVolume -5;
        if( newVolume < 0 ){
           newVolume = 0; 
        }
        if( getConfig()->musicVolume != newVolume ){
            getSound()->setMusicVolume( newVolume ); 
            getSound()->playSound("Click");
        }
    } else if( buttonName == "SoundFX"){
        getConfig()->soundEnabled = !getConfig()->soundEnabled;
        getSound()->playSound("Click");
    } else if( buttonName == "FXVolumePlus"){
        int newVolume = getConfig()->soundVolume + 5;
        if( newVolume > 100 ){
           newVolume = 100; 
        }
        if( getConfig()->soundVolume != newVolume ){
            getSound()->setSoundVolume( newVolume ); 
            getSound()->playSound("Click");
        }
    } else if( buttonName == "FXVolumeMinus"){
        int newVolume = getConfig()->soundVolume - 5;
        if( newVolume < 0 ){
           newVolume = 0; 
        }
        if( getConfig()->soundVolume != newVolume ){
            getSound()->setSoundVolume( newVolume ); 
            getSound()->playSound("Click");
        }
    } else if( buttonName == "Fullscreen"){
        getSound()->playSound("Click");
        getConfig()->useFullScreen = !getConfig()->useFullScreen; 
        if( getConfig()->restartOnChangeScreen ){
            quitState = RESTART;
            running = false;
        } else {
            initVideo(getConfig()->videoX, getConfig()->videoY);
            loadOptionsMenu();
        }
    } else if( buttonName == "TrackPrev"){
        changeTrack(false);
    } else if( buttonName == "TrackNext"){
        changeTrack(true);
    } else {
        std::cerr << "MainMenu::optionsMenuButtonClicked " << buttonName << " unknown Button!\n";
    }    
}

void
MainMenu::changeTrack( bool next)
{
    std::string filename;
    std::string directory = "music/";
    std::string fullname;
    std::string currentname = getConfig()->playSongName;
    std::string prevname = currentname;
    std::string nextname = currentname;

    bool hit = false;

    char **files= PHYSFS_enumerateFiles(directory.c_str());
    char **fptr=files;
    while(*fptr)
    {
        fullname = directory;
        fullname.append( *fptr );
        filename.assign( *fptr );
        
        if(!PHYSFS_isDirectory(fullname.c_str())){
            if( filename == currentname ){
                hit = true;
            } else if ( !hit ){
                prevname = filename; 
            } else {
                nextname = filename;
                break;
            }
        }
        fptr++;
    }
    PHYSFS_freeList(files);

    if(next){
        if( nextname != currentname){
            getSound()->playSound("Click");
            getSound()->playMusic(nextname);
        }
    } else {
        if( prevname != currentname){
            getSound()->playSound("Click");
            getSound()->playMusic(prevname);
        }
    }
    musicParagraph->setText(getConfig()->playSongName);
}
void
MainMenu::quitButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    quitState = QUIT;
    running = false;
}

void
MainMenu::switchMenu(Component* newMenu)
{
    currentMenu = dynamic_cast<Desktop*> (newMenu);
    if(!currentMenu)
        throw std::runtime_error("Menu Component is not a Desktop");
}

void
MainMenu::creditsButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadCreditsMenu();
    switchMenu(creditsMenu.get());
}

void
MainMenu::optionsButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadOptionsMenu();
    switchMenu(optionsMenu.get());
}

void
MainMenu::continueButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    quitState = INGAME;
    running = false;
}

void
MainMenu::newGameButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadNewGameMenu();
    switchMenu(newGameMenu.get());
}

void
MainMenu::loadGameButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadLoadGameMenu();
    switchMenu(loadGameMenu.get());
}

void
MainMenu::saveGameButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadSaveGameMenu();
    switchMenu(saveGameMenu.get());
}

void
MainMenu::creditsBackButtonClicked(Button* )
{
    getSound()->playSound("Click");
    loadMainMenu();
    switchMenu(mainMenu.get());
}

void
MainMenu::newGameStartButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    if( loadCityNG( mFilename ) ){
    	strcpy (given_scene, baseName.c_str());
        quitState = INGAME;
        running = false;
    }
    mFilename = "empty"; //don't erase scenarios later 
}

void
MainMenu::newGameStartBareButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    new_city( &main_screen_originx, &main_screen_originy, 0 );
    GameView* gv = getGameView();
    if( gv ){ gv->readOrigin(); }
    quitState = INGAME;
    running = false;
}

void
MainMenu::newGameStartVillageClicked(Button* )
{
    getSound()->playSound( "Click" );
    new_city( &main_screen_originx, &main_screen_originy, 1 );
    GameView* gv = getGameView();
    if( gv ){ gv->readOrigin(); }
    quitState = INGAME;
    running = false;
}

void
MainMenu::newGameBackButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadMainMenu();
    switchMenu(mainMenu.get());
}

void
MainMenu::loadGameBackButtonClicked(Button* )
{
    getSound()->playSound( "Click" );
    loadMainMenu();
    switchMenu(mainMenu.get());   
}

void
MainMenu::gotoMainMenu()
{
    getSound()->playSound( "Click" );
    loadMainMenu();
    switchMenu(mainMenu.get());   
}

void
MainMenu::loadGameLoadButtonClicked(Button *)
{
    getSound()->playSound( "Click" );
    if( loadCityNG( mFilename ) ){
        quitState = INGAME;
        running = false;
    }
}

void
MainMenu::loadGameSaveButtonClicked(Button *)
{
    getSound()->playSound( "Click" );
    if( file_exists( const_cast<char*>(mFilename.c_str()) ) ){
        std::cout << "remove( " << mFilename << ")\n";
        remove( mFilename.c_str() );
    }
    /* Build filename */
    std::stringstream newStart;
    newStart << slotNr << "_Y";
    newStart << std::setfill('0') << std::setw(5);
    fprintf(stderr,"total_time %i\n",total_time);
    newStart << total_time/1200;
    newStart << "_Tech";
    newStart << std::setfill('0') << std::setw(3);
    newStart << tech_level/10000;
    newStart << "_Cash";
    if (total_money >= 0)
    	newStart << "+";
    else
    	newStart << "-";
    newStart << std::setfill('0') << std::setw(3);
    int money = abs(total_money);
    if (money > 1000000000)
        newStart << money/1000000000 << "G";  
    else if (money > 1000000)
        newStart << money/1000000 << "M";  
    else  if(money > 1000)
        newStart << money/1000 << "K"; 
    else
        newStart << money/1 << "_";
 
    newStart << "_P";
    newStart << std::setfill('0') << std::setw(5);
    newStart << housed_population + people_pool;
    std::string newFilename( newStart.str() ); 
    saveCityNG( newFilename );
    fillLoadMenu( true );
}


MainState
MainMenu::run()
{
    SDL_Event event;
    running = true;
    quitState = QUIT;
    Uint32 ticks = SDL_GetTicks();
    
    int frame = 0;
    while(running) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_VIDEORESIZE:
                    initVideo(event.resize.w, event.resize.h);
                    currentMenu->resize(event.resize.w, event.resize.h);
                    break;
                case SDL_MOUSEMOTION:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_KEYDOWN:{
                    Event gui_event(event);
                    currentMenu->event(gui_event);
                    break;
                }
                case SDL_KEYUP: {
                    Event gui_event(event);
                    //In menu ESC as well as ^c exits the game.
                    //might come in handy if video-mode is not working as expected.
                    if( ( gui_event.keysym.sym == SDLK_ESCAPE ) ||
                        ( gui_event.keysym.sym == SDLK_c && ( gui_event.keysym.mod & KMOD_CTRL) ) ){
                        running = false;
                        quitState = QUIT;
                        break;
                    }
                    currentMenu->event(gui_event);
                    break;
                }
                case SDL_VIDEOEXPOSE:
                    currentMenu->resize( currentMenu->getWidth(), currentMenu->getHeight() );
                    break;
                case SDL_ACTIVEEVENT:
                    if( event.active.gain == 1 ){
                        currentMenu->resize( currentMenu->getWidth(), currentMenu->getHeight() );
                    }
                    break;
                case SDL_QUIT:
                    running = false;
                    quitState = QUIT;
                    break;
                default:
                    break;
            }
        }

        SDL_Delay(10); // give the CPU time to relax... (we are in main menu)

        if(currentMenu->needsRedraw()) {
            currentMenu->draw(*painter);
            flipScreenBuffer();
        } 

        frame++;
        if(SDL_GetTicks() - ticks > 1000) {
#ifdef DEBUG_FPS
            printf("FPS: %d.\n", frame);
#endif
            frame = 0;
            ticks = SDL_GetTicks();
        }
    }

    return quitState;
}
