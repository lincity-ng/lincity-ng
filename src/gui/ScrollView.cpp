#include "ScrollView.hpp"

#include <sigc++/signal.h>

#include "XmlReader.hpp"
#include "ScrollBar.hpp"
#include "ComponentFactory.hpp"
#include "ComponentLoader.hpp"

ScrollView::ScrollView(Component* parent, XmlReader& reader)
    : Component(parent)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttributeValue(attribute, value)) {
            continue;
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }

    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string element = (const char*) reader.getName();
            
            if(element == "scrollbar") {
                scrollBar.setComponent(new ScrollBar(this, reader));
            } else if(element == "contents") {
                contents.setComponent(parseEmbeddedComponent(this, reader));
            } else {
                std::cerr << "Skipping unknown element '" << element << "'.\n";
            }
        }
    }

    if(scrollBar.getComponent() == 0) {
        throw std::runtime_error("No ScrollBar specified in ScrollView");
    }
    scrollBar.getComponent()->signalValueChanged.connect(
            sigc::mem_fun(*this, &ScrollView::scrollBarChanged));

    setFlags(getFlags() | FLAG_RESIZABLE);
}

ScrollView::~ScrollView()
{
}

void
ScrollView::resize(float newwidth, float newheight)
{
    float scrollBarWidth = scrollBar.getComponent()->getWidth();
    scrollBar.resize(scrollBarWidth, newheight);
    scrollBar.setPos(Vector2(newwidth - scrollBarWidth, 0));
    
    contents.resize(newwidth - scrollBarWidth, newheight);
    float scrollarea = contents.getComponent()->getHeight() - newheight;
    if(scrollarea < 0)
        scrollarea = 0;
    scrollBar.getComponent()->setRange(0, scrollarea);

    width = newwidth;
    height = newheight;
}

void
ScrollView::scrollBarChanged(ScrollBar* bar, float newvalue)
{
    contents.setPos(Vector2(0, -newvalue));
}

void
ScrollView::draw(Painter& painter)
{
    float scrollBarWidth = scrollBar.getComponent()->getWidth();
    painter.setClipRectangle(Rectangle(0, 0, width - scrollBarWidth, height));
    contents.draw(painter);
    painter.clearClipRectangle();
    
    scrollBar.draw(painter);
}

void
ScrollView::event(Event& event)
{
    contents.event(event);
    scrollBar.event(event);
}

IMPLEMENT_COMPONENT_FACTORY(ScrollView)
