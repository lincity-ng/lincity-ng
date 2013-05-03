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
#include <config.h>

#include "TableLayout.hpp"

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <typeinfo>

#include "Painter.hpp"
#include "Event.hpp"
#include "ComponentFactory.hpp"
#include "ComponentLoader.hpp"
#include "XmlReader.hpp"

TableLayout::TableLayout()
{
    setFlags(FLAG_RESIZABLE);
}

TableLayout::~TableLayout()
{
}

void
TableLayout::parse(XmlReader& reader)
{
    int rows = -1, cols = -1;
    border = false;
    
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else if(strcmp(attribute, "rows") == 0) {
            if(sscanf(value, "%d", &rows) != 1) {
                std::stringstream msg;
                msg << "Error while parsing rows attribute: " << value;
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(attribute, "cols") == 0) {
            if(sscanf(value, "%d", &cols) != 1) {
                std::stringstream msg;
                msg << "Error while parsing cols attribute: " << value;
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(attribute, "border") == 0) {
            if(strcmp(value, "true") == 0) {
                border = true;
            } else if(strcmp(value, "false") == 0) {
                border = false;
            } else {
                std::cerr << "Invalid value for border attribute. "
                    "Please specify 'true' or 'false'\n";
            }
        } else {
            std::cerr << "Skipping unknown attribute '" << attribute << "'.\n";
        }
    }
    if(rows <= 0 || cols <= 0) {
        throw std::runtime_error("Invalid values for rows/cols");
    }
    
    rowproperties.assign(rows, RowColProperties());
    colproperties.assign(cols, RowColProperties());
    cells.assign(rows*cols, Cell());
    
    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            const std::string& element = (const char*) reader.getName();
            if(element == "rowsize") {
                RowColProperties props;
                int num = parseProperties(reader, props) - 1;
                if(num < 0 || num >= rows) {
                    std::cerr 
                        << "Invalid row specified in rowsize element.\n";
                    continue;
                }
                rowproperties[num] = props;
            } else if(element == "colsize") {
                RowColProperties props;
                int num = parseProperties(reader, props) - 1;
                if(num < 0 || num >= cols) {
                    std::cerr 
                        << "Invalid col specified in colsize element.\n";
                    continue;
                }
                colproperties[num] = props;
            } else if(element == "cell") {
                int row = -1, col = -1;
                int colspan = 1, rowspan = 1;
                Cell::Alignment halign = Cell::CENTER;
                Cell::Alignment valign = Cell::CENTER;
                XmlReader::AttributeIterator iter(reader);
                while(iter.next()) {
                    const char* name = (const char*) iter.getName();
                    const char* value = (const char*) iter.getValue();

                    if(strcmp(name, "row") == 0) {
                        if(sscanf(value, "%d", &row) != 1) {
                            std::cerr << "Couldn't parse integer value '"
                                << value << "' in row attribute.\n";
                        }
                    } else if(strcmp(name, "col") == 0) {
                        if(sscanf(value, "%d", &col) != 1) {
                            std::cerr << "Couldn't parse integer value '"
                                << value << "' in col attribute.\n";
                        }
                    } else if(strcmp(name, "rowspan") == 0) {
                        if(sscanf(value, "%d", &rowspan) != 1) {
                            std::cerr << "Couldn't parse integer value '"
                                << value << "' in rowspan attribute.\n";
                        }
                    } else if(strcmp(name, "colspan") == 0) {
                        if(sscanf(value, "%d", &colspan) != 1) {
                            std::cerr << "Couldn't parse integer value '"
                                << value << "' in colspan attribute.\n";
                        }
                    } else if(strcmp(name, "halign") == 0) {
                        if(strcmp(value, "left") == 0) {
                            halign = Cell::LEFT;
                        } else if(strcmp(value, "center") == 0) {
                            halign = Cell::CENTER;
                        } else if(strcmp(value, "right") == 0) {
                            halign = Cell::RIGHT;
                        } else {
                            std::cerr << "Skipping unknown halignment value '"
                                << value << "'.\n";
                        }
                    } else if(strcmp(name, "valign") == 0) {
                        if(strcmp(value, "top") == 0) {
                            valign = Cell::TOP;
                        } else if(strcmp(value, "center") == 0) {
                            valign = Cell::CENTER;
                        } else if(strcmp(value, "bottom") == 0) {
                            valign = Cell::BOTTOM;
                        } else {
                            std::cerr << "Skipping unknown valignment value '"
                                << value << "'.\n";
                        }
                    } else {
                        std::cerr << "Unknown attribute '" << name
                            << "' in cell element.\n";
                    }
                }
                row--;
                col--;
                if(row < 0 || row >= rows) {
                    std::cerr 
                        << "Skipping cell because row value is invalid.\n";
                    continue;
                }
                if(col < 0 || col >= cols) {
                    std::cerr
                        << "Skipping cell because col value is invalid.\n";
                    continue;
                }
                if(rowspan <= 0 || row + rowspan - 1 >= rows) {
                    std::cerr << "rowspan value invalid.\n";
                    rowspan = 1;
                }
                if(colspan <= 0 || col + colspan - 1 >= cols) {
                    std::cerr << "colspan value invalid.\n";
                    colspan = 1;
                }

                Component* component = parseEmbeddedComponent(reader);
                if(component == 0) {
                    std::cerr << "No Component specified in cell "
                        << (row+1) << ", " << (col+1) << "\n";
                    continue;
                }
                addChild(component);
                Cell cell(childs.size() - 1);
                cell.halign = halign;
                cell.valign = valign;
                cell.rowspan = rowspan;
                cell.colspan = colspan;
                cells[row*cols + col] = cell;
            } else {
                std::cerr << "Unknown element '" << element 
                    << "' in TableLayout.\n";
                reader.nextNode();
                continue;
            }
        }   
    }
}

