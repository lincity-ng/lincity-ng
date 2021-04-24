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
#include "gui/Painter.hpp"
#include "gui/callback/Callback.hpp"

#include "gui_interface/shared_globals.h"
#include "lincity/fileutil.h"

#include "CheckButton.hpp"

#include "MainMenu.hpp"
#include "Util.hpp"
#include "Config.hpp"
#include "Sound.hpp"
#include "GameView.hpp"
#include "Game.hpp"
#include "MainLincity.hpp"
#include "readdir.hpp"

#include "tinygettext/gettext.hpp"

#include "lincity/init_game.h"

extern std::string autoLanguage;

MainMenu::MainMenu(SDL_Window* _window)
    : window(_window)
{
    loadMainMenu();
    switchMenu(mainMenu.get());
    baseName = "";
    lastClickTick = 0;
    doubleClickButtonName = "";
    mFilename = "";
    baseName = "";
}

MainMenu::~MainMenu()
{
}

void
MainMenu::loadMainMenu()
{
    if(mainMenu.get() == 0)
    {
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

    int width = 0, height = 0;
    SDL_GetWindowSize(window, &width, &height);
    mainMenu->resize(width, height);
}

void MainMenu::fillNewGameMenu()
{
#if 1
  //Do not remove it!
  //It is need for localization
  //
  // If you going to remove this, you must
  // get names from data/opening/*scn files,
  // and update messages.pot from script.
  (void)N_("Rocket_98");
  (void)N_("Beach");
  (void)N_("bad_times");
  (void)N_("extreme_arid");
  (void)N_("extreme_wetland");
  (void)N_("good_times");
#endif
  const std::string buttonNames[]={"File0","File1","File2","File3","File4","File5"};

  char **files= PHYSFS_enumerateFiles("opening");

  char **fptr=files;

  CheckButton *button;

  fileMap.clear();

  for(int i=0;i<6;i++)
  {
    button=getCheckButton(*newGameMenu.get(),buttonNames[i]);

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

  button=getCheckButton(*newGameMenu.get(),"WithVillage");
  button->check();
  //button->setCaptionText(_("random empty board"));
  //button->clicked.connect(makeCallback(*this,&MainMenu::selectLoadGameButtonClicked));

  button=getCheckButton(*newGameMenu.get(),"RiverDelta");
  button->setCaptionText(_("river delta"));
  button->clicked.connect(makeCallback(*this,&MainMenu::selectLoadGameButtonClicked));

  button=getCheckButton(*newGameMenu.get(),"DesertArea");
  button->setCaptionText(_("semi desert"));
  button->clicked.connect(makeCallback(*this,&MainMenu::selectLoadGameButtonClicked));

  button=getCheckButton(*newGameMenu.get(),"TemperateArea");
  button->setCaptionText(_("temperate"));
  button->clicked.connect(makeCallback(*this,&MainMenu::selectLoadGameButtonClicked));

  button=getCheckButton(*newGameMenu.get(),"SwampArea");
  button->setCaptionText(_("swamp"));
  button->clicked.connect(makeCallback(*this,&MainMenu::selectLoadGameButtonClicked));

  return;
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
/*#ifdef DEBUG
                        fprintf(stderr," %s is more recent than previous %s\n",
                                          curfile, recentfile);
#endif*/
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
        startButton->clicked.connect(makeCallback(*this, &MainMenu::newGameStartButtonClicked));

        Button* backButton = getButton(*newGameMenu, "BackButton");
        backButton->clicked.connect(makeCallback(*this, &MainMenu::newGameBackButtonClicked));


        fillNewGameMenu();
    }
    int width = 0, height = 0;
    SDL_GetWindowSize(window, &width, &height);
    newGameMenu->resize(width, height);
}

void
MainMenu::loadCreditsMenu()
{
    if(creditsMenu.get() == 0)
    {
        creditsMenu.reset(loadGUIFile("gui/credits.xml"));
        Button* backButton = getButton(*creditsMenu, "BackButton");
        backButton->clicked.connect(
                makeCallback(*this, &MainMenu::creditsBackButtonClicked));
    }
    int width = 0, height = 0;
    SDL_GetWindowSize(window, &width, &height);
    creditsMenu->resize(width, height);
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
        currentCheckButton = getCheckButton(*optionsMenu, "ResolutionPrev");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "ResolutionNext");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "WorldLenPrev");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "WorldLenNext");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "LanguagePrev");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "LanguageNext");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "BinaryMode");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));
        currentCheckButton = getCheckButton(*optionsMenu, "SeedMode");
        currentCheckButton->clicked.connect( makeCallback(*this, &MainMenu::optionsMenuButtonClicked));

        Button* currentButton = getButton(*optionsMenu, "BackButton");
        currentButton->clicked.connect( makeCallback(*this, &MainMenu::optionsBackButtonClicked));
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
    if (binary_mode)
    {   getCheckButton(*optionsMenu, "BinaryMode")->check();}
    else
    {   getCheckButton(*optionsMenu, "BinaryMode")->uncheck();}
    if (seed_compression)
    {   getCheckButton(*optionsMenu, "SeedMode")->check();}
    else
    {   getCheckButton(*optionsMenu, "SeedMode")->uncheck();}
    //current background track
    musicParagraph = getParagraph( *optionsMenu, "musicParagraph");
    musicParagraph->setText(getSound()->currentTrack.title);


    int width = 0, height = 0;
    SDL_GetWindowSize(window, &width, &height);

    std::stringstream mode;
    if (getConfig()->useFullScreen) {
        mode << "fullscreen";
    } else {
        mode << width << "x" << height;
    }
    getParagraph( *optionsMenu, "resolutionParagraph")->setText(mode.str());
    mode.str("");
    mode << world.len();
    getParagraph( *optionsMenu, "WorldLenParagraph")->setText(mode.str());
    languageParagraph = getParagraph( *optionsMenu, "languageParagraph");
    currentLanguage = getConfig()->language;
    languageParagraph->setText( getConfig()->language );
    languages = dictionaryManager->get_languages();
    languages.insert( "autodetect" );
    languages.insert( "en" ); // English is the default when no translation is used
    optionsMenu->resize(getConfig()->videoX, getConfig()->videoY); //(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
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
    loadGameMenu->resize(getConfig()->videoX, getConfig()->videoY); //(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
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
    saveGameMenu->resize(getConfig()->videoX, getConfig()->videoY); //(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
}


