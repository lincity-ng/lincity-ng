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

TableLayout::TableLayout(Component* parent)
    : Component(parent)
{
    setFlags(FLAG_RESIZABLE);
}

TableLayout::TableLayout(Component* parent, XmlReader& reader)
    : Component(parent)
{
    int rows = -1, cols = -1;
    
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
        } else {
            std::cerr << "Skipping unknown attribute '" << attribute << "'.\n";
        }
    }
    if(rows <= 0 || cols <= 0) {
        throw std::runtime_error("Invalid values for rows/cols");
    }
    
    rowproperties.assign(rows, RowColProperties());
    colproperties.assign(cols, RowColProperties());
    cells.assign(rows*cols, -1);
    
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
                } else {
                    addChild(component);
                    cells[row*cols + col] = childs.size()-1;
                }
            } else {
                std::cerr << "Unknown element '" << element 
                    << "' in TableLayout.\n";
                reader.nextNode();
                continue;
            }
        }   
    }

    setFlags(FLAG_RESIZABLE);
}

TableLayout::~TableLayout()
{
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
        } else if(strcmp(name, "alignment") == 0) {
            if(strcmp(value, "left") == 0) {
                props.alignment = RowColProperties::LEFT;
            } else if(strcmp(value, "center") == 0) {
                props.alignment = RowColProperties::CENTER;
            } else if(strcmp(value, "right") == 0) {
                props.alignment = RowColProperties::RIGHT;
            } else if(strcmp(value, "top") == 0) {
                props.alignment = RowColProperties::TOP;
            } else if(strcmp(value, "bottom") == 0) {
                props.alignment = RowColProperties::BOTTOM;
            } else {
                std::cerr << "Unknown alignment type '" << value << "'.\n";
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

    // layout childs
    int r = 0, c;
    Vector2 p;
    for(Properties::iterator row = rowproperties.begin();
        row != rowproperties.end(); ++row) {
        c = 0;
        p.x = 0;
        for(Properties::iterator col = colproperties.begin();
            col != colproperties.end(); ++col) {
            int childid = cells[r * colproperties.size() + c];
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
            if(component->getFlags() & FLAG_RESIZABLE)
                component->resize(col->realval, row->realval);

            Vector2 pos = p;
            switch(col->alignment) {
                case RowColProperties::LEFT:
                    break;
                case RowColProperties::CENTER:
                    pos.x += (col->realval - component->getWidth()) / 2;
                    break;
                case RowColProperties::RIGHT:
                    pos.x += col->realval - component->getWidth();
                    break;
                default:
                    assert(false);
            }
            switch(row->alignment) {
                case RowColProperties::TOP:
                    break;
                case RowColProperties::CENTER:
                    pos.y += (row->realval - component->getHeight()) / 2;
                    break;
                case RowColProperties::BOTTOM:
                    pos.y += col->realval - component->getHeight();
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
}

void
TableLayout::addRow(const RowColProperties& props)
{
    removeComponents();
    rowproperties.push_back(props);
    cells.assign(rowproperties.size() * colproperties.size(), -1);
}

void
TableLayout::addColumn(const RowColProperties& props)
{
    removeComponents();
    colproperties.push_back(props);
    cells.assign(rowproperties.size() * colproperties.size(), -1);
}

void
TableLayout::addComponent(size_t col, size_t row, Component* component)
{
    if(row >= rowproperties.size())
        throw std::runtime_error("row out of range");
    if(col >= colproperties.size())
        throw std::runtime_error("col out of range");

    if(cells[row * colproperties.size() + col] != 0)
        throw std::runtime_error("Already a component in this cell.");
    
    addChild(component);
    cells[row * colproperties.size() + col] = childs.size()-1;
}

IMPLEMENT_COMPONENT_FACTORY(TableLayout)
