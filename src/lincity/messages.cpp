/* ---------------------------------------------------------------------- *
 * src/lincity/messages.cpp
 * This file is part of Lincity-NG.
 *
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

#include "messages.hpp"

#include <cassert>               // for assert
#include <exception>             // for exception, rethrow_exception, exceptio...
#include <sstream>               // for basic_stringstream, operator<<, basic_...
#include <typeinfo>              // for type_info

#include "lintypes.hpp"          // for ConstructionGroup
#include "modules/monument.hpp"  // for MonumentConstructionGroup, monumentCon...
#include "modules/tip.hpp"       // for TipConstructionGroup, tipConstructionG...

const char *
Message::Exception::what() const noexcept {
  // if(whatMsg == "") whatMsg = message->str();
  return whatMsg.c_str();
}


std::string
ExceptionMessage::str() const {
  try {
    std::rethrow_exception(exception_);
  } catch(std::exception& ex) {
    return std::string(typeid(ex).name()) + ": " + ex.what();
  } catch(...) {
    return "unknown exception";
  }
}


std::string
CannotBuildMessage::str() const {
  std::stringstream s;
  s << "cannot build " << group.getName();
  if(reason) s << ": " << reason->str();
  return s.str();
}


std::string
CannotBuildHereMessage::str() const {
  std::stringstream s;
  s << "cannot build " << group.getName() << " at " << point;
  if(reason) s << ": " << reason->str();
  return s.str();
}


std::string
CannotBulldozeThisMessage::str() const {
  return (std::stringstream() << "cannot bulldoze "
    << group.getName() << " at " << point).str();
}


std::string
CannotEvacuateThisMessage::str() const {
  return (std::stringstream() << "cannot evacuate "
    << group.getName() << " at " << point).str();
}


std::string
FireStartedMessage::str() const {
  return (std::stringstream() << "a fire has started: "
    << group.getName() << " " << point).str();
}


std::string
NothingHereMessage::str() const {
  return (std::stringstream() << "nothing here: " << point).str();
}


std::string
DesertHereMessage::str() const {
  return (std::stringstream() << "desert here: " << point).str();
}


std::string
OutsideMapMessage::str() const {
  return (std::stringstream() << "outside map: " << point).str();
}


std::string
NotEnoughTechMessage::str() const {
  return (std::stringstream() << "insufficient tech: "
    << "current = " << currentTech << ", "
    << "required = " << requiredTech).str();
}


std::string
SpaceOccupiedMessage::str() const {
  return (std::stringstream() << "something is already here: " << point).str();
}


std::string
SustainableEconomyMessage::str() const {
  return "the economy is sustainable";
}


std::string
NoPeopleLeftMessage::str() const {
  return "everyone died";
}


CannotBulldozeIncompleteMonumentMessage::
CannotBulldozeIncompleteMonumentMessage(const MapPoint point) :
  CannotBulldozeThisMessage(point, monumentConstructionGroup)
{ }

std::string
CannotBulldozeIncompleteMonumentMessage::str() const {
  return (std::stringstream()
    << "cannot bulldoze an incomplete monument: "
    << point
  ).str();
}


CannotBulldozeNonemptyTipMessage::CannotBulldozeNonemptyTipMessage(
  const MapPoint point
) :
  CannotBulldozeThisMessage(point, tipConstructionGroup)
{ }

std::string
CannotBulldozeNonemptyTipMessage::str() const {
  return (std::stringstream()
    << "cannot bulldoze a tip that is full of waste: "
    << point
  ).str();
}

std::string
CannotBulldozeThisEverMessage::str() const {
  return (std::stringstream()
    << "cannot bulldoze " << group.name
    << ": " << point
  ).str();
}


std::string
RocketReadyMessage::str() const {
  return "a rocket is ready to launch";
}


std::string
RocketResultMessage::str() const {
  switch(result) {
  case LaunchResult::FAIL:
    return "the rocket crashed";
  case LaunchResult::SUCCESS:
    return "the rocket launched successfully";
  case LaunchResult::EVAC:
    return "1000 people were evacuated";
  case LaunchResult::EVAC_WIN:
    return "everyone has been evacuated from the city";
  default:
    assert(false);
    return "unknown LaunchResult";
  }
}