/**
 * Check if a given component, identified by its position, is opaque or not.
 *
 * @param pos Constant vector representing the component's position.
 * @return True if the component is opaque at this place.
 * @todo Remove code duplication with SwitchComponent::opaque (pos) and
 *       Panel::opaque(pos).
 */
bool
TableLayout::opaque(const Vector2& pos) const
{
    for(Childs::const_iterator i = childs.begin(); i != childs.end(); ++i) {
        const Child& child = *i;
        if(child.getComponent() == 0 || !child.isEnabled())
            continue;

        if(child.getComponent()->opaque(pos - child.getPos())) {
            return true;
        }
    }

    return false;
}

void
TableLayout::removeComponents()
{
    cells.clear();
    childs.clear();
}

int
TableLayout::parseProperties(XmlReader& reader, RowColProperties& props)
{
    props.type = RowColProperties::TYPE_RELATIVE;

    int num = -1;
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();
        if(strcmp(name, "fixed") == 0) {
            props.type = RowColProperties::TYPE_FIXEDSIZE;
            if(sscanf(value, "%f", &props.val) != 1) {
                std::cerr << "Eror parsing float value '"
                    << value << "' in fixed attribute.\n";
            }
        } else if(strcmp(name, "relative") == 0) {
            props.type = RowColProperties::TYPE_RELATIVE;
            if(sscanf(value, "%f", &props.val) != 1) {
                std::cerr << "Error parsing float vluae '"
                    << value << "' in relative attribute.\n";
            }
        } else if(strcmp(name, "row") == 0 || strcmp(name, "col") == 0) {
            if(sscanf(value, "%d", &num) != 1) {
                std::cerr << "Error parsing int value '"
                    << value << "' in row or col attribute.\n";
            }
        } else {
            std::cerr << "Unknown attribute '" << name 
                << "' in colsize/rowsize element.\n";
        }
    }

    return num;
}

