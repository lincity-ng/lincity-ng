#include "CheckButton.hpp"
#include "gui/Event.hpp"
#include "gui/ComponentFactory.hpp"
#include "Alternate.hpp"
#include "Debug.hpp"

#include <config.h>

#include <stdexcept>

#include "gui/Painter.hpp"
#include "gui/TextureManager.hpp"
#include "gui/Image.hpp"
#include "gui/Paragraph.hpp"
#include "gui/XmlReader.hpp"

CheckButton::CheckButton()
    : state(STATE_NORMAL), lowerOnClick(false), checked(false), mclicked(false)
{
}

CheckButton::~CheckButton()
{
}

void
CheckButton::parse(XmlReader& reader)
{
    // parse xml attributes
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else if(strcmp(attribute,"main")==0) {
          mmain=value;
        
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

    // we need 5 child components
    childs.assign(5, Child());
    
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
            } else if(element == "image-checked") {
                if(comp_checked().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for state "
                        "comp_hover defined.\n";
                setChildImage(comp_checked(), reader);
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
CheckButton::setChildImage(Child& child, XmlReader& reader)
{
    std::auto_ptr<Image> image(new Image());
    image->parse(reader);
    resetChild(child, image.release());
}

void
CheckButton::setChildText(Child& child, XmlReader& reader)
{
    std::auto_ptr<Paragraph> paragraph(new Paragraph());
    paragraph->parse(reader);
    resetChild(child, paragraph.release());
}

void CheckButton::uncheck()
{
  checked=false;
  state=STATE_NORMAL;
}

void CheckButton::check()
{
  checked=true;
  state=STATE_CHECKED;
}

void
CheckButton::event(const Event& event)
{
  bool nochange=false;
  bool hovering=event.inside;
  
    State oldstate=state;
    switch(event.type) {
        case Event::MOUSEMOTION:
          break;
        case Event::MOUSEBUTTONDOWN:
            if(!event.inside)
            {
              nochange=true;
              break;
            }
            mclicked=true;
            break;
        case Event::MOUSEBUTTONUP:
            if(event.inside && oldstate == STATE_CLICKED) {
                printf("Clicked on Button '%s'.\n", getName().c_str());
                if(event.mousebutton==SDL_BUTTON_LEFT)
                  checked=!checked;
            }
            if(event.inside && mclicked)
                clicked(this,event.mousebutton);
            mclicked=false;
            break;
        default:
          nochange=true;
            break;
    }
    if(mmain.length())
      checked=false; // these buttons have no state
    
    if(!nochange)
    {
      if(mclicked)
        state=STATE_CLICKED;
      else if(checked)
        state=STATE_CHECKED;
      else if(hovering)
        state=STATE_HOVER;
      else
        state=STATE_NORMAL;
    }

    Component::event(event);
}

void
CheckButton::draw(Painter& painter)
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
        case STATE_CHECKED:
            if(comp_checked().isEnabled()) {
                drawChild(comp_checked(), painter);
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

Component *CheckButton::getCaption()
{
  return comp_caption().getComponent();
}

std::string CheckButton::getMain() const
{
  return mmain;
}


IMPLEMENT_COMPONENT_FACTORY(CheckButton);

/*

CheckButton::CheckButton(Component *pParent,XmlReader &reader):
  Button(pParent,reader),checked(false),alternate(0)
{
  //FIXME: implement attribute reader here somehow!!!
  // find alternate in parent
  
  Component *c=pParent;
  while(alternate==0 && c!=0)
  {
    Alternate *a=dynamic_cast<Alternate*>(c);
    if(a)
      alternate=a;
    else
      c=c->getParent();
  }
  
}
void CheckButton::event(const Event& event)
{
    State oldstate=state;
    switch(event.type) {
        case Event::MOUSEMOTION:
            if(event.inside) {
                if(state == STATE_NORMAL) {
                    state = STATE_HOVER;
                }
            } else {
                if(state == STATE_HOVER) {
                    state = STATE_NORMAL;
                }
            }
            if(checked)
              state=STATE_CLICKED;
            break;
        case Event::MOUSEBUTTONDOWN:
            if(event.inside) {
                state = STATE_CLICKED;
                if(alternate)
                  alternate->toggleDisplay();
            } else {
                state = STATE_NORMAL;
            }
            if(checked)
              state=STATE_CLICKED;
            break;
        case Event::MOUSEBUTTONUP:
            if(event.inside && oldstate == STATE_CLICKED) {
                printf("Clicked on Button '%s'.\n", getName().c_str());
                //clicked(this);
                checked=!checked;
            }
            state = event.inside ? STATE_HOVER : STATE_NORMAL;
            if(checked)
              state=STATE_CLICKED;
            if(event.inside && oldstate == STATE_CLICKED)
                clicked(this);
            break;
        default:
            break;
    }

    Component::event(event);
}

void CheckButton::enable(bool flag)
{
  state=flag?STATE_CLICKED:STATE_NORMAL;
}

IMPLEMENT_COMPONENT_FACTORY(CheckButton)
*/
