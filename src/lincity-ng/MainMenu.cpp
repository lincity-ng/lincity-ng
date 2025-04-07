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

#include "MainMenu.hpp"

#include <SDL.h>                           // for SDL_GetTicks, SDL_GetWindo...
#include <assert.h>                        // for assert
#include <physfs.h>                        // for PHYSFS_enumerateFiles, PHY...
#include <stdio.h>                         // for fprintf, remove, size_t
#include <stdlib.h>                        // for abs, atoi, unsetenv
#include <string.h>                        // for strcpy
#include <algorithm>                       // for sort
#include <functional>                      // for bind, _1, function, _2
#include <iomanip>                         // for operator<<, setfill, setw
#include <iostream>                        // for basic_ostream, operator<<
#include <sstream>                         // for basic_stringstream, basic_...
#include <utility>                         // for pair
#include <vector>                          // for vector

#include "Config.hpp"                      // for getConfig, Config
#include "Game.hpp"                        // for getGame
#include "MainLincity.hpp"                 // for loadCityNG, saveCityNG
#include "Sound.hpp"                       // for getSound, Sound, MusicTran...
#include "Util.hpp"                        // for getCheckButton, getButton
#include "gui/Button.hpp"                  // for Button
#include "gui/CheckButton.hpp"             // for CheckButton
#include "gui/Component.hpp"               // for Component
#include "gui/ComponentLoader.hpp"         // for loadGUIFile
#include "gui/Desktop.hpp"                 // for Desktop
#include "gui/DialogBuilder.hpp"           // for DialogBuilder
#include "gui/Event.hpp"                   // for Event
#include "gui/Painter.hpp"                 // for Painter
#include "gui/Paragraph.hpp"               // for Paragraph
#include "gui/Signal.hpp"                  // for Signal
#include "gui/SwitchComponent.hpp"         // for SwitchComponent
#include "gui/WindowManager.hpp"           // for WindowManager
#include "gui_interface/shared_globals.h"  // for main_screen_originx, main_...
#include "lincity/engglobs.h"              // for world, total_money, total_...
#include "lincity/init_game.h"             // for new_city, city_settings
#include "lincity/world.h"                 // for World
#include "tinygettext/gettext.hpp"         // for _, N_, dictionaryManager
#include "tinygettext/tinygettext.hpp"     // for DictionaryManager

using namespace std::placeholders;

extern std::string autoLanguage;