void
TableLayout::resize(float width, float height)
{
    this->width = width;
    this->height = height;

    float fixedwidth = 0, fixedheight = 0;
    float remainingwidth = 0, remainingheight = 0;

    // Step1: assign all fixed sizes
    for(Properties::iterator i = rowproperties.begin();
            i != rowproperties.end(); ++i) {
        if(i->type == RowColProperties::TYPE_FIXEDSIZE) {
            fixedheight += i->val;
            i->realval = i->val;
        } else {
            remainingheight += i->val;
        }
    }
    for(Properties::iterator i = colproperties.begin();
            i != colproperties.end(); ++i) {
        if(i->type == RowColProperties::TYPE_FIXEDSIZE) {
            fixedwidth += i->val;
            i->realval = i->val;
        } else {
            remainingwidth += i->val;
        }
    }

    // Step2: distribute remaining space to remaining rows/cols
    float heightfact;
    if(remainingheight <= 0)
        heightfact = 0;                                           
    else
        heightfact = (height - fixedheight) / remainingheight;

    for(Properties::iterator i = rowproperties.begin();
            i != rowproperties.end(); ++i) {
        if(i->type == RowColProperties::TYPE_RELATIVE) {
            i->realval = heightfact * i->val;
        }
    }
    
    float widthfact;
    if(remainingwidth <= 0)
        widthfact = 0;
    else
        widthfact = (width - fixedwidth) / remainingwidth;

    for(Properties::iterator i = colproperties.begin();
            i != colproperties.end(); ++i) {
        if(i->type == RowColProperties::TYPE_RELATIVE) {
            i->realval = widthfact * i->val;
        }
    }

    // layout childs
    int r = 0, c;
    Vector2 p;
    for(Properties::iterator row = rowproperties.begin();
        row != rowproperties.end(); ++row) {
        c = 0;
        p.x = 0;
        for(Properties::iterator col = colproperties.begin();
            col != colproperties.end(); ++col) {
            Cell& cell = cells[r * colproperties.size() + c];
            int childid = cell.childid;
            ++c;

            if(childid < 0) {
                p.x += col->realval;
                continue;
            }
            Child& child = childs[childid];
            Component* component = child.getComponent();
			
            if(!component) {
                p.x += col->realval;
                continue;
            }

            float width = 0;
            for(int i = 0; i < cell.colspan; ++i)
                width += (col+i)->realval;
            float height = 0;
            for(int i = 0; i < cell.rowspan; ++i)
                height += (row+i)->realval;
			
            if(component->getFlags() & FLAG_RESIZABLE)
                component->resize(width, height);
#ifdef DEBUG
            if(! (component->getFlags() & FLAG_RESIZABLE) 
                    && (component->getWidth() <= 0 
                        || component->getHeight() <= 0))
                std::cerr << "Warning: component with name '"                                  
                    << component->getName() 
                    << "' has invalid width/height but is not resizable.\n";
#endif

            Vector2 pos = p;
            switch(cell.halign) {
                case Cell::LEFT:
                    break;
                case Cell::CENTER:
                    pos.x += (width - component->getWidth()) / 2;
                    break;
                case Cell::RIGHT:
                    pos.x += width - component->getWidth();
                    break;
                default:
                    assert(false);
            }
            switch(cell.valign) {
                case Cell::TOP:
                    break;
                case Cell::CENTER:
                    pos.y += (height - component->getHeight()) / 2;
                    break;
                case Cell::BOTTOM:
                    pos.y += height - component->getHeight();
                    break;
                default:
                    assert(false);
            }
            child.setPos(pos);

            p.x += col->realval;
        }
        ++r;
        p.y += row->realval;
    }

    setDirty();
}

void
TableLayout::draw(Painter& painter)
{
    Component::draw(painter);
    
    if(border) {
        float r = 0;
        float c = 0;
        painter.setLineColor(Color(0, 0, 255));
        for(size_t row = 0; row < rowproperties.size(); ++row) {
            float nextr = r + rowproperties[row].realval;
            for(size_t col = 0; col < colproperties.size(); ++col) {
                float nextc = c + colproperties[col].realval;
                painter.drawRectangle(Rect2D(c, r, nextc, nextr));
                c = nextc;
            }
            r = nextr;
            c = 0;
        }
    }
}

void
TableLayout::addRow(const RowColProperties& props)
{
    removeComponents();
    rowproperties.push_back(props);
    cells.assign(rowproperties.size() * colproperties.size(), Cell());
}

void
TableLayout::addColumn(const RowColProperties& props)
{
    removeComponents();
    colproperties.push_back(props);
    cells.assign(rowproperties.size() * colproperties.size(), Cell());
}

void
TableLayout::addComponent(size_t col, size_t row, Component* component)
{
    if(row >= rowproperties.size())
        throw std::runtime_error("row out of range");
    if(col >= colproperties.size())
        throw std::runtime_error("col out of range");

    if(cells[row * colproperties.size() + col].childid >= 0)
        throw std::runtime_error("Already a component in this cell.");
    
    addChild(component);
    cells[row * colproperties.size() + col] = Cell(childs.size()-1);
}

IMPLEMENT_COMPONENT_FACTORY(TableLayout)


/** @file gui/TableLayout.cpp */

