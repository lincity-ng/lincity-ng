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

#include <SDL.h>                           // for Uint32, SDL_GetTicks, SDL_...
#include <algorithm>                       // for min
#include <filesystem>                      // for path, directory_iterator
#include <functional>                      // for bind, _1, function
#include <iostream>                        // for basic_ostream, operator<<
#include <stdexcept>                       // for runtime_error

#include "ButtonPanel.hpp"                 // for getButtonPanel, ButtonPanel
#include "Config.hpp"                      // for getConfig, Config
#include "Dialog.hpp"                      // for closeAllDialogs, Dialog
#include "EconomyGraph.hpp"                // for getEconomyGraph, EconomyGraph
#include "GameView.hpp"                    // for getGameView, GameView
#include "HelpWindow.hpp"                  // for HelpWindow
#include "MainLincity.hpp"                 // for saveCityNG, loadCityNG
#include "MiniMap.hpp"                     // for MiniMap, getMiniMap
#include "ScreenInterface.hpp"             // for print_stats, updateDate
#include "TimerInterface.hpp"              // for get_real_time_with
#include "Util.hpp"                        // for getButton
#include "gui/Button.hpp"                  // for Button
#include "gui/Component.hpp"               // for Component
#include "gui/ComponentLoader.hpp"         // for loadGUIFile
#include "gui/Desktop.hpp"                 // for Desktop
#include "gui/DialogBuilder.hpp"
#include "gui/Event.hpp"                   // for Event
#include "gui/Painter.hpp"                 // for Painter
#include "gui/Signal.hpp"                  // for Signal
#include "gui/WindowManager.hpp"
#include "gui_interface/mps.h"             // for mps_refresh, mps_set, mps_...
#include "lincity/engglobs.h"              // for constructionCount
#include "lincity/lin-city.h"              // for ANIMATE_DELAY, SIM_DELAY_P...
#include "lincity/lintypes.h"              // for Construction
#include "PBar.hpp"
#include "Sound.hpp"
#include "tinygettext/gettext.hpp"

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
    saveCityNG(*world, getConfig()->userDataDir / "9_currentGameNG.scn.gz");
    running = false;
    quitState = MAINMENU;
}

void Game::quickLoad(){
  closeAllDialogs();

  //load file
  getGameView().printStatusMessage("quick load...");
  std::string filename("quicksave.scn.gz");
  if(std::unique_ptr<World> world =
    loadCityNG(getConfig()->userDataDir / filename)
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
  saveCityNG(*world, getConfig()->userDataDir / "quicksave.scn.gz");
}

void Game::testAllHelpFiles(){
  getGameView().printStatusMessage("Testing Help Files...");

  std::filesystem::path dir = getConfig()->appDataDir / "help" / "en";
  for(auto& dirEntry : std::filesystem::directory_iterator(dir)) {
    if(!dirEntry.is_regular_file()) continue;
    std::cerr << "--- Examining " << dirEntry.path().stem() << "\n";
    helpWindow->showTopic(dirEntry.path().stem().string());
  }
}

void
Game::loadGui() {
  gui.reset(loadGUIFile("gui/app.xml"));

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
  mpsmap->setGame(this);
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

  getButtonPanel().selectQueryTool();
  getButtonPanel().updateTech(false);
  gameview->show(this->world->map.recentPoint);
  getSound()->setTechLevel(this->world->tech_level);
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
  warnBullWater = warnBullShanty = warnBullMonument = true;

  getGameView().setCursorSize(userOperation.cursorSize());
  getGameView().showToolInfo();
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


MainState
Game::run()
{
    assert(world);
    SDL_Event event;
    running = true;
    Desktop* desktop = dynamic_cast<Desktop*> (gui.get());
    if(!desktop)
    {   throw std::runtime_error("Toplevel component is not a Desktop");}
    gui->resize(getConfig()->videoX, getConfig()->videoY);
    DialogBuilder::setDefaultWindowManager(dynamic_cast<WindowManager *>(
      desktop->findComponent("windowManager")));
    getButtonPanel().selectQueryTool();
    int frame = 0;

    bool new_day = true, new_month = true, new_year = true;
    Uint32 next_execute = ~0, next_animate = ~0, next_gui = 0, next_fps = 0;
    __attribute__((unused))
    Uint32 prev_execute = 0, prev_animate = 0, prev_gui = 0, prev_fps = 0;
    Uint32 next_task;
    Uint32 tick = 0;
    while(running) {
        next_task = std::min({next_execute, next_animate, next_gui, next_fps});
        while(true) {
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
                        getConfig()->videoX = event.window.data1;
                        getConfig()->videoY = event.window.data2;
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
                case SDL_QUIT:
                    saveCityNG(*world, getConfig()->userDataDir / "9_currentGameNG.scn.gz");
                    running = false;
                    quitState = QUIT;
                    break;
                default:
                    break;
            }

            if(desktop->needsRedraw())
              next_task = tick;
        }

        tick = SDL_GetTicks();
        get_real_time_with(tick);
        frame++;

        if(tick >= next_gui) { // gui update
            // fire update event
            gui->event(Event((tick - prev_gui) / 1000.0f));

            // show any pending dialogs
            processMessageQueue();

            // update the help window
            // TODO: Why is this not triggered by the gui update?
            helpWindow->update();

            if(new_day) {
              new_day = false;
              updateDate();
              updateMoney();
            }
            if(new_month) {
              new_month = false;
              getPBar1().refresh();
              getPBar2().refresh();
              getMpsMap().refresh();
              getEconomyGraph().setDirty();
            }
            if(new_year) {
              new_year = false;
              getButtonPanel().updateTech(true);
              getSound()->setTechLevel(this->world->tech_level);
              getMpsFinance().refresh();
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
    return quitState;
}

void
Game::processMessageQueue() {
  while(Message::ptr message_ = world->popMessage()) {
    if(FireStartedMessage::ptr message =
      dynamic_message_cast<FireStartedMessage>(message_)
    ) {
      DialogBuilder()
        .titleText(_("Fire!"))
        .messageAddTextBold(_("A Fire has Started"))
        .messageAddText(_("A fire has broken out in a ")
          + _(message->getGroup().name) + "! " + _("You should address this "
          "promptly before the whole city burns down."))
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
        .messageAddText((std::ostringstream() << _("The rocket at ")
          << message->getPoint() << _(" has finished construction and is ready "
            "for takeoff. You may choose to launch now or later. If you choose "
            "to wait, beware it costs money to keep the rocket in tip-top "
            "shape until launch day.")).str())
        .messageAddText("Launch now?")
        .imageFile("images/gui/dialogs/info.png") // TODO: rocket icon
        .buttonSet(DialogBuilder::ButtonSet::YESNO)
        // TODO: onYes
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
          .messageAddTextBold(_("The Rocket Crashed"))
          .messageAddText(_("Looks like your rocket technology leaves some to "
            "be desired. Thankfully there were no people on board this test "
            "flight. The scientists say they have identified the cause of the"
            "crash and are confident future launches will go smoothly."))
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
    else {
#ifndef NDEBUG
      std::cerr << "warning: unrecognized message in queue: "
        << typeid(*message).name() << ": "
        << message->str() << std::endl;
#endif
    }
  }
}

/** @file lincity-ng/Game.cpp */
