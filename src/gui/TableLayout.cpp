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
                int spancols = 1, spanrows = 1;
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
                    } else if(strcmp(name, "spanrows") == 0) {
                        if(sscanf(value, "%d", &spanrows) != 1) {
                            std::cerr << "Couldn't parse integer value '"
                                << value << "' in spanrows attribute.\n";
                        }
                    } else if(strcmp(name, "spancols") == 0) {
                        if(sscanf(value, "%d", &spancols) != 1) {
                            std::cerr << "Couldn't parse integer value '"
                                << value << "' in spancols attribute.\n";
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
                if(spanrows <= 0 || row + spanrows - 1 >= rows) {
                    std::cerr << "spanrows value invalid.\n";
                    spanrows = 1;
                }
                if(spancols <= 0 || col + spancols - 1 >= cols) {
                    std::cerr << "spancols value invalid.\n";
                    spancols = 1;
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
                cell.spanrows = spanrows;
                cell.spancols = spancols;
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
            for(int i = 0; i < cell.spancols; ++i)
                width += (col+i)->realval;
            float height = 0;
            for(int i = 0; i < cell.spanrows; ++i)
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