MainMenu::MainMenu(SDL_Window* _window)
    : window(_window)
{
    loadMainMenu();
    loadNewGameMenu();
    loadLoadGameMenu();
    loadSaveGameMenu();
    loadCreditsMenu();
    loadOptionsMenu();
    switchMenu(mainMenu);

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
MainMenu::loadMainMenu() {
  menu.reset(dynamic_cast<Desktop *>(loadGUIFile("gui/mainmenu.xml")));
  menuSwitch = dynamic_cast<SwitchComponent *>(
    menu->findComponent("menu-switch"));
  assert(menuSwitch);

  mainMenu = menuSwitch->findComponent("main-menu");
  // connect signals
  Button* quitButton = getButton(*mainMenu, "QuitButton");
  quitButton->clicked.connect(std::bind(&MainMenu::quitButtonClicked, this, _1));
  Button* continueButton = getButton(*mainMenu, "ContinueButton");
  continueButton->clicked.connect(std::bind(&MainMenu::continueButtonClicked, this, _1));
  Button* newGameButton = getButton(*mainMenu, "NewGameButton");
  newGameButton->clicked.connect(std::bind(&MainMenu::newGameButtonClicked, this, _1));
  Button* loadGameButton = getButton(*mainMenu, "LoadButton");
  loadGameButton->clicked.connect(std::bind(&MainMenu::loadGameButtonClicked, this, _1));
  Button* saveGameButton = getButton(*mainMenu, "SaveButton");
  saveGameButton->clicked.connect(std::bind(&MainMenu::saveGameButtonClicked, this, _1));
  Button* creditsButton = getButton(*mainMenu, "CreditsButton");
  creditsButton->clicked.connect(std::bind(&MainMenu::creditsButtonClicked, this, _1));
  Button* optionsButton = getButton(*mainMenu, "OptionsButton");
  optionsButton->clicked.connect(std::bind(&MainMenu::optionsButtonClicked, this, _1));
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
    button = getCheckButton(*newGameMenu, buttonNames[i]);

    while(*fptr)
    {
      if(std::string(*fptr).find(".scn.gz")!=std::string::npos)
        break;
      fptr++;
    }
    if(*fptr)
    {
      std::string f=*fptr;
      if(f.length()>7){
        f=f.substr(0,f.length()-7); // truncate .scn.gz
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
            button = getCheckButton(*saveGameMenu, buttonNames[i]);
        } else {
            button = getCheckButton(*loadGameMenu, buttonNames[i]);
        }
        //make sure Button is connected only once
        button->clicked.clear();
        if( save )
            button->clicked.connect(std::bind(&MainMenu::selectSaveGameButtonClicked, this, _1, _2));
        else {
            button->clicked.connect(std::bind(&MainMenu::selectLoadGameButtonClicked, this, _1, _2));
        }
        for(char** i = rc; *i != 0; i++){
            curfile = *i;
            if(std::string( curfile ).find( filestart.str() ) != 0 ) {
                // && !( curfile->d_type & DT_DIR  ) ) is not portable. So
                // don't create a directoy named 2_ in a savegame-directory or
                // you can no longer load from slot 2.
                continue;
            }
            PHYSFS_Stat stat;
            int err = PHYSFS_stat(curfile, &stat);
            if(!err) {
                std::cerr << "could not stat file: " <<
                    curfile << std::endl;
                continue;
            }
            if (!recentfile || stat.modtime > t) {
                recentfile = curfile;
                t = stat.modtime;
            }
        }
#ifdef DEBUG
        fprintf(stderr,"Most recent file: %s\n\n",recentfile);
#endif

        if(recentfile) {
            std::string f = recentfile;
            button->setCaptionText(f);
        } else {
            button->setCaptionText(_("empty"));
        }
    }
}

void
MainMenu::loadNewGameMenu() {
  newGameMenu = menuSwitch->findComponent("newgame-menu");

  // connect signals
  Button* startButton = getButton(*newGameMenu, "StartButton");
  startButton->clicked.connect(std::bind(&MainMenu::newGameStartButtonClicked, this, _1));

  Button* backButton = getButton(*newGameMenu, "BackButton");
  backButton->clicked.connect(std::bind(&MainMenu::newGameBackButtonClicked, this, _1));

  CheckButton *button;
  button = getCheckButton(*newGameMenu,"WithVillage");
  button->check();
  //button->setCaptionText(_("random empty board"));
  //button->clicked.connect(std::bind(&MainMenu::selectLoadGameButtonClicked, this, _1, _2));

  button = getCheckButton(*newGameMenu,"RiverDelta");
  button->setCaptionText(_("river delta"));
  button->clicked.connect(std::bind(&MainMenu::selectLoadGameButtonClicked, this, _1, _2));

  button = getCheckButton(*newGameMenu,"DesertArea");
  button->setCaptionText(_("semi desert"));
  button->clicked.connect(std::bind(&MainMenu::selectLoadGameButtonClicked, this, _1, _2));

  button = getCheckButton(*newGameMenu,"TemperateArea");
  button->setCaptionText(_("temperate"));
  button->clicked.connect(std::bind(&MainMenu::selectLoadGameButtonClicked, this, _1, _2));

  button = getCheckButton(*newGameMenu,"SwampArea");
  button->setCaptionText(_("swamp"));
  button->clicked.connect(std::bind(&MainMenu::selectLoadGameButtonClicked, this, _1, _2));

  button = getCheckButton(*newGameMenu, "File0");
  button->clicked.connect(std::bind(&MainMenu::selectLoadGameButtonClicked, this, _1, _2));

  button = getCheckButton(*newGameMenu, "File1");
  button->clicked.connect(std::bind(&MainMenu::selectLoadGameButtonClicked, this, _1, _2));

  button = getCheckButton(*newGameMenu, "File2");
  button->clicked.connect(std::bind(&MainMenu::selectLoadGameButtonClicked, this, _1, _2));

  button = getCheckButton(*newGameMenu, "File3");
  button->clicked.connect(std::bind(&MainMenu::selectLoadGameButtonClicked, this, _1, _2));

  button = getCheckButton(*newGameMenu, "File4");
  button->clicked.connect(std::bind(&MainMenu::selectLoadGameButtonClicked, this, _1, _2));

  button = getCheckButton(*newGameMenu, "File5");
  button->clicked.connect(std::bind(&MainMenu::selectLoadGameButtonClicked, this, _1, _2));
}

void
MainMenu::loadCreditsMenu() {
  creditsMenu = menuSwitch->findComponent("credits-menu");
  Button* backButton = getButton(*creditsMenu, "BackButton");
  backButton->clicked.connect(std::bind(&MainMenu::creditsBackButtonClicked, this, _1));
}

void
MainMenu::fillOptionsMenu() {
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
}

void
MainMenu::loadOptionsMenu() {
  optionsMenu = menuSwitch->findComponent("options-menu");
  CheckButton* currentCheckButton = getCheckButton(*optionsMenu, "BackgroundMusic");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "SoundFX");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "Fullscreen");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "MusicVolumePlus");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "MusicVolumeMinus");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "FXVolumePlus");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "FXVolumeMinus");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "TrackPrev");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "TrackNext");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "ResolutionPrev");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "ResolutionNext");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "WorldLenPrev");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "WorldLenNext");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "LanguagePrev");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "LanguageNext");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  // currentCheckButton = getCheckButton(*optionsMenu, "BinaryMode");
  // currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  // currentCheckButton = getCheckButton(*optionsMenu, "SeedMode");
  // currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));

  Button* currentButton = getButton(*optionsMenu, "BackButton");
  currentButton->clicked.connect(std::bind(&MainMenu::optionsBackButtonClicked, this, _1));
}

