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

#include <SDL.h>                        // for SDL_EventType, Uint32, SDL_Ge...
#include <stdio.h>                      // for fprintf, size_t, stderr
#include <algorithm>                    // for min, sort
#include <array>                        // for array
#include <cassert>                      // for assert
#include <chrono>                       // for operator<, time_point
#include <cstdlib>                      // for abs
#include <functional>                   // for function, bind, _1, _2
#include <initializer_list>             // for initializer_list
#include <iomanip>                      // for operator<<, setfill, setw
#include <iostream>                     // for basic_ostream, operator<<
#include <sstream>                      // for basic_stringstream
#include <stdexcept>                    // for invalid_argument, out_of_range
#include <utility>                      // for pair, move
#include <vector>                       // for vector
#include <optional>
#include <string>

#include "Config.hpp"                   // for getConfig, Config
#include "Game.hpp"                     // for Game
#include "MainLincity.hpp"              // for loadCityNG, saveCityNG
#include "Sound.hpp"                    // for getSound, Sound, MusicTransport
#include "Util.hpp"                     // for getCheckButton, getButton
#include "gui/Button.hpp"               // for Button
#include "gui/CheckButton.hpp"          // for CheckButton
#include "gui/Component.hpp"            // for Component
#include "gui/ComponentLoader.hpp"      // for loadGUIFile
#include "gui/Desktop.hpp"              // for Desktop
#include "gui/DialogBuilder.hpp"        // for DialogBuilder
#include "gui/Event.hpp"                // for Event
#include "gui/Painter.hpp"              // for Painter
#include "gui/Paragraph.hpp"            // for Paragraph
#include "gui/Signal.hpp"               // for Signal
#include "gui/SwitchComponent.hpp"      // for SwitchComponent
#include "gui/WindowManager.hpp"        // for WindowManager
#include "lincity/init_game.hpp"        // for _CitySettings, new_city, city...
#include "lincity/lintypes.hpp"         // for NUMOF_DAYS_IN_MONTH
#include "lincity/stats.hpp"            // for Stat, Stats
#include "lincity/world.hpp"            // for World
#include "main.hpp"                     // for resizeVideo, painter, videoSi...
#include "util/gettextutil.hpp"
#include "config.h"
#include "util/ptrutil.hpp"

using namespace std::placeholders;
using namespace std::string_literals;

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
}

MainMenu::~MainMenu()
{
}

