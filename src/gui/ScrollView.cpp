#include <config.h>

#include "ScrollView.hpp"

#include <memory>

#include "XmlReader.hpp"
#include "ScrollBar.hpp"
#include "ComponentFactory.hpp"
#include "ComponentLoader.hpp"
#include "callback/Callback.hpp"

ScrollView::ScrollView()
{
}

ScrollView::~ScrollView()
{
}

void
ScrollView::parse(XmlReader& reader)
{
    // parse xml attributes
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }

    // we need 2 child components
    childs.assign(2, Child());

    // parse xml contents
    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string element = (const char*) reader.getName();
            
            if(element == "scrollbar") {
                std::auto_ptr<ScrollBar> scrollbar (new ScrollBar());
                scrollbar->parse(reader);
                resetChild(scrollBar(), scrollbar.release());
            } else if(element == "contents") {
                resetChild(contents(), parseEmbeddedComponent(reader));
            } else {
                std::cerr << "Skipping unknown element '" << element << "'.\n";
            }
        }
    }

    if(scrollBar().getComponent() == 0) {
        throw std::runtime_error("No ScrollBar specified in ScrollView");
    }
    ScrollBar* scrollBarComponent = (ScrollBar*) scrollBar().getComponent();
    scrollBarComponent->valueChanged.connect(
            makeCallback(*this, &ScrollView::scrollBarChanged));

    setFlags(FLAG_RESIZABLE);
}

void
ScrollView::resize(float newwidth, float newheight)
{
    float scrollBarWidth = scrollBar().getComponent()->getWidth();
    scrollBar().getComponent()->resize(scrollBarWidth, newheight);
    scrollBar().setPos(Vector2(newwidth - scrollBarWidth, 0));
    
    if(contents().getComponent()->getFlags() & FLAG_RESIZABLE)
        contents().getComponent()->resize(newwidth - scrollBarWidth, newheight);
    contents().setClipRect(Rect2D(0, 0, newwidth - scrollBarWidth, newheight));
    float scrollarea = contents().getComponent()->getHeight() - newheight;
    if(scrollarea < 0)
        scrollarea = 0;

    ScrollBar* scrollBarComponent = (ScrollBar*) scrollBar().getComponent();
    scrollBarComponent->setRange(0, scrollarea);

    width = newwidth;
    height = newheight;

    setDirty();
}

void
ScrollView::scrollBarChanged(ScrollBar* , float newvalue)
{
    contents().setPos(Vector2(0, -newvalue));
    setDirty();
}

IMPLEMENT_COMPONENT_FACTORY(ScrollView)
