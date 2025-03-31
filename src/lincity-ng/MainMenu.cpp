/* ---------------------------------------------------------------------- *
 * src/lincity-ng/MainMenu.cpp
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

#include "MainMenu.hpp"

#include <SDL.h>                           // for SDL_GetWindowSize, SDL_Get...
#include <stdio.h>                         // for fprintf, size_t, stderr
#include <string.h>                        // for strcpy
#include <algorithm>                       // for sort
#include <cassert>
#include <chrono>                          // for operator>
#include <cstdlib>                         // for abs, unsetenv
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
#include "lincity/init_game.h"             // for _CitySettings, new_city
#include "lincity/world.h"                 // for Map
#include "tinygettext/gettext.hpp"         // for _, N_, dictionaryManager
#include "tinygettext/tinygettext.hpp"     // for DictionaryManager
#include "lincity/lintypes.h"

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
    doubleClickButton = nullptr;
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
  std::filesystem::path scenarioDir = getConfig()->appDataDir / "opening";
  std::filesystem::directory_iterator dirIt(scenarioDir);

  CheckButton *button;

  for(int i=0;i<6;i++)
  {
    button = getCheckButton(*newGameMenu, buttonNames[i]);

    std::filesystem::path file;
    while(dirIt != std::filesystem::end(dirIt)) {
      std::string fName = dirIt->path().filename().string();
      if(fName.substr(fName.size() - 7) == ".scn.gz"
        && dirIt->is_regular_file()
      ) {
        file = *(dirIt++);
        break;
      }
      dirIt++;
    }
    button->clicked.connect(std::bind(&MainMenu::selectLoadSaveGameButtonClicked, this, _1, _2, false, file));
    if(!file.empty())
      button->setCaptionText(_(file.stem().stem().string().c_str()));
    else
      button->setCaptionText(_("empty"));
  }

  return;
}

void MainMenu::fillLoadMenu( bool save /*= false*/ )
{
  const std::string buttonNames[]={"File0","File1","File2","File3","File4","File5"};

  char* curfile;
  CheckButton *button;

  for(int i=0;i<6;i++) {
    std::filesystem::path recentfile;
    std::filesystem::file_time_type t;

    std::string filestart = std::to_string(i+1) + "_";
    button = getCheckButton(*(save?saveGameMenu:loadGameMenu),buttonNames[i]);

    for(auto& dirEnt : std::filesystem::directory_iterator(
      getConfig()->userDataDir)
    ) {
      std::string fName = dirEnt.path().filename().string();
      if(fName.substr(0, filestart.size()) == filestart
        && dirEnt.is_regular_file()
        && (dirEnt.last_write_time() > t || recentfile.empty())
      ) {
        recentfile = dirEnt;
        t = dirEnt.last_write_time();
      }
    }

    // FIXME: this will cause problems with RadioButtonGroup
    button->clicked.clear();
    button->clicked.connect(std::bind(&MainMenu::selectLoadSaveGameButtonClicked, this, _1, _2, save, recentfile));
    button->setCaptionText(recentfile.empty() ? _("empty") :
      _(recentfile.filename().string().c_str()));
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
  button->clicked.connect(std::bind(&MainMenu::selectLoadSaveGameButtonClicked,
    this, _1, _2, false, std::filesystem::path()));

  button = getCheckButton(*newGameMenu,"DesertArea");
  button->setCaptionText(_("semi desert"));
  button->clicked.connect(std::bind(&MainMenu::selectLoadSaveGameButtonClicked,
    this, _1, _2, false, std::filesystem::path()));

  button = getCheckButton(*newGameMenu,"TemperateArea");
  button->setCaptionText(_("temperate"));
  button->clicked.connect(std::bind(&MainMenu::selectLoadSaveGameButtonClicked,
    this, _1, _2, false, std::filesystem::path()));

  button = getCheckButton(*newGameMenu,"SwampArea");
  button->setCaptionText(_("swamp"));
  button->clicked.connect(std::bind(&MainMenu::selectLoadSaveGameButtonClicked,
    this, _1, _2, false, std::filesystem::path()));
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
  mode << getConfig()->worldSize;
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

/**
 * Handle RadioButtons in load, save and new game dialog
 */
void
MainMenu::selectLoadSaveGameButtonClicked(CheckButton* button, int /* btn */,
  bool save, std::filesystem::path file
) {
    mFilename = file;
    baseName = "";

    Component *currentMenu = menuSwitch->getActiveComponent();
    const std::string bs[]={"File0","File1","File2","File3","File4","File5",""};
    for(int i=0;std::string(bs[i]).length();i++) {
        CheckButton *b=getCheckButton(*currentMenu,bs[i]);
        if(b != button){
            b->uncheck();
        } else {
            b->check();
            if(newGameMenu != currentMenu)
              slotNr = i + 1;
        }
    }

    if(newGameMenu == currentMenu) {
        const std::string rnd[]={"RiverDelta","DesertArea","TemperateArea","SwampArea",""};
        for(int i=0;std::string(rnd[i]).length();i++) {
            CheckButton *b=getCheckButton(*currentMenu,rnd[i]);
            if(b != button){
                b->uncheck();
            } else {
                b->check();
                baseName = rnd[i];
            }
        }
    }

    Uint32 now = SDL_GetTicks();
    //doubleclick on Filename loads File
    if( ( button == doubleClickButton ) &&  ( !save ) &&
            ( now - lastClickTick < doubleClickTime ) ) {

        lastClickTick = 0;
        doubleClickButton = nullptr;
        if(newGameMenu == currentMenu) {
            //load scenario
            newGameStartButtonClicked( 0 );
        } else {
            //load game
            loadGameLoadButtonClicked( 0 );
        }
    } else {
        lastClickTick = now;
        doubleClickButton = button;
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
MainMenu::changeWorldLen(bool next) {
  getConfig()->worldSize += (next?25:-25);
  getParagraph(*optionsMenu, "WorldLenParagraph")->setText(
    std::to_string(getConfig()->worldSize));
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
MainMenu::continueButtonClicked(Button* ) {
  getSound()->playSound( "Click" );
  quitState = INGAME;

  if(!game) {
    std::unique_ptr<World> world;
    std::filesystem::path file =
      getConfig()->userDataDir / "9_currentGameNG.scn.gz";
    if(std::filesystem::exists(file)) {
      world = loadCityNG(file);
    }
    else {
      city_settings city;
      city.with_village  = true;
      city.without_trees = false;

      //by default create a new City
      world = new_city(&city, getConfig()->worldSize);
    }

    if(world) {
      game.reset(new Game(window));
      game->setWorld(std::move(world));
    }
    else {
      quitState = MAINMENU;
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
  if(game) {
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
    getSound()->playSound( "Click" );
    getConfig()->save();
    int width = 0, height = 0;
    SDL_GetWindowSize(window, &width, &height);
    if( getConfig()->videoX != width || getConfig()->videoY != height )
    {
        if( getConfig()->restartOnChangeScreen )
        {
            quitState = RESTART;
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
MainMenu::newGameStartButtonClicked(Button* ) {
  if(mFilename.empty()) {
    // std::cout << "nothing selected\n";
    return;
  }
  getSound()->playSound("Click");

  city_settings city_obj;
  city_settings *city = &city_obj;

  city->with_village =
    getCheckButton(*newGameMenu, "WithVillage")->isChecked();
  city->without_trees =
    getCheckButton(*newGameMenu, "WithoutTrees")->isChecked();

  std::unique_ptr<World> world;
  if(baseName == "RiverDelta") {
    world = new_city(city, getConfig()->worldSize);
  } else if(baseName == "DesertArea") {
    world = new_desert_city(city, getConfig()->worldSize);
  } else if(baseName == "TemperateArea") {
    world = new_temperate_city(city, getConfig()->worldSize);
  } else if(baseName == "SwampArea") {
    world = new_swamp_city(city, getConfig()->worldSize);
  } else {
    if(world = loadCityNG(mFilename)) {
      world->given_scene = baseName; // TODO: move this to backend
    }
  }

  if(world) {
    if(!game)
      game.reset(new Game(window));
    game->setWorld(std::move(world));

    quitState = INGAME;
  }
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
  switchMenu(mainMenu);
}

void
MainMenu::loadGameLoadButtonClicked(Button *)
{
    getSound()->playSound( "Click" );
    if(mFilename.empty())
      return;
    if(std::unique_ptr<World> world = loadCityNG(mFilename)) {
      if(!game) game.reset(new Game(window));
      game->setWorld(std::move(world));
      quitState = INGAME;
    }
}

void
MainMenu::loadGameSaveButtonClicked(Button *)
{
    getSound()->playSound( "Click" );
    assert(!!game);
    World& world = game->getWorld();
    std::cout << "remove( " << mFilename << ")\n";
    std::filesystem::remove(mFilename);
    /* Build filename */
    std::stringstream newStart;
    newStart << slotNr << "_Y";
    newStart << std::setfill('0') << std::setw(5);
    fprintf(stderr,"total_time %i\n", world.total_time);
    newStart << world.total_time/1200;
    newStart << "_Tech";
    newStart << std::setfill('0') << std::setw(3);
    newStart << world.tech_level/10000;
    newStart << "_Cash";
    if (world.total_money >= 0)
    {   newStart << "+";}
    else
    {   newStart << "-";}
    newStart << std::setfill('0') << std::setw(3);
    int money = abs(world.total_money);
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
    newStart << world.stats.population.population_m / NUMOF_DAYS_IN_MONTH;
    std::string newFilename( newStart.str() + ".gz" );
    saveCityNG(world, getConfig()->userDataDir / newFilename);
    fillLoadMenu( true );
    gotoMainMenu();
}


MainState
MainMenu::run()
{
    SDL_Event event;
    quitState = MAINMENU;
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
    while(quitState == MAINMENU) {
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
                        quitState = QUIT;
                        break;
                    }
                    menu->event(gui_event);
                    break;
                }
                case SDL_QUIT:
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

        while(quitState == INGAME) {
          launchGame();
        }
    }

    return quitState;
}

void
MainMenu::launchGame() {
  assert(!!game);
  quitState = game->run();
  switchMenu(mainMenu);
}

/** @file lincity-ng/MainMenu.cpp */
