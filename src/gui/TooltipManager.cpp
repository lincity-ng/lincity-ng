#include <config.h>

#include "TooltipManager.hpp"
#include "XmlReader.hpp"
#include "ComponentFactory.hpp"
#include "Paragraph.hpp"
#include "Event.hpp"

TooltipManager* tooltipManager = 0;

TooltipManager::TooltipManager()
{
    childs.assign(1, Child());
    if(tooltipManager == 0)
        tooltipManager = this;

    setFlags(FLAG_RESIZABLE);
}

TooltipManager::~TooltipManager()
{
    if(tooltipManager == this)
        tooltipManager = 0;
}

void
TooltipManager::parse(XmlReader& reader)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else {
            std::cerr << "Skipping unknown attribute '" << attribute
                      << "' in TooltipManager.\n";
        }
    }

    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            const char* element = (const char*) reader.getName();
            std::cerr << "Skipping unknown child '" << element
                      << "in TooltipManager.\n";
        }
    }
}

void
TooltipManager::resize(float width, float height)
{
    printf("Resized.\n");
    this->width = width;
    this->height = height;
}

bool
TooltipManager::opaque(const Vector2& ) const
{
    return false;
}

void
TooltipManager::event(const Event& event)
{
    if(event.type == Event::MOUSEMOTION) {
        if(comp_tooltip().getComponent() != 0) {
            printf("Hide tooltip.\n");
            comp_tooltip().setComponent(0);
        }
    }    
}

void
TooltipManager::showTooltip(const std::string& text, const Vector2& pos)
{
    std::auto_ptr<Paragraph> p (new Paragraph());

    std::map<std::string, Style>::iterator s = styleRegistry.find("tooltip");
    if(s == styleRegistry.end()) {
        std::cerr << "Warning: No tooltip style defined.\n";
        p->setText(text);
    } else {
        p->setText(text, s->second);
    }
    p->resize(300, -1);
    printf("Tooltip '%s' at %f,%f.\n", text.c_str(), pos.x, pos.y);
    comp_tooltip().setComponent(p.release());
    comp_tooltip().setPos(Vector2(10, 10));
}

IMPLEMENT_COMPONENT_FACTORY(TooltipManager);
