/*
Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef __lc_mps_h__
#define __lc_mps_h__

#include "gui/Component.hpp"
#include "MapPoint.hpp"
#include "gui_interface/mps.h"

class XmlReader;

void mps_update(int mps_x, int mps_y, int mps_style);
extern int mps_x;
extern int mps_y;
extern int mps_style;

class Paragraph;

/**
 * The MPS component is more or less a text component with 13 lines that
 * is used to display status information about the game/engine.
 */
class Mps : public Component
{
public:
    Mps();
    ~Mps();

    void parse(XmlReader& reader);

    void clear();
    void setView(MapPoint point, int style = MPS_MAP );
    void setText(int i, const std::string &s);
    void playBuildingSound(int mps_x, int mps_y);

private:
    typedef std::vector<Paragraph*> Paragraphs;
    Paragraphs paragraphs;
    int mX, mY;
};

extern Mps* globalMPS;
extern Mps* mapMPS;
extern Mps* envMPS;

#endif

/** @file lincity-ng/Mps.hpp */

