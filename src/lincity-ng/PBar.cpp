#include "PBar.hpp"
#include "Util.hpp"
#include "gui/ComponentFactory.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/XmlReader.hpp"
#include "gui/Paragraph.hpp"

LCPBar* LCPBarInstance = 0;

LCPBar::LCPBar(Component* parent, XmlReader& reader)
    : Component(parent)
{
    Component* component = parseEmbeddedComponent(this, reader);
    if(component)
        addChild(component);

    width = component->getWidth();
    height = component->getHeight();
    
    LCPBarInstance = this;
}

LCPBar::~LCPBar()
{
    if(LCPBarInstance == this)
        LCPBarInstance = 0;
}

void
LCPBar::setValue(int num, int value)
{
    std::ostringstream compname;
    compname << "pbar_text" << (num+1);
    Paragraph* p = getParagraph(*this, compname.str());

    std::ostringstream os;
    os<<std::fixed;
    os<<std::setprecision(1);
    if(num==PTECH)
        os<<value/10000.0;
    else if(num==PMONEY)
    {
        if(value>1000000)
            os<<value/1000000.0<<"M";
        else if(value>1000)
            os<<value/1000.0<<"K";
        else
            os<<value;
    }
    else
        os<<value;
    
    p->setText(os.str());
}

IMPLEMENT_COMPONENT_FACTORY(LCPBar)
