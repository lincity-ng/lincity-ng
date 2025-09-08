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
#include "TableLayout.hpp"

#include <assert.h>                       // for assert
#include <fmt/base.h>                     // for println
#include <fmt/format.h>                   // for format
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <stdio.h>                        // for stderr
#include <iostream>                       // for basic_ostream, operator<<
#include <stdexcept>                      // for runtime_error
#include <string>                         // for basic_string, operator==
#include <utility>                        // for move

#include "Child.hpp"                      // for Childs, Child
#include "Color.hpp"                      // for Color
#include "ComponentFactory.hpp"           // for IMPLEMENT_COMPONENT_FACTORY
#include "ComponentLoader.hpp"            // for parseEmbeddedComponent
#include "Painter.hpp"                    // for Painter
#include "Rect2D.hpp"                     // for Rect2D
#include "Vector2.hpp"                    // for Vector2
#include "util/xmlutil.hpp"               // for xmlParse, unexpectedXmlAttr...

using namespace std::string_literals;

TableLayout::TableLayout()
{
    setFlags(FLAG_RESIZABLE);
}

TableLayout::~TableLayout()
{
}

void
TableLayout::parse(xmlpp::TextReader& reader) {
  int rows = -1, cols = -1;
  border = false;

  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else if(name == "rows")
      rows = xmlParse<int>(value);
    else if(name == "cols")
      cols = xmlParse<int>(value);
    else if(name == "border")
      border = xmlParse<bool>(value);
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  if(rows <= 0 || cols <= 0)
    throw std::runtime_error("Invalid rows/cols");

  rowproperties.assign(rows, RowColProperties());
  colproperties.assign(cols, RowColProperties());
  cells.assign(rows*cols, Cell());

  if(!reader.is_empty_element() && reader.read())
  while(reader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      reader.next();
      continue;
    }
    xmlpp::ustring element = reader.get_name();
    if(element == "rowsize") {
      parseRowColProperties(reader, true);
    } else if(element == "colsize") {
      parseRowColProperties(reader, false);
    } else if(element == "cell") {
      int row, col;
      int colspan = 1, rowspan = 1;
      Cell::Alignment halign = Cell::CENTER;
      Cell::Alignment valign = Cell::CENTER;

      while(reader.move_to_next_attribute()) {
        xmlpp::ustring name = reader.get_name();
        xmlpp::ustring value = reader.get_value();
        if(parseAttribute(reader));
        else if(name == "row")
          row = xmlParse<int>(value) - 1;
        else if(name == "col")
          col = xmlParse<int>(value) - 1;
        else if(name == "rowspan")
          rowspan = xmlParse<int>(value);
        else if(name == "colspan")
          colspan = xmlParse<int>(value);
        else if(name == "halign") {
          if(value == "left")
            halign = Cell::LEFT;
          else if(value == "center")
            halign = Cell::CENTER;
          else if(value == "right")
            halign = Cell::RIGHT;
          else
            throw std::runtime_error(fmt::format(
              "invalid horizontal alignment {:?}"s +
              ". Expected 'left', 'center', or 'right'.",
              value
            ));
        }
        else if(name == "valign") {
          if(value == "top")
            valign = Cell::TOP;
          else if(value == "center")
            valign = Cell::CENTER;
          else if(value == "bottom")
            valign = Cell::BOTTOM;
          else
            throw std::runtime_error(fmt::format(
              "invalid vertical alignment {:?}"s +
              ". Expected 'top', 'center', or 'bottom'.",
              value
            ));
        }
        else
          unexpectedXmlAttribute(reader);
      }
      reader.move_to_element();

      if(row < 0 || row >= rows)
        throw std::runtime_error(fmt::format("invalid row: {}", row));
      if(col < 0 || col >= cols)
        throw std::runtime_error(fmt::format("invalid column: {}", col));
      if(rowspan < 0 || row + rowspan > rows)
        throw std::runtime_error(fmt::format("invalid rowspan: {}", rowspan));
      if(colspan < 0 || col + colspan > cols)
        throw std::runtime_error(fmt::format("invalid colspan: {}", colspan));

      std::unique_ptr<Component> component = parseEmbeddedComponent(reader);
      if(!component)
        throw std::runtime_error(fmt::format(
          "no component in cell ({}, {})", row + 1, col + 1));
      addChild(std::move(component));
      Cell cell(childs.size() - 1);
      cell.halign = halign;
      cell.valign = valign;
      cell.rowspan = rowspan;
      cell.colspan = colspan;
      cells[row*cols + col] = cell;
    } else {
      unexpectedXmlElement(reader);
    }
    reader.next();
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

void
TableLayout::parseRowColProperties(xmlpp::TextReader& reader, bool isRow) {
  RowColProperties props;
  props.type = RowColProperties::TYPE_RELATIVE;
  int num = -1;

  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(name == "fixed") {
      props.type = RowColProperties::TYPE_FIXEDSIZE;
      props.val = xmlParse<float>(value);
    }
    else if(name == "relative") {
      props.type = RowColProperties::TYPE_RELATIVE;
      props.val = xmlParse<float>(value);
    }
    else if(name == (isRow ? "row" : "col"))
      num = xmlParse<int>(value) - 1;
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  if(num < 0 || num >= (isRow ? rowproperties : colproperties).size()) {
    fmt::println(stderr, "error: invalid {} specied: {}",
      isRow ? "row" : "col", num);
    assert(false);
    return;
  }

  (isRow ? rowproperties : colproperties)[num] = props;
}

void
TableLayout::resize(float width, float height)
{
    if(width < 0) width = 0;
    if(height < 0) height = 0;
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
                // TODO: honor minimum sizes of children
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
TableLayout::addComponent(size_t col, size_t row,
  std::unique_ptr<Component>&& component
) {
    if(row >= rowproperties.size())
        throw std::runtime_error("row out of range");
    if(col >= colproperties.size())
        throw std::runtime_error("col out of range");

    if(cells[row * colproperties.size() + col].childid >= 0)
        throw std::runtime_error("Already a component in this cell.");

    addChild(std::move(component));
    cells[row * colproperties.size() + col] = Cell(childs.size()-1);
}

IMPLEMENT_COMPONENT_FACTORY(TableLayout)


/** @file gui/TableLayout.cpp */