void
MainMenu::loadMainMenu() {
  menu = dynamic_unique_cast<Desktop>(loadGUIFile("gui/mainmenu.xml"));
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

void
MainMenu::updateNewGameMenu() {
  std::filesystem::path scenarioDir = getConfig()->appDataDir.get() / "opening";
  CheckButton *button;

  button = getCheckButton(*newGameMenu, "File0");
  loadFiles.insert_or_assign(button, scenarioDir / "good_times.scn.gz");
  button->setCaptionText(_("Good Times"));

  button = getCheckButton(*newGameMenu, "File1");
  loadFiles.insert_or_assign(button, scenarioDir / "bad_times.scn.gz");
  button->setCaptionText(_("Bad Times"));

  button = getCheckButton(*newGameMenu, "File2");
  loadFiles.insert_or_assign(button, scenarioDir / "extreme_arid.scn.gz");
  button->setCaptionText(_("Extreme Arid"));

  button = getCheckButton(*newGameMenu, "File3");
  loadFiles.insert_or_assign(button, scenarioDir / "extreme_wetland.scn.gz");
  button->setCaptionText(_("Extreme Wetland"));

  button = getCheckButton(*newGameMenu, "File4");
  loadFiles.insert_or_assign(button, scenarioDir / "Beach.scn.gz");
  button->setCaptionText(_("Beach"));

  button = getCheckButton(*newGameMenu, "File5");
  loadFiles.insert_or_assign(button, scenarioDir / "Rocket_98.scn.gz");
  button->setCaptionText(_("Rocket 98"));
}

void MainMenu::updateLoadSaveMenus() {
  static const int buttonCount = 6;
  static const std::array<std::string, buttonCount> buttonNames = {
    "File0", "File1", "File2", "File3", "File4", "File5"
  };
  std::array<std::filesystem::file_time_type, buttonCount> fileTimes;
  for(auto& t : fileTimes) t = std::filesystem::file_time_type::min();

  for(const std::string& name : buttonNames) {
    getCheckButton(*loadGameMenu, name)->setCaptionText(_("empty"));
    getCheckButton(*saveGameMenu, name)->setCaptionText(_("empty"));
  }

  std::filesystem::path dir = getConfig()->userDataDir.get();
  for(auto& dirEnt : std::filesystem::directory_iterator(dir)) {
    std::string fName = dirEnt.path().filename().string();
    if(fName.substr(1, 1) != "_") continue;
    unsigned long i = 0;
    try { i = std::stoul(fName.substr(0,1)) - 1; }
      catch(const std::invalid_argument& ex) { continue; }
      catch(const std::out_of_range& ex) { continue; }
    if(i >= buttonCount) continue;
    if(!dirEnt.is_regular_file()) continue;
    if(dirEnt.last_write_time() < fileTimes[i]) continue;

    CheckButton *button = getCheckButton(*loadGameMenu, buttonNames[i]);
    loadFiles.insert_or_assign(button, dirEnt.path());
    button->setCaptionText(fName.c_str());
    getCheckButton(*saveGameMenu, buttonNames[i])
      ->setCaptionText(fName.c_str());
    fileTimes[i] = dirEnt.last_write_time();
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

  button = getCheckButton(*newGameMenu, "RiverDelta");
  button->setCaptionText(_("river delta"));
  button->clicked.connect(std::bind(&MainMenu::doubleClick, this, _1,
    [this](){ newGameStartButtonClicked(nullptr); }));
  newGameSelection.registerButton(button);

  button = getCheckButton(*newGameMenu, "DesertArea");
  button->setCaptionText(_("semi desert"));
  button->clicked.connect(std::bind(&MainMenu::doubleClick, this, _1,
    [this](){ newGameStartButtonClicked(nullptr); }));
  newGameSelection.registerButton(button);

  button = getCheckButton(*newGameMenu, "TemperateArea");
  button->setCaptionText(_("temperate"));
  button->clicked.connect(std::bind(&MainMenu::doubleClick, this, _1,
    [this](){ newGameStartButtonClicked(nullptr); }));
  newGameSelection.registerButton(button);

  button = getCheckButton(*newGameMenu, "SwampArea");
  button->setCaptionText(_("swamp"));
  button->clicked.connect(std::bind(&MainMenu::doubleClick, this, _1,
    [this](){ newGameStartButtonClicked(nullptr); }));
  newGameSelection.registerButton(button);

  for(const std::string& bName :
    {"File0", "File1", "File2", "File3", "File4", "File5"}
  ) {
    CheckButton *button = getCheckButton(*newGameMenu, bName);
    button->clicked.connect(std::bind(&MainMenu::doubleClick, this, _1,
      [this](){ newGameStartButtonClicked(nullptr); }));
    newGameSelection.registerButton(button);
  }

  // starting scenarios don't change, so we need only do this once
  updateNewGameMenu();
}

void
MainMenu::loadCreditsMenu() {
  creditsMenu = menuSwitch->findComponent("credits-menu");
  Button* backButton = getButton(*creditsMenu, "BackButton");
  backButton->clicked.connect(std::bind(&MainMenu::creditsBackButtonClicked, this, _1));
}

void
MainMenu::updateOptionsMenu() {
  //adjust checkbutton-states
  if(getConfig()->musicEnabled.get()) {
    getCheckButton(*optionsMenu, "BackgroundMusic")->check();
  } else {
    getCheckButton(*optionsMenu, "BackgroundMusic")->uncheck();
  }
  if(getConfig()->soundEnabled.get()) {
    getCheckButton(*optionsMenu, "SoundFX")->check();
  } else {
    getCheckButton(*optionsMenu, "SoundFX")->uncheck();
  }
  if(getConfig()->useFullScreen.get()) {
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
  if(getConfig()->useFullScreen.get()) {
    mode << "fullscreen";
  } else {
    mode << width << "x" << height;
  }
  getParagraph(*optionsMenu, "resolutionParagraph")->setText(mode.str());
  mode.str("");
  mode << getConfig()->worldSize.get();
  getParagraph( *optionsMenu, "WorldLenParagraph")->setText(mode.str());

#if ENABLE_NLS
  languageParagraph = getParagraph(*optionsMenu, "languageParagraph");
  currentLanguage = getConfig()->language.get();
  languageParagraph->setText(getConfig()->language.get());
  languages.clear();
  for(const auto& ent : std::filesystem::directory_iterator(
    getConfig()->appDataDir.get() / "locale")
  ) {
    if(!std::filesystem::exists(
      ent.path() / "LC_MESSAGES" / (PACKAGE_NAME + ".mo"s))
    ) continue;
    std::string lang = ent.path().filename().generic_string();
    languages.insert(lang);
#ifdef DEBUG
    std::cerr << "debug: detected available locale: " << lang << std::endl;
#endif
  }
  languages.insert("autodetect");
  languages.insert("en"); // English is the default when no translation is used
#endif
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
#if ENABLE_NLS
  currentCheckButton = getCheckButton(*optionsMenu, "LanguagePrev");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
  currentCheckButton = getCheckButton(*optionsMenu, "LanguageNext");
  currentCheckButton->clicked.connect(std::bind(&MainMenu::optionsMenuButtonClicked, this, _1, _2));
#endif
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

  for(const std::string& bName :
    {"File0", "File1", "File2", "File3", "File4", "File5"}
  ) {
    CheckButton *button = getCheckButton(*loadGameMenu, bName);
    button->clicked.connect(std::bind(&MainMenu::doubleClick, this, _1,
      [this](){ loadGameLoadButtonClicked(nullptr); }));
    loadGameSelection.registerButton(button);
  }
}

void
MainMenu::loadSaveGameMenu() {
  saveGameMenu = menuSwitch->findComponent("savegame-menu");

  // connect signals
  Button* saveButton = getButton(*saveGameMenu, "SaveButton");
  saveButton->clicked.connect(std::bind(&MainMenu::loadGameSaveButtonClicked, this, _1));
  Button* backButton = getButton(*saveGameMenu, "BackButton");
  backButton->clicked.connect(std::bind(&MainMenu::loadGameBackButtonClicked, this, _1));

  for(const std::string& bName :
    {"File0", "File1", "File2", "File3", "File4", "File5"}
  ) {
    CheckButton *button = getCheckButton(*saveGameMenu, bName);
    saveGameSelection.registerButton(button);
  }
}

void
MainMenu::switchMenu(Component *newMenu) {
  if(newMenu == newGameMenu) {
    // updateNewGameMenu();
  }
  else if(newMenu == loadGameMenu) {
    updateLoadSaveMenus();
  }
  else if(newMenu == saveGameMenu) {
    updateLoadSaveMenus();
  }
  else if(newMenu == optionsMenu) {
    updateOptionsMenu();
  }
  menuSwitch->switchComponent(newMenu->getName());
}

void
MainMenu::doubleClick(Component *button, std::function<void()> action) {
  Uint32 now = SDL_GetTicks();
  if(button == doubleClickButton && now - doubleClickTick < doubleClickTime) {
    doubleClickButton = nullptr;
    action();
  } else {
    doubleClickTick = now;
    doubleClickButton = button;
  }
}

void MainMenu::optionsMenuButtonClicked(CheckButton* button, int) {
    std::string buttonName = button->getName();
    if(buttonName == "BackgroundMusic") {
      getSound()->playSound("Click");
      getSound()->enableMusic(!getConfig()->musicEnabled.get());
      getConfig()->musicEnabled.sessionToConfig();
    } else if(buttonName == "MusicVolumePlus") {
      int volume = getConfig()->musicVolume.get();
      if(volume < 100) {
        if((volume += 5) > 100)
          volume = 100;
        getSound()->setMusicVolume(volume);
        getSound()->playSound("Click");
      }
      getConfig()->musicVolume.sessionToConfig();
    } else if(buttonName == "MusicVolumeMinus") {
      int volume = getConfig()->musicVolume.get();
      if(volume > 0) {
        if((volume -= 5) < 0)
          volume = 0;
        getSound()->setMusicVolume(volume);
        getSound()->playSound("Click");
      }
      getConfig()->musicVolume.sessionToConfig();
    } else if(buttonName == "SoundFX") {
      getConfig()->soundEnabled.session = !getConfig()->soundEnabled.get();
      getSound()->playSound("Click");
      getConfig()->soundEnabled.sessionToConfig();
    } else if(buttonName == "FXVolumePlus") {
      int volume = getConfig()->soundVolume.get();
      if(volume < 100) {
        if((volume += 5) > 100)
          volume = 100;
        getSound()->setSoundVolume(volume);
        getSound()->playSound("Click");
      }
      getConfig()->soundVolume.sessionToConfig();
    } else if(buttonName == "FXVolumeMinus") {
      int volume = getConfig()->soundVolume.get();
      if(volume > 0) {
        if((volume -= 5) < 0)
          volume = 0;
        getSound()->setSoundVolume(volume);
        getSound()->playSound("Click");
      }
      getConfig()->soundVolume.sessionToConfig();
    } else if(buttonName == "ResolutionPrev") {
      changeResolution(false);
      getConfig()->videoX.sessionToConfig();
      getConfig()->videoY.sessionToConfig();
    } else if(buttonName == "ResolutionNext") {
      changeResolution(true);
      getConfig()->videoX.sessionToConfig();
      getConfig()->videoY.sessionToConfig();
    } else if(buttonName == "WorldLenPrev") {
      changeWorldLen(false);
      getConfig()->worldSize.sessionToConfig();
    } else if(buttonName == "WorldLenNext") {
      changeWorldLen(true);
      getConfig()->worldSize.sessionToConfig();
#if ENABLE_NLS
    } else if(buttonName == "LanguagePrev") {
      changeLanguage(false);
      getConfig()->language.sessionToConfig();
    } else if(buttonName == "LanguageNext") {
      changeLanguage(true);
      getConfig()->language.sessionToConfig();
#endif
    } else if(buttonName == "Fullscreen") {
        getSound()->playSound("Click");
        getConfig()->useFullScreen.session = !getConfig()->useFullScreen.get();
        getConfig()->useFullScreen.sessionToConfig();
        getConfig()->save();
        resizeVideo(
          getConfig()->videoX.get(),
          getConfig()->videoY.get(),
          getConfig()->useFullScreen.get()
        );
        // switching to/from fullscreen may change the window size
        // that will be handled by a SDL_WINDOWEVENT_SIZE_CHANGED
    } else if(buttonName == "TrackPrev") {
        changeTrack(false);
    } else if(buttonName == "TrackNext") {
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
    if(getConfig()->useFullScreen.get()) {
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

    const int width = getConfig()->videoX.get();
    const int height = getConfig()->videoY.get();
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
    getConfig()->videoX.session = resolutions[new_mode].first;
    getConfig()->videoY.session = resolutions[new_mode].second;
}

void
MainMenu::changeWorldLen(bool next) {
  getConfig()->worldSize.session = getConfig()->worldSize.get() + (next?25:-25);
  getParagraph(*optionsMenu, "WorldLenParagraph")->setText(
    std::to_string(getConfig()->worldSize.get()));
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

#if ENABLE_NLS
void
MainMenu::changeLanguage(bool next) {
  std::set<std::string>::iterator i =
    languages.find(getConfig()->language.get());
  if(next) { // next language in set
    i++;
    if(i == languages.end()) {
      i = languages.begin();
    }
  } else { // previous
    if(i == languages.begin()) {
      i = languages.end();
    }
    i--;
  }

  std::string newLang = *i;
  languageParagraph->setText(newLang);
  getConfig()->language.session = newLang;
  setLang(newLang);
  getSound()->playSound("Click");
}
#endif

void
MainMenu::quitButtonClicked(Button *) {
  getSound()->playSound( "Click" );
  state = State::QUIT;
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
  state = State::GAME;

  if(!game) {
    std::unique_ptr<World> world;
    std::filesystem::path file =
      getConfig()->userDataDir.get() / "9_currentGameNG.scn.gz";
    if(std::filesystem::exists(file)) {
      world = loadCityNG(file);
    }
    else {
      city_settings city;
      city.with_village  = true;
      city.without_trees = false;

      //by default create a new City
      world = new_city(&city, getConfig()->worldSize.get());
    }

    if(world) {
      game.reset(new Game(window));
      game->setWorld(std::move(world));
    }
    else {
      state = State::MENU;
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
MainMenu::optionsBackButtonClicked(Button *) {
  getSound()->playSound("Click");
  getConfig()->save();
  int width = 0, height = 0;
  SDL_GetWindowSize(window, &width, &height);
  if(getConfig()->videoX.get() != width
    || getConfig()->videoY.get() != height
  ) {
    resizeVideo(getConfig()->videoX.get(), getConfig()->videoY.get(),
      getConfig()->useFullScreen.get());
  }
#if ENABLE_NLS
  else if(currentLanguage != getConfig()->language.get())
  {
    // TODO: re-parse GUI to update translatable text
    DialogBuilder()
      .titleText(_("Warning"))
      .messageAddTextBold(_("Restart Required"))
      .messageAddText(_("Changing the language requires restarting LinCity"
        " for changes to take full effect."))
      .imageFile("images/gui/dialogs/warning.png")
      .buttonSet(DialogBuilder::ButtonSet::OK)
      .build();
  }
#endif
  gotoMainMenu();
}

/**
 * Either create selected random terrain or load a scenario.
**/
void
MainMenu::newGameStartButtonClicked(Button *) {
  CheckButton *sel = newGameSelection.getSelection();
  if(!sel) return;

  getSound()->playSound("Click");

  std::unique_ptr<World> world;
  auto fileIt = loadFiles.find(sel);
  if(fileIt == loadFiles.end()) {
    const std::string& bName = sel->getName();
    city_settings settings;
    settings.with_village =
      getCheckButton(*newGameMenu, "WithVillage")->isChecked();
    settings.without_trees =
      getCheckButton(*newGameMenu, "WithoutTrees")->isChecked();
    if(bName == "RiverDelta") {
      world = new_city(&settings, getConfig()->worldSize.get());
    } else if(bName == "DesertArea") {
      world = new_desert_city(&settings, getConfig()->worldSize.get());
    } else if(bName == "TemperateArea") {
      world = new_temperate_city(&settings, getConfig()->worldSize.get());
    } else if(bName == "SwampArea") {
      world = new_swamp_city(&settings, getConfig()->worldSize.get());
    }
    assert(world);
  }
  else {
    world = loadCityNG(fileIt->second);
  }

  if(world) {
    if(!game)
      game.reset(new Game(window));
    game->setWorld(std::move(world));
    state = State::GAME;
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
MainMenu::loadGameLoadButtonClicked(Button *) {
  CheckButton *sel = loadGameSelection.getSelection();
  if(!sel) return;
  auto fileIt = loadFiles.find(sel);
  if(fileIt == loadFiles.end()) return; // empty slot selected
  getSound()->playSound("Click");
  if(std::unique_ptr<World> world = loadCityNG(fileIt->second)) {
    if(!game) game.reset(new Game(window));
    game->setWorld(std::move(world));
    state = State::GAME;
  }
}

void
MainMenu::loadGameSaveButtonClicked(Button *) {
  static const std::array<std::string, 6> buttonNames = {
    "File0", "File1", "File2", "File3", "File4", "File5"
  };

  CheckButton *sel = saveGameSelection.getSelection();
  if(!sel) return;

  int i;
  for(i = 0; buttonNames[i] != sel->getName(); i++)
    if(i == buttonNames.size() - 1) { assert(false); return; }

  assert(!!game);
  World& world = game->getWorld();

  getSound()->playSound( "Click" );

  /* Build filename */
  std::stringstream filename;
  filename << (i + 1) << "_Y";
  filename << std::setfill('0') << std::setw(5);
  fprintf(stderr,"total_time %i\n", world.total_time);
  filename << world.total_time/1200;
  filename << "_Tech";
  filename << std::setfill('0') << std::setw(3);
  filename << world.tech_level/10000;
  filename << "_Cash";
  if(world.total_money >= 0)
    filename << "+";
  else
    filename << "-";
  filename << std::setfill('0') << std::setw(3);
  int money = abs(world.total_money);
  if (money > 1000000000)
    filename << money/1000000000 << "G";
  else if (money > 1000000)
    filename << money/1000000 << "M";
  else  if(money > 1000)
    filename << money/1000 << "K";
  else
    filename << money << "_";

  filename << "_P";
  filename << std::setfill('0') << std::setw(5);
  filename << world.stats.population.population_m / NUMOF_DAYS_IN_MONTH;
  filename << ".gz";

  saveCityNG(world, getConfig()->userDataDir.get() / filename.str());

  // TODO: remove the old save file, but only if the save was successful

  gotoMainMenu();
}

void
MainMenu::run() {
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
    state = State::MENU;
    while(state == State::MENU) {
        next_task = std::min({next_gui, next_fps});
        while(true) {
            SDL_Event event;
            int event_timeout = next_task - SDL_GetTicks();
            if(event_timeout < 0) event_timeout = 0;
            int status = SDL_WaitEventTimeout(&event, event_timeout);
            if(!status) break; // timed out

            switch(event.type) {
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        videoSizeChanged(event.window.data1, event.window.data2);
                        menu->resize(event.window.data1, event.window.data2);
                        getConfig()->videoX.session = event.window.data1;
                        getConfig()->videoY.session = event.window.data2;
                        getConfig()->videoX.sessionToConfig();
                        getConfig()->videoY.sessionToConfig();

                        if(menuSwitch->getActiveComponent() == optionsMenu) {
                            std::stringstream mode;
                            mode.str("");
                            if (getConfig()->useFullScreen.get()) {
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
                        state = State::QUIT;
                        break;
                    }
                    menu->event(gui_event);
                    break;
                }
                case SDL_QUIT:
                    state = State::QUIT;
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

        if(state == State::GAME) {
          launchGame();
        }
    }
}

void
MainMenu::launchGame() {
  assert(game);
  game->run();
  state = State::MENU;
  switchMenu(mainMenu);
  DialogBuilder::setDefaultWindowManager(dynamic_cast<WindowManager *>(
    menu->findComponent("windowManager")));
}

/** @file lincity-ng/MainMenu.cpp */