void
MainMenu::selectLoadGameButtonClicked(CheckButton* button, int i){
    selectLoadSaveGameButtonClicked( button , i,  false );
}

void
MainMenu::selectSaveGameButtonClicked(CheckButton* button, int i){
    selectLoadSaveGameButtonClicked( button , i, true );
}

/**
 * Handle RadioButtons in load, save and new game dialog
 */
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

    if( newGameMenu.get()==currentMenu ) {
        const std::string rnd[]={"RiverDelta","DesertArea","TemperateArea","SwampArea",""};
        for(int i=0;std::string(rnd[i]).length();i++) {
            CheckButton *b=getCheckButton(*currentMenu,rnd[i]);
            if(b->getName()!=button->getName()){
                b->uncheck();
            } else {
                b->check();
                fc = rnd[i];
            }
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
    } else if( buttonName == "ResolutionPrev"){
        changeResolution(false);
    } else if( buttonName == "ResolutionNext"){
        changeResolution(true);
    } else if( buttonName == "WorldLenPrev"){
        changeWorldLen(false);
    } else if( buttonName == "WorldLenNext"){
        changeWorldLen(true);
    } else if( buttonName == "LanguagePrev"){
        changeLanguage(false);
    } else if( buttonName == "LanguageNext"){
        changeLanguage(true);
    } else if( buttonName == "Fullscreen"){
        getSound()->playSound("Click");
        getConfig()->useFullScreen = !getConfig()->useFullScreen;
        getConfig()->save();
        if( getConfig()->restartOnChangeScreen )
        {
            quitState = RESTART;
            running = false;
        }
        else
        {
            int width = 0, height = 0;
            SDL_GetWindowSize(window, &width, &height);
            resizeVideo(width, height, getConfig()->useFullScreen);
            // switching to fullscreen may change window size again
            SDL_GetWindowSize(window, &width, &height);
            currentMenu->resize(width, height);
            loadOptionsMenu();
        }
    } else if( buttonName == "TrackPrev"){
        changeTrack(false);
    } else if( buttonName == "TrackNext"){
        changeTrack(true);
    } else if( buttonName == "BinaryMode"){
        binary_mode = !binary_mode;
    } else if( buttonName == "SeedMode"){
        seed_compression = !seed_compression;
    } else {
        std::cerr << "MainMenu::optionsMenuButtonClicked " << buttonName << " unknown Button!\n";
    }
}

