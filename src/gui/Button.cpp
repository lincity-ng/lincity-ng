#include <config.h>

#include <stdexcept>

#include "Painter.hpp"
#include "Event.hpp"
#include "TextureManager.hpp"
#include "Button.hpp"
#include "Image.hpp"
#include "Paragraph.hpp"
#include "TooltipManager.hpp"
#include "ComponentFactory.hpp"
#include "XmlReader.hpp"

static const Uint32 TOOLTIP_TIME = 500;

Button::Button()
    : state(STATE_NORMAL), lowerOnClick(false), mouseholdTicks(0)
{
}

void
Button::parse(XmlReader& reader)
{
    // parse xml attributes
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else if(strcmp(attribute, "width") == 0) {
            if(sscanf(value, "%f", &width) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse width '" << value << "'.";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(attribute, "height") == 0) {
            if(sscanf(value, "%f", &height) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse height '" << value << "'.";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(attribute, "lower") == 0) {
           lowerOnClick=true;
        } else if(strcmp(attribute, "direction") == 0) {
            // skip
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }

    // we need 4 child components
    childs.assign(4, Child());
    
    // parse contents of the xml-element
    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string element = (const char*) reader.getName();
            if(element == "image") {
                if(comp_normal().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for state "
                        "comp_normal defined.\n";
                setChildImage(comp_normal(), reader);
            } else if(element == "text") {
                if(comp_normal().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for state "
                        "comp_normal defined.\n";
                setChildText(comp_normal(), reader);
            } else if(element == "image-hover") {
                if(comp_hover().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for state "
                        "comp_hover defined.\n";
                setChildImage(comp_hover(), reader);
            } else if(element == "text-hover") {
                if(comp_hover().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for state "
                        "comp_hover defined.\n";
                setChildText(comp_hover(), reader);
            } else if(element == "image-clicked") {
                if(comp_clicked().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for state "
                        "comp_clicked defined.\n";
                setChildImage(comp_clicked(), reader);
            } else if(element == "text-clicked") {
                if(comp_clicked().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for state "
                        "comp_clicked defined.\n";
                setChildText(comp_clicked(), reader);
            } else if(element == "image-caption") {
                if(comp_caption().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for comp_caption "
                        "defined.\n";
                setChildImage(comp_caption(), reader);
            } else if(element == "text-caption") {
                if(comp_caption().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for comp_caption "
                        "defined.\n";
                setChildText(comp_caption(), reader);
            } else if (element == "tooltip") {
                tooltip = "Blup";
            } else {
                std::cerr << "Skipping unknown element '" << element << "'.\n";
            }
        }
    }

    if(comp_normal().getComponent() == 0)
        throw std::runtime_error("No component for state comp_normal defined.");

    // if no width/height was specified we use the one from the biggest image
    if(width <= 0 || height <= 0) {
        for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
            Component* component = i->getComponent();
            if(!component)
                continue;
            if(component->getFlags() & FLAG_RESIZABLE)
                component->resize(-1, -1);

            if(component->getWidth() > width)
                width = component->getWidth();
            if(component->getHeight() > height)
                height = component->getHeight();
        }
    }

    // place components at the middle of the button
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        if(!child.getComponent())
            continue;
        Component* component = child.getComponent();
        
        child.setPos( Vector2 ((width - component->getWidth())/2,
                               (height - component->getHeight())/2));
    }
}

void
Button::setChildImage(Child& child, XmlReader& reader)
{
    std::auto_ptr<Image> image(new Image());
    image->parse(reader);
    resetChild(child, image.release());
}

void
Button::setChildText(Child& child, XmlReader& reader)
{
    std::auto_ptr<Paragraph> paragraph(new Paragraph());
    paragraph->parse(reader);
    resetChild(child, paragraph.release());
}

void Button::setCaptionText(const std::string &pText)
{
    Child &c=comp_caption();
    Component *cm=c.getComponent();
    if(cm)
    {
        Paragraph *p=dynamic_cast<Paragraph*>(cm);
        if(p)
            p->setText(pText);
    }

    // place components at the middle of the button
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        if(!child.getComponent())
            continue;
        Component* component = child.getComponent();
        
        child.setPos( Vector2 ((width - component->getWidth())/2,
                    (height - component->getHeight())/2));
    }
}

std::string Button::getCaptionText()
{
  std::string s;
  Child &c=comp_caption();
  Component *cm=c.getComponent();
  if(cm)
  {
    Paragraph *p=dynamic_cast<Paragraph*>(cm);
    if(p)
      s=p->getText();
  }  
  
  return s;
}


Button::~Button()
{
}

void
Button::event(const Event& event)
{
    State oldState = state;
    switch(event.type) {
        case Event::MOUSEMOTION:
            if(event.inside) {
                if(state == STATE_NORMAL) {
                    state = STATE_HOVER;
                }
                mouseholdTicks = SDL_GetTicks();
                mouseholdPos = event.mousepos;
            } else {
                mouseholdTicks = 0;
                if(state == STATE_HOVER) {
                    state = STATE_NORMAL;
                }
            }
            break;
        case Event::MOUSEBUTTONDOWN:
            if(event.inside && event.mousebutton != SDL_BUTTON_WHEELUP &&
                    event.mousebutton != SDL_BUTTON_WHEELDOWN) {
                state = STATE_CLICKED;
            } else {
                state = STATE_NORMAL;
            }
            break;
        case Event::MOUSEBUTTONUP:
            if(event.inside && state == STATE_CLICKED) {
                clicked(this);
            }
            state = event.inside ? STATE_HOVER : STATE_NORMAL;
            break;
        case Event::UPDATE: {
                Uint32 ticks = SDL_GetTicks();
                if(mouseholdTicks != 0 && ticks - mouseholdTicks > TOOLTIP_TIME) {
                    if(tooltipManager && tooltip != "") {
                        tooltipManager->showTooltip(tooltip,
                                relative2Global(mouseholdPos));
                    }
                    mouseholdTicks = 0;
                }
            }
            break;
        default:
            break;
    }
    if(state != oldState)
        setDirty();

    Component::event(event);
}

void
Button::draw(Painter& painter)
{
    switch(state) {
        case STATE_CLICKED:
            if(comp_clicked().isEnabled()) {
                drawChild(comp_clicked(), painter);
                break;
            }
            // fallthrough
        case STATE_HOVER:
            if(comp_hover().isEnabled()) {
                drawChild(comp_hover(), painter);
                break;
            }
            // fallthrough
        case STATE_NORMAL:
            drawChild(comp_normal(), painter);
            break;
            
        default:
            assert(false);
    }
    if(lowerOnClick && state==STATE_CLICKED)
    {
       painter.pushTransform();
       painter.translate(Vector2(1,1));
    }
    if(comp_caption().isEnabled())
        drawChild(comp_caption(), painter);
    if(lowerOnClick && state==STATE_CLICKED)
        painter.popTransform();
}

IMPLEMENT_COMPONENT_FACTORY(Button)
