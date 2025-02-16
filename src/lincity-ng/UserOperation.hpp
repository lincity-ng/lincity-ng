/* ---------------------------------------------------------------------- *
 * src/lincity/UserOperation.h
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
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

#include <string>  // for string, basic_string

class ConstructionGroup;

class UserOperation {
public:
    UserOperation();
    ~UserOperation();
    ConstructionGroup *constructionGroup; // !0 in case of ACTION_BUILD
    std::string helpName;

    enum Action //What User wants to do
    {
        ACTION_QUERY,
        ACTION_EVACUATE,
        ACTION_BULLDOZE,
        ACTION_FLOOD,
        ACTION_BUILD,
        ACTION_UNKNOWN
    };

    Action action;
    bool is_allowed_here(int x, int y, bool warning);
    bool enoughTech();// tells if scaled tech_level is sufficient
    float requiredTech(); // returns scaled tech_level for tooltipp info
    unsigned short cursorSize(); //tells size of affectet aerea
    std::string createTooltip( bool root = true );
private:
};



#endif
