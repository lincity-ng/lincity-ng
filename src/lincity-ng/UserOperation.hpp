/* ---------------------------------------------------------------------- *
 * src/lincity-ng/UserOperation.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2025      David Bears
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

#ifndef USER_OPERATION_H__
#define USER_OPERATION_H__

#include "lincity/messages.hpp"  // for Message

class ConstructionGroup;
class MapPoint;
class World;

class UserOperation {
public:
  UserOperation();
  ~UserOperation();
  UserOperation& operator=(const UserOperation& other);

  enum Action {
    ACTION_QUERY,
    ACTION_EVACUATE,
    ACTION_BULLDOZE,
    ACTION_FLOOD,
    ACTION_BUILD,
    ACTION_UNKNOWN
  };
  Action action;
  ConstructionGroup *constructionGroup; // non-null for ACTION_BUILD

  bool isAllowedHere(World& world, MapPoint point, Message::ptr& message) const;
  bool isAllowed(World& world, Message::ptr& message) const;
  void execute(World& world, MapPoint point);
  unsigned short cursorSize() const;
};



#endif
