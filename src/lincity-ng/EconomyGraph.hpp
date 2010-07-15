/*
Copyright (C) 2005 Wolfgang Becker <uafr@gmx.de>

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
#ifndef __EconomyGraph_HPP
#define __EconomyGraph_HPP

#include "gui/Component.hpp"
#include "gui/XmlReader.hpp"
#include "gui/TextureManager.hpp"
#include "gui/Paragraph.hpp"
#include "gui/Style.hpp"

#include "CheckButton.hpp"

class EconomyGraph : public Component {
public:
    EconomyGraph();
    ~EconomyGraph();

    void parse(XmlReader& reader);
    void draw(Painter& painter);
    void updateData();    
    void newFPS( int frame );
private:
    static const int border = 5;
    void drawHistoryLineGraph( Painter& painter, Rect2D mg );
    void drawSustBarGraph( Painter& painter, Rect2D mg );
    void drawFPSGraph( Painter& painter, Rect2D fpsRect );
 
    int* fps;
    Texture* labelTextureMIN;
    Texture* labelTexturePRT;
    Texture* labelTextureMNY;
    Texture* labelTexturePOP;
    Texture* labelTextureTEC;
    Texture* labelTextureFIR;

    Texture* labelTextureEconomy;
    Texture* labelTextureSustainability;
    Texture* labelTextureFPS;

    bool nobodyHomeDialogShown;

    CheckButton* switchEconomyGraphButton;
    std::string switchEconomyGraphText;
    Paragraph* switchEconomyGraphParagraph;

    Style normalStyle;
    Style redStyle;
    Style yellowStyle;
};

EconomyGraph* getEconomyGraph();

#endif

/** @file lincity-ng/EconomyGraph.hpp */

