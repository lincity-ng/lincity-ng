/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>

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

/**
 * @author Matthias Braun
 * @file TableLayout.hpp
 */

#ifndef __TABLELAYOUT_HPP__
#define __TABLELAYOUT_HPP__

#include <vector>
#include "Component.hpp"

class XmlReader;

/**
 * @class TableLayout
 * @brief This components allows to layout child-components in a
 * flat-rectangular table. 
 * 
 * You can setup an arbitrary number of rows and columns (you need at
 * least 1 of course). You specify the sizing properties for each row and column
 * (see layouting algorithm below)
 *
 * There are 2 types of cells and rows: Fixed size and relative size. The
 * layouting algorithm works in 2 phases: First it gives all fixed size cells
 * and rows the requested size in pixels. After that it distributes the
 * remaining width/height to all rows/cols based on the weight given the
 * properties.
 *  Example:
 *      row1 is relative size 1.0, row2 is fixed size 100,
 *      row3 is relative size 2.5
 *
 *      If the table gets a resize request for height 800 now it first assigns
 *      row2 a size of 100 pixel. The remaining 700 pixels are distributed for
 *      row1 and row3. row1 gets 200 pixels and row3 500pixels which is 2.5
 *      times the value of row1.
 */
class TableLayout : public Component
{
public:
    TableLayout();
    ~TableLayout();

    void parse(XmlReader& reader);

    void resize(float width, float height);
    void draw(Painter& painter);
    bool opaque(const Vector2& pos) const;

    struct RowColProperties
    {
        RowColProperties()
            : type(TYPE_RELATIVE), val(1)
        {
        }

        enum Type { TYPE_FIXEDSIZE, TYPE_RELATIVE };
        Type type;
        float val;
        float realval;
    };
    
    struct Cell
    {
        Cell(int _childid = -1)
            : childid(_childid), halign(CENTER), valign(CENTER),
            colspan(1), rowspan(1)
        { }

        enum Alignment { LEFT = 0, CENTER = 1, RIGHT = 2,
                         TOP = 0, BOTTOM = 2 };                  
        int childid;
        Alignment halign;
        Alignment valign;
        int colspan;
        int rowspan;
    };
    void addRow(const RowColProperties& props);
    void addColumn(const RowColProperties& props);
    void addComponent(size_t col, size_t row, Component* component);

private:
    void removeComponents();
    int parseProperties(XmlReader& reader, RowColProperties& props);

    typedef std::vector<RowColProperties> Properties;
    Properties rowproperties;
    Properties colproperties;

    typedef std::vector<Cell> Cells;
    Cells cells;
    bool border;
};

#endif


/** @file gui/TableLayout.hpp */

