/* ---------------------------------------------------------------------- *
 * src/lincity-ng/Game.cpp
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

#include "Game.hpp"

#include <SDL.h>                            // for SDL_KeyCode, Uint32, SDL_...
#include <assert.h>                         // for assert
#include <fmt/format.h>                     // for format, native_formatter:...
#include <stddef.h>                         // for NULL
#include <algorithm>                        // for min
#include <filesystem>                       // for path, operator/, director...
#include <functional>                       // for function, bind, _1
#include <initializer_list>                 // for initializer_list
#include <iostream>                         // for basic_ostream, operator<<
#include <list>                             // for list
#include <optional>                         // for optional
#include <stdexcept>                        // for runtime_error
#include <typeinfo>                         // for type_info
#include <utility>                          // for move

#include "ButtonPanel.hpp"                  // for ButtonPanel
#include "Config.hpp"                       // for getConfig, Config
#include "Dialog.hpp"                       // for Dialog, closeAllDialogs
#include "EconomyGraph.hpp"                 // for EconomyGraph
#include "GameView.hpp"                     // for GameView
#include "HelpWindow.hpp"                   // for HelpWindow
#include "MainLincity.hpp"                  // for saveCityNG, loadCityNG
#include "MiniMap.hpp"                      // for MiniMap
#include "Mps.hpp"                          // for MpsMap, MpsFinance
#include "PBar.hpp"                         // for LCPBar
#include "Sound.hpp"                        // for getSound, Sound
#include "TimerInterface.hpp"               // for get_real_time_with
#include "Util.hpp"                         // for getButton
#include "gui/Button.hpp"                   // for Button
#include "gui/Component.hpp"                // for Component
#include "gui/ComponentLoader.hpp"          // for loadGUIFile
#include "gui/Desktop.hpp"                  // for Desktop
#include "gui/DialogBuilder.hpp"            // for DialogBuilder
#include "gui/Event.hpp"                    // for Event
#include "gui/Painter.hpp"                  // for Painter
#include "gui/Signal.hpp"                   // for Signal
#include "gui/WindowManager.hpp"            // for WindowManager
#include "lincity/MapPoint.hpp"             // for MapPoint, operator<<
#include "lincity/groups.hpp"               // for GROUP_MARKET, GROUP_MONUMENT
#include "lincity/lin-city.hpp"             // for MAX_TECH_LEVEL, ANIMATE_D...
#include "lincity/lintypes.hpp"             // for ConstructionGroup, Constr...
#include "lincity/messages.hpp"             // for dynamic_message_cast, Mes...
#include "lincity/modules/all_modules.hpp"  // for RocketPad, ParklandConstr...
#include "lincity/world.hpp"                // for World, Map, MapTile
#include "main.hpp"                         // for painter, videoSizeChanged
#include "util/gettextutil.hpp"             // for _
#include "util/ptrutil.hpp"                 // for dynamic_unique_cast

using namespace std::placeholders;

Game::Game(SDL_Window *window) : window(window) {
  loadGui();
}

Game::~Game() = default;

void Game::showHelpWindow( std::string topic ){
    helpWindow->showTopic( topic );
}

void Game::backToMainMenu(){
    closeAllDialogs();
    saveCityNG(*world,
      getConfig()->userDataDir.get() / "9_currentGameNG.scn.gz");
    running = false;
}

void Game::quickLoad(){
  closeAllDialogs();

  //load file
  getGameView().printStatusMessage("quick load...");
  std::string filename("quicksave.scn.gz");
  if(std::unique_ptr<World> world =
    loadCityNG(getConfig()->userDataDir.get() / filename)
  ) {
    setWorld(std::move(world));
    getGameView().printStatusMessage("quick load successful.");
  } else {
    getGameView().printStatusMessage("quick load failed!");
  }
}

void Game::quickSave(){
  //save file
  getGameView().printStatusMessage("quick save...");
  saveCityNG(*world, getConfig()->userDataDir.get() / "quicksave.scn.gz");
}

void Game::testAllHelpFiles(){
  getGameView().printStatusMessage("Testing Help Files...");

  std::filesystem::path dir = getConfig()->appDataDir.get() / "help" / "en";
  for(auto& dirEntry : std::filesystem::directory_iterator(dir)) {
    if(!dirEntry.is_regular_file()) continue;
    std::cerr << "--- Examining " << dirEntry.path().stem() << "\n";
    helpWindow->showTopic(dirEntry.path().stem().string());
  }
}

void
Game::loadGui() {
  gui = dynamic_unique_cast<Desktop>(loadGUIFile("gui/app.xml"));

  gameview = dynamic_cast<GameView *>(gui->findComponent("GameView"));
  minimap = dynamic_cast<MiniMap *>(gui->findComponent("MiniMap"));
  buttonpanel =
    dynamic_cast<ButtonPanel *>(gui->findComponent("ButtonPanel"));
  economygraph =
    dynamic_cast<EconomyGraph *>(gui->findComponent("EconomyGraph"));
  windowmanager = dynamic_cast<WindowManager *>(
    gui->findComponent("windowManager"));
  mpsmap = dynamic_cast<MpsMap *>(gui->findComponent("MapMPS"));
  mpsfinance = dynamic_cast<MpsFinance *>(gui->findComponent("GlobalMPS"));
  pbar1 = dynamic_cast<LCPBar *>(gui->findComponent("PBar"));
  pbar2 = dynamic_cast<LCPBar *>(gui->findComponent("PBar2nd"));
  gameview->setGame(this);
  minimap->setGame(this);
  buttonpanel->setGame(this);
  economygraph->setGame(this);
  mpsfinance->setGame(this);
  pbar1->setGame(this);
  pbar2->setGame(this);

  buttonpanel->selected.connect(std::bind(&Game::setUserOperation, this, _1));

  Button* gameMenu = getButton(*gui, "GameMenuButton");
  gameMenu->clicked.connect(std::bind(&Game::backToMainMenu, this));

  Button* helpButton = getButton(*gui, "HelpButton");
  helpButton->clicked.connect([this](Button *) {
    helpWindow->showTopic("help");
  });

  Button* statButton = getButton(*gui, "StatButton");
  statButton->clicked.connect([this](Button *) {
    new Dialog(*this, GAME_STATS);
  });

  helpWindow.reset(new HelpWindow(windowmanager));

  int width = 0, height = 0;
  SDL_GetWindowSize(window, &width, &height);
  gui->resize(width, height);
}

World&
Game::getWorld() const {
  assert(world);
  return *world;
}

void
Game::setWorld(std::unique_ptr<World>&& world) {
  this->world = std::move(world);

  for(auto w : {&warnBullWater, &warnBullShanty, &warnBullMonument})
    w->onAccept.clear();
  getButtonPanel().selectQueryTool();
  getButtonPanel().updateTech();
  getMpsMap().query(nullptr);
  gameview->show(this->world->map.recentPoint);
  getSound()->setTechLevel(this->world->tech_level);

  inventions.assign({
    &potteryConstructionGroup,       //  0.0
    &monumentConstructionGroup,      //  0.0
    &waterwellConstructionGroup,     //  0.0
    &communeConstructionGroup,       //  0.0
    &organic_farmConstructionGroup,  //  0.0
    &trackConstructionGroup,         //  0.0
    // &residenceLLConstructionGroup,   //  0.0
    // &residenceMLConstructionGroup,   //  0.0
    &residenceHLConstructionGroup,   //  0.0
    &oremineConstructionGroup,       //  0.0
    &tipConstructionGroup,           //  0.0
    &marketConstructionGroup,        //  0.0
    &schoolConstructionGroup,        //  0.1
    &parklandConstructionGroup,      //  0.2
    // &parkpondConstructionGroup       //  0.2
    &blacksmithConstructionGroup,    //  0.3
    &cricketConstructionGroup,       //  1.2
    &fireStationConstructionGroup,   //  2.2
    &millConstructionGroup,          //  2.5
    &windmillConstructionGroup,      //  3.0
    &portConstructionGroup,          //  3.5
    &roadConstructionGroup,          //  5.0
    &coalmineConstructionGroup,      //  8.5
    &healthCentreConstructionGroup,  // 11.0
    &universityConstructionGroup,    // 15.0
    &industryLightConstructionGroup, // 16.0
    &industryHeavyConstructionGroup, // 17.0
    &railConstructionGroup,          // 18.0
    &coal_powerConstructionGroup,    // 20.0
    // &substationConstructionGroup,    // 20.0
    // &powerlineConstructionGroup,     // 20.0
    &recycleConstructionGroup,       // 23.2
    // &residenceLHConstructionGroup,   // 30.0
    // &residenceMHConstructionGroup,   // 30.0
    &residenceHHConstructionGroup,   // 30.0
    &windpowerConstructionGroup,     // 45.0
    &solarPowerConstructionGroup,    // 50.0
    &rocketPadConstructionGroup,     // 75.0
  });
  inventions.sort(
    [](const ConstructionGroup *a, const ConstructionGroup *b)
    { return a->tech < b->tech; }
  );
  while(!inventions.empty()
    && inventions.front()->tech <= this->world->tech_level
  )
    inventions.pop_front();
}

void
Game::updateTech() {
  while(!inventions.empty()
    && inventions.front()->tech <= world->tech_level
  ) {
    const ConstructionGroup& cstGrp = *inventions.front();
    DialogBuilder()
      .titleText(_("New Invention!"))
      .messageAddTextBold(fmt::format(_("{} Available"), _(cstGrp.name_plural)))
      .messageAddText(fmt::format(_("You have reached the tech level which"
          " enables you to build {}. See the help screen for more information"
          " about this type of building."),
        _(cstGrp.name_plural)))
      .imageFile("images/gui/dialogs/invention.png")
      .buttonSet(DialogBuilder::ButtonSet::OK)
      .build();
    inventions.pop_front();
  }
}

UserOperation&
Game::getUserOperation() {
  return userOperation;
}

const UserOperation&
Game::getUserOperation() const {
  return userOperation;
}

void
Game::setUserOperation(const UserOperation& op) {
  userOperation = op;

  for(auto w : {&warnBullWater, &warnBullShanty, &warnBullMonument})
    w->accepted = false;;
  getGameView().setCursorSize(userOperation.cursorSize());
  getGameView().showToolInfo();
}

void
Game::executeUserOperation(MapPoint point) {
  try {
    if(!world->map.is_visible(point))
      OutsideMapMessage::create(point)->throwEx();

    switch(userOperation.action) {
    case UserOperation::ACTION_QUERY: {
      // TODO: do dialogs on double-click, not 'second-click'
      if(getMpsMap().tile == world->map(point)) {
        Construction *cst = world->map(point)->reportingConstruction;
        switch(world->map(point)->getGroup()) {
        case GROUP_MARKET:
          new Dialog(*this, EDIT_MARKET, cst->point);
          break;
        case GROUP_PORT:
          new Dialog(*this, EDIT_PORT, cst->point);
          break;
        case GROUP_ROCKET:
          RocketPad *rocket = dynamic_cast<RocketPad *>(cst);
          assert(rocket);
          if(rocket->stage == RocketPad::AWAITING) {
            new Dialog(*this, ASK_LAUNCH_ROCKET, cst->point);
          }
          break;
        }
      }

      getMpsMap().query(world->map(point));
      getMiniMap().switchView("MapMPS");

      break;
    }

    case UserOperation::ACTION_BUILD: {
      // TODO: move dependence on SDL_GetKeyboardState elsewhere (GameView?)
      if(userOperation.constructionGroup == &parklandConstructionGroup
        && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_K]
      )
        userOperation.constructionGroup = &parkpondConstructionGroup;

      break;
    }

    case UserOperation::ACTION_BULLDOZE: {
      if(Message::ptr msg; !userOperation.isAllowedHere(*world, point, msg)) {
        // don't issue a warning if we can't bulldoze anyway
        break;
      }

      unsigned short grp = world->map(point)->getGroup();
      decltype(warnBullWater) *warnStatus = nullptr;
      switch(grp) {
      case GROUP_MONUMENT:
        warnStatus = &warnBullMonument;
        break;
      case GROUP_RIVER:
        warnStatus = &warnBullWater;
        break;
      case GROUP_SHANTY:
        warnStatus = &warnBullShanty;
        break;
      }
      if(!warnStatus || warnStatus->accepted)
        break;

      Construction *cst = world->map(point)->reportingConstruction;
      warnStatus->onAccept.connect([this, point, cst, grp]() {
        // make sure things haven't changed
        // there must be a better way to do this
        if(world->map(point)->reportingConstruction == cst
          && world->map(point)->getGroup() == grp
        ) {
          try{
            userOperation.execute(*world, point);
            getMpsMap().setTile(world->map(point));
            getSound()->playSound("Raze");
          } catch(const Message::Exception& ex) {
            handleMessage(ex.getMessage());
          }
        }
      });

      if(warnStatus->pending)
        return;

      DialogBuilder()
        .titleText("Warning")
        .messageAddTextBold("Warning:")
        .messageAddText(fmt::format(_("Bulldozing a {} costs a lot of money."),
          _(world->map(point)->getConstructionGroup()->name)))
        .messageAddText("Want to bulldoze?")
        .imageFile("images/gui/dialogs/warning.png")
        // TODO: use "Bulldoze"/"Leave It" buttons
        .buttonSet(DialogBuilder::ButtonSet::YESNO)
        .onYes([warnStatus]() {
          warnStatus->pending = false;
          warnStatus->accepted = true;
          warnStatus->onAccept();
        })
        .onNo([warnStatus]() {
          warnStatus->pending = false;
          warnStatus->onAccept.clear();
        })
        .build();
      warnStatus->pending = true;

      return;
    }

    case UserOperation::ACTION_EVACUATE:
    case UserOperation::ACTION_FLOOD:
      break;

    default: {
      assert(false);
    }
    }

    userOperation.execute(*world, point);

    getMpsMap().setTile(world->map(point));
    if(userOperation.action != UserOperation::ACTION_BULLDOZE)
      getSound()->playSound(*world->map(point));
    else
      getSound()->playSound("Raze");
  } catch(const Message::Exception& ex) {
    handleMessage(ex.getMessage());
  }
}

Component&
Game::getGui() const {
  return *gui;
}

WindowManager&
Game::getWindowManager() const {
  return *windowmanager;
}

GameView&
Game::getGameView() const {
  assert(gameview);
  return *gameview;
}

MiniMap&
Game::getMiniMap() const {
  assert(minimap);
  return *minimap;
}

EconomyGraph&
Game::getEconomyGraph() const {
  assert(economygraph);
  return *economygraph;
}

MpsMap&
Game::getMpsMap() const {
  return *mpsmap;
}

MpsFinance&
Game::getMpsFinance() const {
  return *mpsfinance;
}

LCPBar&
Game::getPBar1() const {
  return *pbar1;
}

LCPBar&
Game::getPBar2() const {
  return *pbar2;
}

ButtonPanel&
Game::getButtonPanel() const {
  return *buttonpanel;
}


void
Game::run() {
    assert(world);
    if(!world) return;

    // init GUI
    Desktop* desktop = dynamic_cast<Desktop*> (gui.get());
    if(!desktop)
      throw std::runtime_error("Toplevel component is not a Desktop");
    gui->resize(getConfig()->videoX.get(), getConfig()->videoY.get());
    DialogBuilder::setDefaultWindowManager(dynamic_cast<WindowManager *>(
      desktop->findComponent("windowManager")));
    world->setUpdated(World::Updatable::MONEY);

    getButtonPanel().selectQueryTool();

    int frame = 0;
    bool new_day = true, new_month = true, new_year = true;
    Uint32 next_execute = ~0, next_animate = ~0, next_gui = 0, next_fps = 0;
    [[maybe_unused]]
    Uint32 prev_execute = 0, prev_animate = 0, prev_gui = 0, prev_fps = 0;
    Uint32 next_task;

    running = true;
    while(true) {
        next_task = std::min({next_execute, next_animate, next_gui, next_fps});
        while(true) {
            if(!running) return;
            SDL_Event event;
            int event_timeout = next_task - SDL_GetTicks();
            if(event_timeout < 0) event_timeout = 0;
            int status = SDL_WaitEventTimeout(&event, event_timeout);
            if(!status) break; // timed out

            switch(event.type) {
                case SDL_WINDOWEVENT:
                    switch(event.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        videoSizeChanged(event.window.data1, event.window.data2);
                        gui->resize(event.window.data1, event.window.data2);
                        getConfig()->videoX.session = event.window.data1;
                        getConfig()->videoY.session = event.window.data2;
                        getConfig()->videoX.sessionToConfig();
                        getConfig()->videoY.sessionToConfig();
                        break;
                    case SDL_WINDOWEVENT_ENTER:
                    case SDL_WINDOWEVENT_LEAVE:
                        Event gui_event(event);
                        gui->event(gui_event);
                        break;
                    }
                    break;
                case SDL_KEYUP: {
                  Event gui_event(event);
                  if(gui_event.keysym.sym == SDLK_ESCAPE) {
                    getButtonPanel().selectQueryTool();
                    break;
                  }
                  if(gui_event.keysym.sym == SDLK_b) {
                    getButtonPanel().toggleBulldozeTool();
                    break;
                  }
                  if(gui_event.keysym.sym == SDLK_F1) {
                    helpWindow->showTopic("help");
                      break;
                  }
                  if(gui_event.keysym.sym == SDLK_F12) {
                    quickSave();
                    break;
                  }
                  if(gui_event.keysym.sym == SDLK_F9) {
                    quickLoad();
                    break;
                  }
#ifdef DEBUG
                  if(gui_event.keysym.sym == SDLK_F5) {
                    testAllHelpFiles();
                    break;
                  }
#endif
                  int need_break=true;
                  switch(gui_event.keysym.sym) {
                    case SDLK_BACKQUOTE: getMiniMap().mapViewChangeDisplayMode(MiniMap::NORMAL); break;
                    case SDLK_1: getMiniMap().mapViewChangeDisplayMode(MiniMap::STARVE); break;
                    case SDLK_2: getMiniMap().mapViewChangeDisplayMode(MiniMap::UB40); break;
                    case SDLK_3: getMiniMap().mapViewChangeDisplayMode(MiniMap::POWER); break;
                    case SDLK_4: getMiniMap().mapViewChangeDisplayMode(MiniMap::FIRE); break;
                    case SDLK_5: getMiniMap().mapViewChangeDisplayMode(MiniMap::CRICKET); break;
                    case SDLK_6: getMiniMap().mapViewChangeDisplayMode(MiniMap::HEALTH); break;
                    case SDLK_7: getMiniMap().mapViewChangeDisplayMode(MiniMap::TRAFFIC); break;
                    case SDLK_8: getMiniMap().mapViewChangeDisplayMode(MiniMap::POLLUTION); break;
                    case SDLK_9: getMiniMap().mapViewChangeDisplayMode(MiniMap::COAL); break;
                    case SDLK_0: getMiniMap().mapViewChangeDisplayMode(MiniMap::COMMODITIES); break;
                    default:  need_break=false;
                  }
                  if (need_break) break;

                  gui->event(gui_event);
                  break;
                }
                case SDL_MOUSEMOTION:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEWHEEL:
                case SDL_KEYDOWN: {
                    Event gui_event(event);
                    gui->event(gui_event);
                    break;
                }
                case SDL_QUIT: {
                    saveCityNG(*world, getConfig()->userDataDir.get()
                      / "9_currentGameNG.scn.gz");
                    // push the QUIT event back for main menu to handle
                    int s = SDL_PushEvent(&event);
                    assert(s == 1);
                    return;
                }
                default:
                    break;
            }

            if(desktop->needsRedraw())
              next_task = 0;
        }

        Uint32 tick = SDL_GetTicks();
        get_real_time_with(tick);
        frame++;

        if(tick >= next_gui) { // gui update
            // fire update event
            gui->event(Event((tick - prev_gui) / 1000.0f));

            // show any pending dialogs
            while(Message::ptr message = world->popMessage())
              handleMessage(message);

            if(world->isUpdated(World::Updatable::MAP)) {
              getGameView().setDirty();
              getGameView().setMapDirty();
              getMiniMap().setDirty();
              getMiniMap().setMapDirty();
              world->clearUpdated(World::Updatable::MAP);
            }

            // update the help window
            // TODO: Why is this not triggered by the gui update?
            helpWindow->update();

            if(new_day) {
              new_day = false;
              updateDate();
            }
            if(new_month) {
              new_month = false;
              getPBar1().refresh();
              getPBar2().refresh();
              getMpsMap().refresh();
              getEconomyGraph().setDirty();
              getGameView().setMapDirty();
              getMiniMap().setMapDirty();
            }
            if(new_year) {
              new_year = false;
              getButtonPanel().updateTech();
              getSound()->setTechLevel(this->world->tech_level);
              getMpsFinance().refresh();
              updateTech();
            }
            if(world->isUpdated(World::Updatable::MONEY)) {
              updateMoney();
              world->clearUpdated(World::Updatable::MONEY);
            }

            // reschedule
            next_gui = tick + 1000/30; // 30 FPS
            prev_gui = tick;
        }
        if(tick >= next_execute) { // execute
            // simulation timestep
            world->do_time_step();

            new_day = true;
            if(world->total_time % NUMOF_DAYS_IN_MONTH == 0)
              new_month = true;
            if(world->total_time % NUMOF_DAYS_IN_YEAR == 0)
              new_year = true;

            // reschedule
            next_execute = tick + simDelay;
            prev_execute = tick;
        }
        if(tick >= next_animate) { // game animation
          // animate
          world->do_animate(tick);
          // TODO: see if texture fetching can be done in initialization
          //fetch remaining textures in order loader thread can exit
          if(getGameView().textures_ready && getGameView().remaining_images)
            getGameView().fetchTextures();
          getGameView().setDirty();
          getGameView().setMapDirty();

          // reschedule
          next_animate = tick + ANIMATE_DELAY;
          prev_animate = tick;
        }
        if(tick >= next_fps) { // fps
#ifdef DEBUG_FPS
          printf("FPS: %d\n", (frame*1000) / (tick - prev_fps));
#endif
          getEconomyGraph().newFPS(frame);
          frame = 0;

          // reschedule
          next_fps = tick + 1000;
          prev_fps = tick;
        }

        if(desktop->needsRedraw()) { // redraw
          desktop->draw(*painter);
          painter->updateScreen();
        }

        // this is kind of janky, but it works for now
        if( simDelay == SIM_DELAY_PAUSE || blockingDialogIsOpen ) {
          // deschedule execute and animate
          next_execute = ~0;
          next_animate = ~0;
        }
        else if(next_execute == (Uint32)~0 || next_animate == (Uint32)~0) {
            // reschedule execute and animate
            next_execute = tick;
            next_animate = tick;
        }
    }
}

void
Game::handleMessage(Message::ptr message_) {
  if(FireStartedMessage::ptr message =
    dynamic_message_cast<FireStartedMessage>(message_)
  ) {
    DialogBuilder()
      .titleText(_("Fire!"))
      .messageAddTextBold(_("A Fire has Started"))
      .messageAddText(fmt::format(_("A fire has broken out in a {}! You should"
          " address this promptly before the whole city burns down."),
        _(message->getGroup().name)))
      .imageFile("images/gui/dialogs/warning.png") // TODO: fire icon
      .buttonSet(DialogBuilder::ButtonSet::OK)
      .build();
  }
  else if(OutOfMoneyMessage::ptr message =
    dynamic_message_cast<OutOfMoneyMessage>(message_)
  ) {
    if(message->isOutOfCredit()) {
        DialogBuilder()
          .titleText(_("Warning!"))
          .messageAddTextBold(_("Out of Credit"))
          .messageAddText(_("You are deep in debt and at the end of your"
            "credit line. You are about to find out what happens when "
            "government checks bounce."))
          .imageFile("images/gui/dialogs/warning.png") // TODO: money icon
          .buttonSet(DialogBuilder::ButtonSet::OK)
          .build();
    }
    else {
      DialogBuilder()
        .titleText(_("Warning!"))
        .messageAddTextBold(_("Out of Money"))
        .messageAddText(_("You just spent all your money. Further spending "
          "will require you to take out a loan which you must pay interest "
          "on. And certain projects you are not allowed to pay for with "
          "credit. The legislative council strongly advises you learn to "
          "budget."))
        .imageFile("images/gui/dialogs/warning.png") // TODO: money icon
        .buttonSet(DialogBuilder::ButtonSet::OK)
        .build();
    }
  }
  else if(RocketReadyMessage::ptr message =
    dynamic_message_cast<RocketReadyMessage>(message_)
  ) {
    DialogBuilder()
      .titleText(_("Rocket Ready"))
      .messageAddTextBold(_("A Rocket is Ready for Launch"))
      .messageAddText(fmt::format(_("The rocket at {} has finished construction"
          " and is ready for takeoff. You may choose to launch now or later. If"
          " you choose to wait, beware it costs money to keep the rocket in"
          " tip-top shape until launch day."),
        message->getPoint()))
      .messageAddText(_("Launch now?"))
      .imageFile("images/gui/dialogs/info.png") // TODO: rocket icon
      .buttonSet(DialogBuilder::ButtonSet::YESNO)
      .onYes([this, point = message->getPoint()]() {
        RocketPad *rocket = dynamic_cast<RocketPad *>(
          getWorld().map(point)->construction);
        if(!rocket) return; // it must have been deleted
        rocket->launch_rocket();
      })
      .build();
  }
  else if(RocketResultMessage::ptr message =
    dynamic_message_cast<RocketResultMessage>(message_)
  ) {
    DialogBuilder dialog;
    dialog
      .titleText(_("Rocket Launched"))
      .imageFile("images/gui/dialogs/info.png") // TODO: rocket icon
      .buttonSet(DialogBuilder::ButtonSet::OK);
    switch(message->getResult()) {
    case RocketResultMessage::LaunchResult::FAIL:
      dialog
        .messageAddTextBold(_("Rocket Crashed"))
        .messageAddText(_("Looks like your rocket technology leaves some to be"
          " desired. Thankfully this was a test flight, so no people were on"
          " board. The scientists say they have identified the cause of the"
          " crash and are confident future launches will go smoothly."))
        .imageFile("images/gui/dialogs/warning.png");
      break;
    case RocketResultMessage::LaunchResult::SUCCESS:
      dialog
        .messageAddTextBold(_("Test Launch Successful"))
        .messageAddText(_("The rocket successfully completed a test flight."));
        // TODO: Add information about # of successful test launches so far
        //       and/or # of remaining successful test launches before
        //       evacuating people.
      break;
    case RocketResultMessage::LaunchResult::EVAC:
      dialog
        .messageAddTextBold(_("Evacuation Launch Successful"))
        .messageAddText(_("You have successfully evacuated 1000 people. You "
          "may consider reducing the size of your economy appropriately."));
      break;
    case RocketResultMessage::LaunchResult::EVAC_WIN:
      dialog
        .titleText("You Won!")
        .messageAddTextBold(_("You Evacuated Everyone!"))
        .messageAddText(_("Congradulations! You have successfully evacuated "
          "everyone from the city and won the game."));
      break;
    default:
      assert(false);
    }
    dialog.build();
  }
  else if(NoPeopleLeftMessage::ptr message =
    dynamic_message_cast<NoPeopleLeftMessage>(message_)
  ) {
    DialogBuilder()
      .titleText(_("You Lost!"))
      .messageAddTextBold(_("Everyone Died!"))
      .messageAddText(_("Your leadership drove the city to ruin, and "
        "all the people died."))
      .imageFile("images/gui/dialogs/warning.png")
      .buttonSet(DialogBuilder::ButtonSet::OK)
      .build();
  }
  else if(SustainableEconomyMessage::ptr message =
    dynamic_message_cast<SustainableEconomyMessage>(message_)
  ) {
    DialogBuilder()
      .titleText(_("You Won!"))
      .messageAddTextBold(_("Economy Sustainable"))
      .messageAddText(_("Well done! Thanks to your excellent leadership, the "
        "city council has determined the economy is sustainable and "
        "(at least theoretically) may continue indefinitely in prosperity. "
        "Therefore, you may retire from your position -- of course with a "
        "substantial pension."))
      .imageFile("images/gui/dialogs/info.png")
      .buttonSet(DialogBuilder::ButtonSet::OK)
      .build();
  }
  else if(OutsideMapMessage::ptr message =
    dynamic_message_cast<OutsideMapMessage>(message_)
  ) {
    // silently ignore requests to do stuff outside the map
  }
  else if(CannotBuildMessage::ptr message =
    dynamic_message_cast<CannotBuildMessage>(message_)
  ) {
    CannotBuildHereMessage::ptr hereMessage =
      dynamic_message_cast<CannotBuildHereMessage>(message);
    Message::ptr reason_ = message->getReason();
    DialogBuilder dialog;
    dialog
      .titleText(_("Cannot Build"))
      .messageAddTextBold(fmt::format(
        hereMessage ? _("Cannot build a {} here.") : _("Cannot build a {}."),
        _(message->getGroup().name)))
      .imageFile("images/gui/dialogs/warning.png")
      .buttonSet(DialogBuilder::ButtonSet::OK);
    if(OutOfMoneyMessage::ptr reason =
      dynamic_message_cast<OutOfMoneyMessage>(reason_)
    ) {
      if(reason->isOutOfCredit()) {
        dialog.messageAddText(_("You do not have sufficient credit to build "
          "this."));
      }
      else {
        dialog.messageAddText(_("You cannot build this on credit."));
      }
    }
    else if(NotEnoughTechMessage::ptr reason =
      dynamic_message_cast<NotEnoughTechMessage>(reason_)
    ) {
      const std::string formatStr = hereMessage
        ? _("Your tech level is too low. Building a {} here requires {:.1f}"
          " tech level, but you have only {:.1f} tech level.")
        : _("Your tech level is too low. Building a {} requires {:.1f}"
          " tech level, but you have only {:.1f} tech level.");
      dialog.messageAddText(fmt::format(formatStr,
        _(message->getGroup().name),
        reason->getRequiredTech() * 100.0f / MAX_TECH_LEVEL,
        reason->getCurrentTech() * 100.0f / MAX_TECH_LEVEL));
    }
    else if(SpaceOccupiedMessage::ptr reason =
      dynamic_message_cast<SpaceOccupiedMessage>(reason_)
    ) {
      dialog.messageAddText(_("This space is occupied."));
    }
    else if(OutsideMapMessage::ptr reason =
      dynamic_message_cast<OutsideMapMessage>(reason_)
    ) {
      dialog.messageAddText(_("Silly! You cannot build outside the map."));
    }
    else if(DesertHereMessage::ptr reason =
      dynamic_message_cast<DesertHereMessage>(reason_)
    ) {
      dialog.messageAddText(fmt::format(
        _("A {} needs water, but this space is desert."),
        _(message->getGroup().name)));
    }
    else if(NoOreMessage::ptr reason =
      dynamic_message_cast<NoOreMessage>(reason_)
    ) {
      dialog.messageAddText(_("There are no ore reserves left here."));
    }
    else if(!reason_) {
// #ifdef DEBUG
      // clang doesn't like '*' operator in typeid
      const CannotBuildMessage& message_raw = *message;
      std::cerr << "warning: no reason given in CannotBuildMessage: "
        << typeid(message_raw).name() << ": "
        << message->str() << std::endl;
// #endif
      assert(false);
    }
    else {
// #ifdef DEBUG
      // clang doesn't like '*' operator in typeid
      const Message& reason_raw = *reason_;
      std::cerr << "warning: unrecognized reason in CannotBuildMessage: "
        << typeid(reason_raw).name() << ": "
        << reason_->str() << std::endl;
// #endif
      dialog
        .messageAddText(_("unrecognized reason"))
        .imageFile("images/gui/dialogs/error.png");
      assert(false);
    }
    dialog.build();
  }
  else if(CannotBulldozeThisMessage::ptr message =
    dynamic_message_cast<CannotBulldozeThisMessage>(message_)
  ) {
    DialogBuilder dialog;
    dialog
      .titleText(_("Cannot Bulldoze"))
      .messageAddTextBold(fmt::format(_("Cannot bulldoze this {}."),
        _(message->getGroup().name)))
      .imageFile("images/gui/dialogs/warning.png")
      .buttonSet(DialogBuilder::ButtonSet::OK);
    if(CannotBulldozeNonemptyTipMessage::ptr message =
      dynamic_message_cast<CannotBulldozeNonemptyTipMessage>(message_)
    ) {
      dialog.messageAddText(_("You cannot bulldoze this land fill because it is"
        " full of waste."));
    }
    else if(CannotBulldozeIncompleteMonumentMessage::ptr message =
      dynamic_message_cast<CannotBulldozeIncompleteMonumentMessage>(message_)
    ) {
      dialog.messageAddText(_("You cannot bulldoze this monument because it is"
        " still under construction."));
    }
    else if(CannotBulldozeThisEverMessage::ptr message =
      dynamic_message_cast<CannotBulldozeThisEverMessage>(message_)
    ) {
      dialog.messageAddText(fmt::format(
        _("You are not allowed to bulldoze {}."),
        _(message->getGroup().name_plural)));
    }
    else {{
// #ifdef DEBUG
      // clang doesn't like '*' operator in typeid
      const Message& message_raw = *message_;
      std::cerr << "warning: unrecognized message derived from "
        << "CannotBulldozeThisMessage: "
        << typeid(message_raw).name() << ": "
        << message_->str() << std::endl;
// #endif
      CannotBulldozeThisMessage::ptr message =
        dynamic_message_cast<CannotBulldozeThisMessage>(message_);
      dialog.messageAddText(fmt::format(_("You are not allowed to bulldoze this"
        " {}, but we're not exactly sure why."),
        _(message->getGroup().name)));
      dialog.imageFile("images/gui/dialogs/warning.png");
      assert(false);
    }}
    dialog.build();
  }
  else if(CannotEvacuateThisMessage::ptr message =
    dynamic_message_cast<CannotEvacuateThisMessage>(message_)
  ) {
    DialogBuilder()
      .titleText(_("Cannot Evacuate"))
      .imageFile("images/gui/dialogs/warning.png")
      .buttonSet(DialogBuilder::ButtonSet::OK)
      .messageAddTextBold(fmt::format(_("Cannot evacuate {}."),
        _(message->getGroup().name)))
      .messageAddText(fmt::format(_("You are not allowed to evacuate {}."),
        _(message->getGroup().name_plural)))
      .build();
  }
  else {
// #ifdef DEBUG
    // clang doesn't like '*' operator in typeid
    const Message& message_raw = *message_;
    std::cerr << "warning: encountered unrecognized message: "
      << typeid(message_raw).name() << ": "
      << message_->str() << std::endl;
// #endif
    // DialogBuilder()
    //   .titleText(_("Error!"))
    //   .messageAddTextBold(_("The requested action failed for an unrecognized "
    //     "reason."))
    //   .imageFile("images/gui/dialogs/error.png")
    //   .buttonSet(DialogBuilder::ButtonSet::OK)
    //   .build();
    assert(false);
  }
}

/** @file lincity-ng/Game.cpp */