void
MainMenu::loadLoadGameMenu() {
  loadGameMenu = menuSwitch->findComponent("loadgame-menu");

  // connect signals
  Button* loadButton = getButton(*loadGameMenu, "LoadButton");
  loadButton->clicked.connect(std::bind(&MainMenu::loadGameLoadButtonClicked, this, _1));
  Button* backButton = getButton(*loadGameMenu, "BackButton");
  backButton->clicked.connect(std::bind(&MainMenu::loadGameBackButtonClicked, this, _1));
}

void
MainMenu::loadSaveGameMenu() {
  saveGameMenu = menuSwitch->findComponent("savegame-menu");

  // connect signals
  Button* saveButton = getButton(*saveGameMenu, "SaveButton");
  saveButton->clicked.connect(std::bind(&MainMenu::loadGameSaveButtonClicked, this, _1));
  Button* backButton = getButton(*saveGameMenu, "BackButton");
  backButton->clicked.connect(std::bind(&MainMenu::loadGameBackButtonClicked, this, _1));
}

void
MainMenu::switchMenu(Component *newMenu) {
  if(newMenu == newGameMenu) {
    fillNewGameMenu();
  }
  else if(newMenu == loadGameMenu) {
    fillLoadMenu();
  }
  else if(newMenu == saveGameMenu) {
    fillLoadMenu(true);
  }
  else if(newMenu == optionsMenu) {
    fillOptionsMenu();
  }
  menuSwitch->switchComponent(newMenu->getName());
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
    Component *currentMenu = menuSwitch->getActiveComponent();
    std::string fc=button->getCaptionText();
    if(newGameMenu == currentMenu) {
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

    if(newGameMenu == currentMenu) {
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
    if(newGameMenu == currentMenu) {
        file=std::string("opening/")+fc+".scn.gz";
    } else {
        file=fc;
    }

    mFilename="";
    if(newGameMenu != currentMenu) {
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
        if(newGameMenu == currentMenu) {
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
            resizeVideo(
              getConfig()->videoX,
              getConfig()->videoY,
              getConfig()->useFullScreen
            );
            // switching to/from fullscreen may change the window size
            // that will be handled by a SDL_WINDOWEVENT_SIZE_CHANGED
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
MainMenu::creditsButtonClicked(Button* ) {
  getSound()->playSound("Click");
  switchMenu(creditsMenu);
}

void
MainMenu::optionsButtonClicked(Button* ) {
  getSound()->playSound("Click");
  switchMenu(optionsMenu);
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
        if(!loadCityNG("9_currentGameNG.scn.gz")) {
            city_settings  city;
            city.with_village  = true;
            city.without_trees = false;

            //by default create a new City
            new_city( &main_screen_originx, &main_screen_originy, &city);
        }
    }
}

void
MainMenu::newGameButtonClicked(Button* ) {
  getSound()->playSound("Click");
  switchMenu(newGameMenu);
}

void
MainMenu::loadGameButtonClicked(Button* ) {
  getSound()->playSound("Click");
  switchMenu(loadGameMenu);
}

void
MainMenu::saveGameButtonClicked(Button* ) {
  getSound()->playSound( "Click" );
  if(getGame()) {
    switchMenu(saveGameMenu);
  }
}

void
MainMenu::creditsBackButtonClicked(Button* ) {
  getSound()->playSound("Click");
  switchMenu(mainMenu);
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
#if defined (WIN32)
        _putenv_s("LINCITY_LANG", "");
#else
        unsetenv("LINCITY_LANG");
#endif
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

    city_settings city_obj;
    city_settings *city = &city_obj;

    city->with_village =
      getCheckButton(*newGameMenu, "WithVillage")->isChecked();
    city->without_trees =
      getCheckButton(*newGameMenu, "WithoutTrees")->isChecked();

    if( baseName == "RiverDelta" ){
        new_city( &main_screen_originx, &main_screen_originy, city);
        quitState = INGAME;
        running = false;
    } else if( baseName == "DesertArea" ){
        new_desert_city( &main_screen_originx, &main_screen_originy, city);
        quitState = INGAME;
        running = false;
    } else if( baseName == "TemperateArea" ){
        new_temperate_city( &main_screen_originx, &main_screen_originy, city);
        quitState = INGAME;
        running = false;
    } else if( baseName == "SwampArea" ){
        new_swamp_city( &main_screen_originx, &main_screen_originy, city);
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
MainMenu::newGameBackButtonClicked(Button* ) {
  getSound()->playSound("Click");
  switchMenu(mainMenu);
}

void
MainMenu::loadGameBackButtonClicked(Button* ) {
  getSound()->playSound("Click");
  switchMenu(mainMenu);
}

void
MainMenu::gotoMainMenu() {
  getSound()->playSound("Click");
  switchMenu(mainMenu);
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
    std::cout << "remove( " << mFilename << ")\n";
    remove( mFilename.c_str() );
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
    std::string newFilename( newStart.str() + ".gz" );
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
    DialogBuilder::setDefaultWindowManager(dynamic_cast<WindowManager *>(
      menu->findComponent("windowManager")));

    {
      int width, height;
      SDL_GetWindowSize(window, &width, &height);
      menu->resize(width, height);
    }
    int frame = 0;
    Uint32 next_gui = 0, next_fps = 0;
    __attribute__((unused))
    Uint32 prev_gui = 0, prev_fps = 0;
    Uint32 next_task;
    Uint32 tick = 0;
    while(running) {
        next_task = std::min({next_gui, next_fps});
        while(true) {
            int event_timeout = next_task - SDL_GetTicks();
            if(event_timeout < 0) event_timeout = 0;
            int status = SDL_WaitEventTimeout(&event, event_timeout);
            if(!status) break; // timed out

            switch(event.type) {
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        videoSizeChanged(event.window.data1, event.window.data2);
                        menu->resize(event.window.data1, event.window.data2);
                        getConfig()->videoX = event.window.data1;
                        getConfig()->videoY = event.window.data2;

                        if(menuSwitch->getActiveComponent() == optionsMenu) {
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
                    menu->event(gui_event);
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
                    menu->event(gui_event);
                    break;
                }
                case SDL_QUIT:
                    running = false;
                    quitState = QUIT;
                    break;
                default:
                    break;
            }

            if(menu->needsRedraw())
              next_task = tick;
        }

        tick = SDL_GetTicks();
        frame++;

        if(tick >= next_gui) { // gui update
            // fire update event
            menu->event(Event((tick - prev_gui) / 1000.0f));

            next_gui = tick + 1000/10; // 10 FPS
            prev_gui = tick;
        }

        if(tick >= next_fps) {
#ifdef DEBUG_FPS
            printf("MainMenu FPS: %d.\n", (frame*1000) / (tick - prev_fps));
#endif
            frame = 0;
            next_fps = tick + 1000;
            prev_fps = tick;
        }

        if(menu->needsRedraw()) {
            menu->draw(*painter);
            painter->updateScreen();
        }
    }

    return quitState;
}

/** @file lincity-ng/MainMenu.cpp */