/** Changes the displayed resolution in the options menu.
This does not actually change the resolution. initVideo has to be called to do this.
@param next if true change to the next resolution in the list; otherwise change to the previous one
@todo sort modes before in ascending order and remove unsupported modes like 640x480
*/
void MainMenu::changeResolution(bool next) {
    if (getConfig()->useFullScreen) {
        /* Resolution changes have no effect in desktop fullscreen mode */
        return;
    }

    // Create a list of candidate resolutions, including a few fallbacks in
    // case the window system doesn't provide any
    std::vector<std::pair<int, int>> resolutions;
    resolutions.push_back(std::pair<int, int>(800,600));
    resolutions.push_back(std::pair<int, int>(1024,768));
    resolutions.push_back(std::pair<int, int>(1280,1024));

    int display = SDL_GetWindowDisplayIndex(window);
    int nmodes = SDL_GetNumDisplayModes(display);
    for (int i = -1; i < nmodes; ++i) {
        SDL_DisplayMode mode;
        if (i >= 0) {
            if (SDL_GetDisplayMode(display, i, &mode) < 0) {
                std::cerr << "Error: SDL failed to get mode " << i << " for display " << display << "!\n";
                continue;
            }
        } else {
            /* Special case: half the current display size */
            if (SDL_GetCurrentDisplayMode(display, &mode) < 0) {
                std::cerr << "Error: SDL failed to get current mode for display " << display << "!\n";
                continue;
            }
            mode.w /= 2;
            mode.h /= 2;
        }
        bool in_list = false;
        for (size_t j = 0; j < resolutions.size(); j++) {
            if (resolutions[j].first == mode.w && resolutions[j].second == mode.h) {
                in_list = true;
                break;
            }
        }
        if (!in_list) {
            resolutions.push_back(std::pair<int, int>(mode.w, mode.h));
        }
    }
    std::sort(resolutions.begin(), resolutions.end());

    const int width = getConfig()->videoX;
    const int height = getConfig()->videoY;
    int closest_mode = 0;
    int min_cost = 1000000000;
    for (size_t i = 0; i < resolutions.size(); ++i) {
        int cost = 2 * abs(resolutions[i].first - width) + abs(resolutions[i].second - height);
        if (cost < min_cost) {
            closest_mode = int(i);
            min_cost = cost;
        }
    }

    std::string currentMode = getParagraph( *optionsMenu, "resolutionParagraph")->getText();

    std::stringstream mode;
    mode.str("");
    mode << resolutions[closest_mode].first << "x" << resolutions[closest_mode].second;

    int new_mode = closest_mode + (next ? 1 : -1);
    /* Wrap around */
    if (new_mode < 0) {
        new_mode = int(resolutions.size()) - 1;
    } else if (new_mode >= int(resolutions.size())) {
        new_mode = 0;
    }

    mode.str("");
    mode << resolutions[new_mode].first << "x" << resolutions[new_mode].second;

    getSound()->playSound("Click");
    getParagraph( *optionsMenu, "resolutionParagraph")->setText(mode.str());
    getConfig()->videoX = resolutions[new_mode].first;
    getConfig()->videoY = resolutions[new_mode].second;
}

