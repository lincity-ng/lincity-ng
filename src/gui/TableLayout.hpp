#ifndef __TABLELAYOUT_HPP__
#define __TABLELAYOUT_HPP__

#include <vector>
#include "Component.hpp"

class XmlReader;

class TableLayout : public Component
{
public:
    TableLayout(Component* parent);
    TableLayout(Component* parent, XmlReader& reader);
    ~TableLayout();

    void draw(Painter& painter);
    void event(Event& event);
    void resize(float width, float height);

    struct RowColProperties
    {
        RowColProperties()
            : type(RELATIVE), val(1)
        {
        }

        enum Type { FIXEDSIZE, RELATIVE };
        Type type;
        float val;
        float realval;
    };
    void addRow(const RowColProperties& props);
    void addColumn(const RowColProperties& props);
    void setComponent(size_t col, size_t row, Component* component);

private:
    void removeComponents();
    int parseProperties(XmlReader& reader, RowColProperties& props);

    typedef std::vector<RowColProperties> Properties;
    Properties rowproperties;
    Properties colproperties;

    typedef std::vector<Component*> Childs;
    Childs childs;
};

#endif

