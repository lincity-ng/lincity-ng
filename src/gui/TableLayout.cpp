#include <config.h>

#include "TableLayout.hpp"

#include <stdexcept>
#include <sstream>
#include <iostream>

#include "Painter.hpp"
#include "Event.hpp"
#include "ComponentFactory.hpp"
#include "XmlReader.hpp"

TableLayout::TableLayout(Component* parent)
    : Component(parent)
{
    setFlags(getFlags() | FLAG_RESIZABLE);
}

TableLayout::TableLayout(Component* parent, XmlReader& reader)
    : Component(parent)
{
    int rows = -1, cols = -1;
    
    std::string val = reader.getAttribute("rows");
    if(sscanf(val.c_str(), "%d", &rows) != 1) {
        std::stringstream msg;
        msg << "Error while parsing rows attribute: " << val;
        throw std::runtime_error(msg.str());
    }
    val = reader.getAttribute("cols");
    if(sscanf(val.c_str(), "%d", &cols) != 1) {
        std::stringstream msg;
        msg << "Error while parsing cols attribute: " << val;
        throw std::runtime_error(msg.str());
    }
    if(rows <= 0 || cols <= 0) {
        throw std::runtime_error("Invalid values for rows/cols");
    }
    
    rowproperties.assign(rows, RowColProperties());
    colproperties.assign(cols, RowColProperties());
    childs.assign(rows*cols, (Component*) 0);
    
    try {
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
                    if(num < 0 || num >= rows) {
                        std::cerr 
                            << "Invalid col specified in colsize element.\n";
                        continue;
                    }
                    colproperties[num] = props;
                } else if(element == "cell") {
                    int row = -1, col = -1;
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
                    
                    Component* component = parseEmbeddedComponent(this, reader);
                    if(component == 0) {
                        std::cerr << "No Component specified in cell "
                            << (row+1) << ", " << (col+1) << "\n";
                        continue;
                    }
                    childs[row*cols + col] = component;
                } else {
                    std::cerr << "Unknown element '" << element 
                        << "' in TableLayout.\n";
                    reader.nextNode();
                    continue;
                }
            }   
        }
    } catch(...) {
        for(Childs::iterator i = childs.begin(); i != childs.end(); ++i)
            delete *i;
        throw;
    }

    setFlags(getFlags() | FLAG_RESIZABLE);
}

TableLayout::~TableLayout()
{
    removeComponents();
}

void
TableLayout::removeComponents()
{
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        delete *i;
        *i = 0;
    }
}

int
TableLayout::parseProperties(XmlReader& reader, RowColProperties& props)
{
    props.type = RowColProperties::RELATIVE;

    int num = -1;
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();
        if(strcmp(name, "fixed") == 0) {
            props.type = RowColProperties::FIXEDSIZE;
            if(sscanf(value, "%f", &props.val) != 1) {
                std::cerr << "Eror parsing float value '"
                    << value << "' in fixed attribute.\n";
            }
        } else if(strcmp(name, "relative") == 0) {
            props.type = RowColProperties::RELATIVE;
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
        if(i->type == RowColProperties::FIXEDSIZE) {
            fixedheight += i->val;
            i->realval = i->val;
        } else {
            remainingheight += i->val;
        }
    }
    for(Properties::iterator i = colproperties.begin();
            i != colproperties.end(); ++i) {
        if(i->type == RowColProperties::FIXEDSIZE) {
            fixedwidth += i->val;
            i->realval = i->val;
        } else {
            remainingwidth += i->val;
        }
    }

    printf("FH: %f RemH: %f.\n", fixedheight, remainingheight);
    printf("FW: %f RemW: %f.\n", fixedwidth, remainingwidth);

    // Step2: distribute remaining space to remaining rows/cols
    float heightfact;
    if(remainingheight <= 0)
        heightfact = 0;                                           
    else
        heightfact = (height - fixedheight) / remainingheight;

    for(Properties::iterator i = rowproperties.begin();
            i != rowproperties.end(); ++i) {
        if(i->type == RowColProperties::RELATIVE) {
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
        if(i->type == RowColProperties::RELATIVE) {
            i->realval = widthfact * i->val;
        }
    }

    // resize childs
    int r = 0, c;
    for(Properties::iterator row = rowproperties.begin();
        row != rowproperties.end(); ++row) {
        c = 0;
        for(Properties::iterator col = colproperties.begin();
            col != colproperties.end(); ++col) {
            Component* component = childs[r * colproperties.size() + c];
            ++c;
            if(!component || !(component->getFlags() & FLAG_RESIZABLE))
                continue;
            component->resize(col->realval, row->realval);
        }
        ++r;
    }
}

void
TableLayout::draw(Painter& painter)
{
    int r = 0, c;
    float x = 0, y = 0;

    for(Properties::iterator row = rowproperties.begin();
        row != rowproperties.end(); ++row) {
        x = 0;
        c = 0;
        for(Properties::iterator col = colproperties.begin();
            col != colproperties.end(); ++col) {
            Component* component = childs[r * colproperties.size() + c];
            ++c;
            if(!component) {
                x += col->realval;
                continue;
            }

            painter.pushTransform();
            Vector2 translation(
                    x + (col->realval - component->getWidth())/2,
                    y + (row->realval - component->getHeight())/2);
            painter.translate(translation);
            component->draw(painter);
            painter.popTransform();

            x += col->realval;
        }
        ++r;
        y += row->realval;
    }
}

void
TableLayout::event(Event& event)
{
    switch(event.type) {
        // these events need the position changed relatived to the table cells
        case Event::MOUSEMOTION:
        case Event::MOUSEBUTTONDOWN:
        case Event::MOUSEBUTTONUP: {
            int r = 0, c;
            float x = 0, y = 0;

            for(Properties::iterator row = rowproperties.begin();
                row != rowproperties.end(); ++row) {
                x = 0;
                c = 0;
                for(Properties::iterator col = colproperties.begin();
                    col != colproperties.end(); ++col) {
                    Component* component = childs[r * colproperties.size() + c];
                    ++c;
                    if(!component) {
                        x += col->realval;
                        continue;
                    }

                    Vector2 oldpos = event.mousepos;
                    Vector2 translation(
                        x + (col->realval - component->getWidth())/2,
                        y + (row->realval - component->getHeight())/2);
                    event.mousepos -= translation;
                    component->event(event);
                    event.mousepos = oldpos;

                    x += col->realval;
                }                                                                                
                ++r;
                y += row->realval;
            }
            break;
        }
        default:
            for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
                (*i)->event(event);
            }
            break;
    }
}

void
TableLayout::addRow(const RowColProperties& props)
{
    removeComponents();
    rowproperties.push_back(props);
    childs.assign(rowproperties.size() * colproperties.size(), (Component*) 0);
}

void
TableLayout::addColumn(const RowColProperties& props)
{
    removeComponents();
    colproperties.push_back(props);
    childs.assign(rowproperties.size() * colproperties.size(), (Component*) 0);
}

void
TableLayout::setComponent(size_t col, size_t row, Component* component)
{
    if(row >= rowproperties.size())
        throw std::runtime_error("row out of range");
    if(col >= colproperties.size())
        throw std::runtime_error("col out of range");
 
    delete (childs[row * colproperties.size() + col]);
    childs[row * colproperties.size() + col] = component;
}

IMPLEMENT_COMPONENT_FACTORY(TableLayout)