void
MainMenu::changeWorldLen(bool next)
{
    std::ostringstream os;
    int new_len;
    new_len = world.len()+(next?25:-25);
    world.len(new_len);
    os << world.len();
    getParagraph( *optionsMenu, "WorldLenParagraph")->setText(os.str());
}

void
MainMenu::changeTrack( bool next)
{
    if(next){
        getSound()->playSound("Click");
        getSound()->changeTrack(NEXT_TRACK);

    } else {
        getSound()->playSound("Click");
        getSound()->changeTrack(PREV_TRACK);
    }
    musicParagraph->setText(getSound()->currentTrack.title);
}

void
MainMenu::changeLanguage( bool next)
{
    std::set<std::string>::iterator i = languages.find( getConfig()->language );
    if( next ){ // next language in set
        i++;
        if( i == languages.end() ){
            i = languages.begin();
        }
    } else { // previous
        if( i == languages.begin() ){
            i = languages.end();
        }
        i--;
    }

    std::string newLang = *i;
    languageParagraph->setText( newLang );
    getConfig()->language = newLang;
    getSound()->playSound("Click");
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
    {   throw std::runtime_error("Menu Component is not a Desktop");}
    int width = 0, height = 0;
    SDL_GetWindowSize(window, &width, &height);
    currentMenu->resize(width, height);
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
    //only act if world is still clean
    if (!world.dirty)
    {
        //load current game if it exists
        if( ! loadCityNG( std::string("9_currentGameNG.scn.gz") )  &&
            ! loadCityNG( std::string("9_currentGameNG.scn") ) )
        {
            city_settings  city;
            city.with_village  = true;
            city.without_trees = false;

            //by default create a new City
            new_city( &main_screen_originx, &main_screen_originy, &city);
        }
    }
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
    if(getGame())
    {
        loadSaveGameMenu();
        switchMenu(saveGameMenu.get());
    }
}

void
MainMenu::creditsBackButtonClicked(Button* )
{
    getSound()->playSound("Click");
    loadMainMenu();
    switchMenu(mainMenu.get());
}

void
MainMenu::optionsBackButtonClicked(Button* )
{
    getConfig()->save();
    int width = 0, height = 0;
    SDL_GetWindowSize(window, &width, &height);
    if( getConfig()->videoX != width || getConfig()->videoY != height )
    {
        if( getConfig()->restartOnChangeScreen )
        {
            quitState = RESTART;
            running = false;
        }
        else
        {
            resizeVideo( getConfig()->videoX, getConfig()->videoY, getConfig()->useFullScreen);
            gotoMainMenu();
        }
    }
    else if( currentLanguage != getConfig()->language )
    {
        unsetenv("LINCITY_LANG");
        quitState = RESTART;
        running = false;
    }
    else
    {
        gotoMainMenu();
    }
}

/**
 * Either create selected random terrain or load a scenario.
 **/
