/* ---------------------------------------------------------------------- *
 * src/lincity-ng/UserOperation.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
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

#include "UserOperation.hpp"

#include <stddef.h>                 // for NULL
#include <iostream>                 // for basic_ostream, operator<<, cout
#include <sstream>                  // for basic_stringstream

#include "lincity/engglobs.h"               // for world, tech_level
#include "lincity/groups.h"                 // for GROUP_RESIDENCE_HH, GROUP_RESIDEN...
#include "lincity/lin-city.h"               // for FLAG_NEVER_EVACUATE, MAX_TECH_LEVEL
#include "lincity/lintypes.h"               // for ConstructionGroup, MapTile, Const...
#include "tinygettext/gettext.hpp"  // for _
#include "lincity/world.h"                  // for Map
#include "Game.hpp"
#include "Dialog.hpp"
#include "lincity/modules/rocket_pad.h"
#include "lincity/modules/windpower.h"
#include "lincity/modules/windmill.h"
#include "lincity/modules/parkland.h"
#include "MiniMap.hpp"
#include "Sound.hpp"
#include "gui/DialogBuilder.hpp"

UserOperation::UserOperation() {
  constructionGroup = NULL;
  action = ACTION_QUERY;
  dialogShown = false;
}

UserOperation::~UserOperation() { }

bool
UserOperation::isAllowed(World& world, Message::ptr& message) {
  switch(action) {
  case ACTION_BUILD:
    return constructionGroup->can_build(world, message);
  case ACTION_QUERY:
  case ACTION_BULLDOZE:
  case ACTION_EVACUATE:
  case ACTION_FLOOD:
    return true;
  default:
    assert(false);
    return false;
  }
}

bool
UserOperation::isAllowedHere(World& world, MapPoint point, Message::ptr& message
) {
  if(!world.map.is_visible(point)) {
    message = OutsideMapMessage::create(point);
    return false;
  }
  switch(action) {
  case ACTION_QUERY:
    return true;
  case ACTION_BUILD:
    return constructionGroup->can_build(world, message)
      && constructionGroup->can_build_here(world, point, message);
  case ACTION_BULLDOZE: {
    return true;
    // bool bare = world.map(point)->is_bare();
    // if(bare) message = NothingHereMessage::create(point);
    // return !bare;
  }
  case ACTION_EVACUATE:
    if(!world.map(point)->reportingConstruction) {
      message = NothingHereMessage::create(point);
      return false;
    }
    if(world.map(point)->reportingConstruction->flags & FLAG_NEVER_EVACUATE) {
      message = CannotEvacuateThisMessage::create(point,
        *world.map(point)->reportingConstruction->constructionGroup);
      return false;
    }
    return true;
  case ACTION_FLOOD: {
    bool bare = world.map(point)->is_bare();
    if(!bare) message = SpaceOccupiedMessage::create(point);
    return bare;
  }
  default:
    assert(false);
    return false;
  }
}

unsigned short UserOperation::cursorSize(void) {
    if( action == ACTION_QUERY )
    {   return 0;}
    else if(action == ACTION_BUILD)
    {   return constructionGroup->size;}
    else
    {   return 1;}
}

bool
UserOperation::execute(Game& game, MapPoint point) {
  try {
    do_execute(game, point);
    return true;
  }
  catch(const Message::Exception& ex) {
    handleMessage(ex.getMessage());
    return false;
  }
}

void
UserOperation::do_execute(Game& game, MapPoint point) {
  World& world = game.getWorld();
  if(!world.map.is_visible(point))
    OutsideMapMessage::create(point)->throwEx();

  switch(action) {
  case UserOperation::ACTION_QUERY: {
    MpsMap& mps = game.getMpsMap();
    if(mps.point == point) {
      switch(mps.page) {
      case MpsMap::Page::INVENTORY:
        mps.page = MpsMap::Page::PRODUCTION;
        break;
      case MpsMap::Page::PRODUCTION:
        mps.page = MpsMap::Page::INVENTORY;
        break;
      case MpsMap::Page::GROUND:
        mps.page = MpsMap::Page::GROUND;
        break;
      default:
        assert(false);
      }

      switch(world.map(point)->getGroup()) {
      case GROUP_MARKET:
        new Dialog(game, EDIT_MARKET, point.x, point.y);
        break;
      case GROUP_PORT:
        new Dialog(game, EDIT_PORT, point.x, point.y);
        break;
      case GROUP_ROCKET:
        RocketPad *rocket = dynamic_cast<RocketPad *>(
          world.map(point)->reportingConstruction);
        assert(rocket);
        if(rocket->stage == RocketPad::AWAITING) {
          new Dialog(game, ASK_LAUNCH_ROCKET, point.x, point.y);
        }
        break;
      }
    }
    mps.point = point;
    mps.refresh();
    game.getMiniMap().switchView("MapMPS");
    getSound()->playSound(*world.map(point));

    return;
  }

  case UserOperation::ACTION_BUILD: {
    assert(constructionGroup);

    //double check windmill tech
    if(constructionGroup == &windmillConstructionGroup
      || constructionGroup == &windpowerConstructionGroup
    ) {
      if(world.tech_level >= MODERN_WINDMILL_TECH)
        constructionGroup = &windpowerConstructionGroup;
      else
        constructionGroup = &windmillConstructionGroup;
    }

    // TODO: move dependence on SDL_GetKeyboardState elsewhere (GameView?)
    if(constructionGroup == &parklandConstructionGroup
      && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_K]
    )
      constructionGroup = &parkpondConstructionGroup;

    world.buildConstruction(*constructionGroup, point);
    getSound()->playSound(*world.map(point));
    break;
  }

  case UserOperation::ACTION_BULLDOZE: {
    unsigned short grp = world.map(point)->getGroup();
    bool *warnBull = nullptr;
    switch(grp) {
    case GROUP_MONUMENT:
      warnBull = &game.warnBullMonument;
      break;
    case GROUP_RIVER:
      warnBull = &game.warnBullWater;
      break;
    case GROUP_SHANTY:
      warnBull = &game.warnBullShanty;
      break;
    }
    if(!warnBull || !*warnBull) {
      world.bulldozeArea(point);
      getSound()->playSound("Raze");
    }
    else {
      // TODO: avoid asking many times for area-bulldoze
      Construction *cst = world.map(point)->reportingConstruction;
      DialogBuilder()
        .titleText("Warning")
        .messageAddTextBold("Warning:")
        .messageAddText(std::string(_("Bulldozing a ")) +
          _(world.map(point)->getConstructionGroup()->getName().c_str()) +
          _(" costs a lot of money."))
          // TODO: make sure this is localized correctly
        .messageAddText("Want to bulldoze?")
        .imageFile("images/gui/dialogs/warning.png")
        // TODO: use "Bulldoze"/"Leave It" buttons
        .buttonSet(DialogBuilder::ButtonSet::YESNO)
        .onYes([&world, point, cst, grp, warnBull]() {
          // make sure things haven't changed
          // TODO: cancel the dialog in the UserOperation deleter
          // TODO: problems might happen if the user goes to the main menu
          if(world.map(point)->reportingConstruction == cst
            && world.map(point)->getGroup() == grp
          ) {
            *warnBull = false;
            world.bulldozeArea(point);
            getSound()->playSound("Raze");
          }
        })
        .build();
    }

    break;
  }

  case UserOperation::ACTION_EVACUATE: {
    world.evacuateArea(point);
    getSound()->playSound(*world.map(point));
    break;
  }

  case UserOperation::ACTION_FLOOD: {
    world.floodArea(point);
    getSound()->playSound(*world.map(point));
    break;
  }

  default: {
    assert(false);
  }
  }
}

void
UserOperation::handleMessage(Message::ptr message_) {
  if(OutsideMapMessage::ptr message =
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
      .messageAddTextBold(_("Cannot build a ") + message->getGroup().name
        + (hereMessage ? _(" here.") : "."))
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
      dialog.messageAddText((std::ostringstream()
        << _("Tech level too low. Requires ")
        << std::fixed << std::setprecision(1)
        << (reason->getRequiredTech() * 100.0f / MAX_TECH_LEVEL)
      ).str());
    }
    else if(SpaceOccupiedMessage::ptr reason =
      dynamic_message_cast<SpaceOccupiedMessage>(reason_)
    ) {
      dialog.messageAddText(_("The space is occupied."));
    }
    else if(OutsideMapMessage::ptr reason =
      dynamic_message_cast<OutsideMapMessage>(reason_)
    ) {
      dialog.messageAddText(_("You cannot build outside the map."));
    }
    else if(DesertHereMessage::ptr reason =
      dynamic_message_cast<DesertHereMessage>(reason_)
    ) {
      dialog.messageAddText(_("A ") + message->getGroup().name
        + _(" needs water, but this space is desert."));
    }
    else if(!reason_) {
// #ifdef DEBUG
      std::cerr << "warning: no reason given in CannotBuildMessage: "
        << typeid(*message).name() << ": "
        << message->str() << std::endl;
// #endif
    }
    else {
// #ifdef DEBUG
      std::cerr << "warning: unrecognized reason in CannotBuildMessage: "
        << typeid(*reason_).name() << ": "
        << reason_->str() << std::endl;
// #endif
      dialog
        .messageAddText(_("unrecognized reason"))
        .imageFile("images/gui/dialogs/error.png");
    }
    dialog.build();
  }
  else if(CannotBulldozeThisMessage::ptr message =
    dynamic_message_cast<CannotBulldozeThisMessage>(message_)
  ) {
    DialogBuilder dialog;
    dialog
      .titleText(_("Cannot Bulldoze"))
      .imageFile("images/gui/dialogs/warning.png")
      .buttonSet(DialogBuilder::ButtonSet::OK);
    if(CannotBulldozeNonemptyTipMessage::ptr message =
      dynamic_message_cast<CannotBulldozeNonemptyTipMessage>(message_)
    ) {
      dialog
        .messageAddTextBold(_("Cannot bulldoze this ")
          + message->getGroup().name)
        .messageAddText(_("You cannot bulldoze a tip that is full of waste."));
    }
    else if(CannotBulldozeIncompleteMonumentMessage::ptr message =
      dynamic_message_cast<CannotBulldozeIncompleteMonumentMessage>(message_)
    ) {
      dialog
        .messageAddTextBold(_("Cannot bulldoze this ")
          + message->getGroup().name)
        .messageAddText(_("You cannot bulldoze a monument that under "
          "construction."));
    }
    else {
      // TODO: CannotBulldozeThisMessage should include a reason like
      //       CannotBuildMessage does. This would allow explicitly giving the
      //       reason that such a construction is never bulldozable.
      assert(typeid(*message) == typeid(CannotBulldozeThisMessage));
      dialog
        .messageAddTextBold(_("You cannot bulldoze a ")
          + message->getGroup().name + ".")
        .messageAddText(_("You are not allowed to bulldoze this type of "
          "construction."));
    }
    dialog.build();
  }
  else if(CannotEvacuateThisMessage::ptr message =
    dynamic_message_cast<CannotEvacuateThisMessage>(message_)
  ) {
    DialogBuilder()
      .titleText(_("Cannot Bulldoze"))
      .imageFile("images/gui/dialogs/warning.png")
      .buttonSet(DialogBuilder::ButtonSet::OK)
      .messageAddTextBold(_("You cannot evacuate a ")
        + message->getGroup().name + ".")
      .messageAddText(_("You are not allowed to evacuate this type of "
        "construction."))
      .build();
  }
  else {
// #ifdef DEBUG
    std::cerr << "warning: unrecognized message thrown while executing action: "
      << typeid(*message_).name() << ": "
      << message_->str() << std::endl;
// #endif
    DialogBuilder()
      .titleText(_("Error!"))
      .messageAddTextBold(_("The requested action failed for an unrecognized "
        "reason."))
      .imageFile("images/gui/dialogs/error.png")
      .buttonSet(DialogBuilder::ButtonSet::OK)
      .build();
  }
}
