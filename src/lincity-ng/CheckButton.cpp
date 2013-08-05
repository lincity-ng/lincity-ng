/*
Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "CheckButton.hpp"
#include "gui/Event.hpp"
#include "gui/ComponentFactory.hpp"
#include "Debug.hpp"

#include <config.h>

#include <stdexcept>

#include "gui/Painter.hpp"
#include "gui/TextureManager.hpp"
#include "gui/Image.hpp"
#include "gui/Paragraph.hpp"
#include "gui/TooltipManager.hpp"
#include "gui/XmlReader.hpp"

CheckButton::CheckButton()
    : state(STATE_NORMAL), lowerOnClick(true), checked(false),
     mclicked(false), mouseholdTicks(0)
{
    /* // FIXME no utput at crash
    std::cout << "constructing: " << this << ": ";
    std::cout.flush();
    state = STATE_NORMAL;
    std::cout << ".";
    std::cout.flush();
    lowerOnClick = false;
    std::cout << ".";
    std::cout.flush();
    checked = false;
    std::cout << ".";
    std::cout.flush();
    mclicked = false;
    std::cout << ".";
    std::cout.flush();
    mouseholdTicks = 0;
    std::cout << ".";
    std::cout.flush();
    std::cout << " done" << std::endl;
    */
}

CheckButton::~CheckButton()
{
}

void
CheckButton::parse(XmlReader& reader)
{
    // parse xml attributes
    //std::cout << "parsing Checkbutton ...";
    //std::cout.flush();
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
        } else if(strcmp(attribute, "checked") == 0) {
            if(strcmp(value, "true") == 0) {
                check();
            } else if(strcmp(value, "false") == 0) {
                uncheck();
            } else {
                std::cerr << "Unknown value '" << value
                          << "' in check attribute."
                          << " Should be 'true' or 'false'.\n";
            }
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }

    // we need 6 child components
    childs.assign(6, Child());

    // parse contents of the xml-element
    bool parseTooltip = false;
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
            } else if(element == "image-disabled") {
                if(comp_disabled().getComponent() != 0)
                    std::cerr << "Warning: More than 1 component for "
                        "comp_disabled defined.\n";
                setChildImage(comp_disabled(), reader);
            } else if(element == "text-caption") {
                if(comp_caption().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for comp_caption "
                        "defined.\n";
                setChildText(comp_caption(), reader);
            } else if (element == "tooltip") {
                parseTooltip = true;
            } else {
                std::cerr << "Skipping unknown element '" << element << "'.\n";
            }
        } else if(reader.getNodeType() == XML_READER_TYPE_END_ELEMENT) {
             std::string element = (const char*) reader.getName();
             if(element == "tooltip")
                 parseTooltip = false;
        } else if(reader.getNodeType() == XML_READER_TYPE_TEXT) {
            if(!parseTooltip)
                continue;

            const char* p = GUI_TRANSLATE((const char*) reader.getValue());

            // skip trailing spaces
            while(*p != 0 && isspace(static_cast<unsigned char>(*p)))
                ++p;

            bool lastspace = tooltip != "";
            for( ; *p != 0; ++p) {
                if(isspace(static_cast<unsigned char>(*p))) {
                    if(!lastspace) {
                        lastspace = true;
                        tooltip += ' ';
                    }
                } else {
                    lastspace = false;
                    tooltip += *p;
                }
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
    //std::cout << " done" << std::endl;
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

void
CheckButton::uncheck()
{
  checked=false;
  state=STATE_NORMAL;
}

void
CheckButton::check()
{
  checked=true;
  state=STATE_CHECKED;
}

void
CheckButton::enable(bool enabled)
{
    if(!enabled) {
        state = STATE_DISABLED;
    } else if(enabled && state == STATE_DISABLED) {
        state = STATE_NORMAL;
    }
}

bool
CheckButton::isEnabled() const
{
    return state != STATE_DISABLED;
}

void
CheckButton::event(const Event& event)
{
    bool nochange=false;

    State oldstate=state;
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
            if(!event.inside || event.mousebutton == SDL_BUTTON_WHEELUP
                    || event.mousebutton == SDL_BUTTON_WHEELDOWN) {
              nochange=true;
              break;
            }
            pressed(this, event.mousebutton);
            mclicked=true;
            break;
        case Event::MOUSEBUTTONUP:
            if(event.inside && mclicked) {
                if(event.mousebutton == SDL_BUTTON_LEFT &&
                   state != STATE_DISABLED)
                  checked = !checked;
                released(this, event.mousebutton);
                clicked(this, event.mousebutton);
            }
            mclicked=false;
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
             nochange=true;
             break;
        }
        default:
            nochange=true;
            break;
    }
    if(mmain.length()){
      checked=false; // these buttons have no state
    }
    if(!nochange && state != STATE_DISABLED) {
        if(mclicked) {
            state=STATE_CLICKED;
        } else if(checked) {
            state=STATE_CHECKED;
        } else if(event.inside) {
            state=STATE_HOVER;
        } else {
            state=STATE_NORMAL;
        }
    }
    if(oldstate != state){
        setDirty();
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
            } else {
                drawChild(comp_normal(), painter);
                break;
            }
        case STATE_CHECKED:
            if(comp_checked().isEnabled()) {
                drawChild(comp_checked(), painter);
            } else {
                drawChild(comp_normal(), painter);
            }
            break;
        case STATE_DISABLED:
#if 0
            if(comp_disabled().isEnabled()) {
                drawChild(comp_disabled(), painter);
                break;
            }
#endif
            // fallthrough
        case STATE_NORMAL:
            drawChild(comp_normal(), painter);
            break;

        default:
            assert(false);
    }
    if(lowerOnClick)
    {
        if(state == STATE_CLICKED)
        {
            painter.pushTransform();
            painter.translate(Vector2(3,3));
        }
        else if(state == STATE_HOVER)
        {
            painter.pushTransform();
            painter.translate(Vector2(1,1));
        }
    }
    if(comp_caption().isEnabled()) {
        if(state == STATE_DISABLED && comp_disabled().isEnabled())
        {   drawChild(comp_disabled(), painter);}
        else
        {   drawChild(comp_caption(), painter);}
    }
    if(lowerOnClick && (state==STATE_CLICKED || state == STATE_HOVER))
    {   painter.popTransform();}
}

Component *CheckButton::getCaption()
{
  return comp_caption().getComponent();
}

std::string CheckButton::getMain() const
{
  return mmain;
}
void CheckButton::setCaptionText(const std::string &pText)
{
  Child &c=comp_caption();
  Component *cm=c.getComponent();
  if(cm)
  {
    Paragraph *p=dynamic_cast<Paragraph*>(cm);
    if(p)
      p->setText(pText);
  }
}

void CheckButton::setTooltip(const std::string &pText)
{
    tooltip = pText;
}

std::string CheckButton::getCaptionText()
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

IMPLEMENT_COMPONENT_FACTORY(CheckButton)


/** @file lincity-ng/CheckButton.cpp */

