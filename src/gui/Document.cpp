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
    for(Components::iterator i = components.begin(); i != components.end(); ++i)
        delete *i;
}

void
Document::parse(XmlReader& reader)
{
    std::cout << "parse...\n";

    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(style.parseStyleAttribute(attribute, value))
            continue;
        if(strcmp(attribute, "src") == 0) {
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
                components.push_back(paragraph);
            } else {
                std::cerr << "Skipping unknown node type '" << node << "'.\n";
                reader.nextNode();
            }
        } else if(reader.getNodeType() == XML_READER_TYPE_TEXT) {
            // TODO create anonymous paragraph...
            std::cerr << "Warning: text outside paragraph not allowed (yet).\n";
        }
    }

    setFlags(getFlags() | FLAG_RESIZABLE);
}

void
Document::resize(float newwidth, float newheight)
{
    height = 0;
    for(Components::iterator i = components.begin();
            i != components.end(); ++i) {
        Component* component = *i;
        component->resize(newwidth, -1);
        height += component->getHeight();
    }
    width = newwidth;
}

void
Document::draw(Painter& painter)
{
    painter.setFillColor(style.background);
    painter.fillRectangle(Rectangle(0, 0, width, height));
    
    Vector2 translation;    
    for(Components::iterator i = components.begin();
            i != components.end(); ++i) {
        Component* component = *i;
        
        painter.pushTransform();
        painter.translate(translation);
        component->draw(painter);
        painter.popTransform();
        translation.y += component->getHeight();
    }
}

void
Document::event(Event& event)
{
}

IMPLEMENT_COMPONENT_FACTORY(Document);
