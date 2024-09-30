/* ---------------------------------------------------------------------- *
 * src/lincity/ConstructionManager.cpp
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

#include "ConstructionManager.h"

#include <utility>                // for pair

#include "ConstructionRequest.h"

void ConstructionManager::submitRequest(ConstructionRequest *request) {
    std::map<Construction *, ConstructionRequest *>::iterator iterator;
    iterator = pendingRequests.find(request->subject);
    if (iterator != pendingRequests.end())
    {
        //std::cout<<"duplicate Request at "<<request->subject->constructionGroup->name<<" x,y = "<<request->subject->x<<","<<request->subject->y<<std::endl;
        delete iterator->second;
        iterator->second = request;
    }
    else
    {
        //std::cout<<"added Request at "<<request->subject->constructionGroup->name<<" x,y = "<<request->subject->x<<","<<request->subject->y<<std::endl;
        pendingRequests[request->subject] = request;
    }
}

void ConstructionManager::executeRequest(ConstructionRequest *request) {
    std::map<Construction *, ConstructionRequest *>::iterator iterator;
    iterator = pendingRequests.find(request->subject);
    if (iterator != pendingRequests.end())
    {
        //make sure there wont be no another pending request after execution
        if (request != iterator->second)
        {
			delete iterator->second;
			pendingRequests.erase(iterator->first);
		}
    }
    request->execute();
    delete request;
}

void ConstructionManager::executePendingRequests() {
    std::map<Construction *, ConstructionRequest *>::const_iterator iterator;
    for (iterator = pendingRequests.begin(); iterator != pendingRequests.end(); iterator++)
    {
        executeRequest(iterator->second);
    }
    pendingRequests.clear();
}

void ConstructionManager::clearRequests()
{
	pendingRequests.clear();
}


std::map<Construction *, ConstructionRequest *> ConstructionManager::pendingRequests;