void
MainMenu::newGameStartButtonClicked(Button* )
{
    if( mFilename.empty() ){
        // std::cout << "nothing selected\n";
        return;
    }
    getSound()->playSound( "Click" );

    city_settings  city_obj;
    city_settings *city=&city_obj;

    city->with_village  = (getCheckButton(*currentMenu,"WithVillage" )->state == CheckButton::STATE_CHECKED);
    city->without_trees = (getCheckButton(*currentMenu,"WithoutTrees")->state == CheckButton::STATE_CHECKED);

    if( baseName == "RiverDelta" ){
        new_city( &main_screen_originx, &main_screen_originy, city);
        GameView* gv = getGameView();
        if( gv ){ gv->readOrigin(); }
        quitState = INGAME;
        running = false;
    } else if( baseName == "DesertArea" ){
        new_desert_city( &main_screen_originx, &main_screen_originy, city);
        GameView* gv = getGameView();
        if( gv ){ gv->readOrigin(); }
        quitState = INGAME;
        running = false;
    } else if( baseName == "TemperateArea" ){
        new_temperate_city( &main_screen_originx, &main_screen_originy, city);
        GameView* gv = getGameView();
        if( gv ){ gv->readOrigin(); }
        quitState = INGAME;
        running = false;
    } else if( baseName == "SwampArea" ){
        new_swamp_city( &main_screen_originx, &main_screen_originy, city);
        GameView* gv = getGameView();
        if( gv ){ gv->readOrigin(); }
        quitState = INGAME;
        running = false;
    } else {
        if( loadCityNG( mFilename ) ){
            strcpy (given_scene, baseName.c_str());
            quitState = INGAME;
            running = false;
        }
    }
    mFilename = "empty"; //don't erase scenarios later
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
    {   newStart << "+";}
    else
    {   newStart << "-";}
    newStart << std::setfill('0') << std::setw(3);
    int money = abs(total_money);
    if (money > 1000000000)
    {   newStart << money/1000000000 << "G";}
    else if (money > 1000000)
    {   newStart << money/1000000 << "M";}
    else  if(money > 1000)
    {   newStart << money/1000 << "K";}
    else
    {   newStart << money << "_";}

    newStart << "_P";
    newStart << std::setfill('0') << std::setw(5);
    newStart << housed_population + people_pool;
    std::string newFilename( newStart.str() );
    saveCityNG( newFilename );
    fillLoadMenu( true );
    gotoMainMenu();
}


MainState
MainMenu::run()
{
    SDL_Event event;
    running = true;
    quitState = QUIT;
    Uint32 fpsTicks = SDL_GetTicks();
    Uint32 lastticks = fpsTicks;
    Uint32 lastRedrawTicks = fpsTicks;
    int frame = 0;
    while(running)
    {
        if(SDL_WaitEventTimeout(&event, 100))
        {
            switch(event.type) {
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        videoSizeChanged(event.window.data1, event.window.data2);
                        currentMenu->resize(event.window.data1, event.window.data2);
                        getConfig()->videoX = event.window.data1;
                        getConfig()->videoY = event.window.data2;

                        if(currentMenu == optionsMenu.get())//update resolution display
                        {
                            std::stringstream mode;
                            mode.str("");
                            if (getConfig()->useFullScreen) {
                                mode << "fullscreen";
                            } else {
                                mode << event.window.data1 << "x" << event.window.data2;
                            }
                            getParagraph( *optionsMenu, "resolutionParagraph")->setText(mode.str());
                        }
                    }
                    break;
                case SDL_MOUSEMOTION:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEWHEEL:
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
                case SDL_QUIT:
                    running = false;
                    quitState = QUIT;
                    break;
                default:
                    break;
            }
        }

        // create update Event
        Uint32 ticks = SDL_GetTicks();
        float elapsedTime = ((float) (ticks - lastticks)) / 1000.0;
        currentMenu->event(Event(elapsedTime));
        lastticks = ticks;

        /* We unconditionally redraw every ~100 ms as workaround for an SDL bug
         * under Wayland, in which window resizing is not communicated to the
         * program until it redraws the window. When this bug is no longer
         * present, this behavior should be safe to remove */
        if (ticks - lastRedrawTicks > 90) {
             currentMenu->reLayout();
        }

        if(currentMenu->needsRedraw()) {
            currentMenu->draw(*painter);
            painter->updateScreen();
            lastRedrawTicks = ticks;
        }

        frame++;
        if(ticks - fpsTicks > 1000) {
#ifdef DEBUG_FPS
            printf("MainMenu FPS: %d.\n", (frame*1000) / (ticks - fpsTicks));
#endif
            frame = 0;
            fpsTicks = ticks;
        }
    }

    return quitState;
}

/** @file lincity-ng/MainMenu.cpp */

