/* ---------------------------------------------------------------------- *
 * src/lincity/ConstructionRequest.hpp
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

#ifndef CONSTRUCTION_REQUEST_H__
#define CONSTRUCTION_REQUEST_H__

class Construction;

class [[deprecated]] ConstructionRequest {
public:
    ConstructionRequest(Construction *subject): subject(subject) { }
    virtual ~ConstructionRequest() { }
    virtual void execute() = 0;
protected:
    Construction *subject;
};

class ConstructionDeletionRequest: public ConstructionRequest {
public:
    ConstructionDeletionRequest(Construction *subject): ConstructionRequest(subject) { }
    virtual void execute();
};

class OreMineDeletionRequest: public ConstructionRequest {
public:
    OreMineDeletionRequest(Construction *subject): ConstructionRequest(subject) { }
    virtual void execute();
};

class CommuneDeletionRequest: public ConstructionRequest {
public:
    CommuneDeletionRequest(Construction *subject): ConstructionRequest(subject) { }
    virtual void execute();
};

class BurnDownRequest: public ConstructionRequest {
public:
    BurnDownRequest(Construction *subject): ConstructionRequest(subject) { }
    virtual void execute();
};

class SetOnFire: public ConstructionRequest {
public:
    SetOnFire(Construction *subject): ConstructionRequest(subject) { }
    virtual void execute();
};


class PowerLineFlashRequest: public ConstructionRequest {
public:
    PowerLineFlashRequest(Construction *subject): ConstructionRequest(subject) { }
    virtual void execute();
};


#endif
