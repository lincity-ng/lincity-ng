#include "Document.hpp"

#include <iostream>

#include "Paragraph.hpp"
#include "XmlReader.hpp"
#include "Vector2.hpp"
#include "Painter.hpp"
#include "ComponentFactory.hpp"

Document::Document(Component* parent, XmlReader& reader)
    : Component(parent)
{
    parse(reader);
}

Document::~Document()
{
}

void
Document::parse(XmlReader& reader)
{
    std::cout << "parse...\n";

    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else if(style.parseAttribute(attribute, value)) {
            continue;
        } else if(strcmp(attribute, "src") == 0) {
            XmlReader fileReader(value);
            parse(fileReader);
            return;
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }

    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string node = (const char*) reader.getName();
            if(node == "p") {
                Paragraph* paragraph = new Paragraph(this, reader, style);
                addChild(paragraph);
            } else {
                std::cerr << "Skipping unknown node type '" << node << "'.\n";
                reader.nextNode();
            }
        } else if(reader.getNodeType() == XML_READER_TYPE_TEXT) {
            // TODO create anonymous paragraph...
            std::cerr << "Warning: text outside paragraph not allowed (yet).\n";
        }
    }

    setFlags(FLAG_RESIZABLE);
}

void
Document::resize(float newwidth, float )
{
    height = 0;
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        Component* component = child.getComponent();
        component->resize(newwidth, -1);
        child.position = Vector2(0, height);
        height += component->getHeight();
    }
    width = newwidth;
}

void
Document::draw(Painter& painter)
{
    painter.setFillColor(style.background);
    painter.fillRectangle(Rect2D(0, 0, width, height));

    Component::draw(painter);
}
                

IMPLEMENT_COMPONENT_FACTORY(Document);
