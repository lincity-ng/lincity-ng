#include "ButtonPanel.hpp"
#include "Util.hpp"

#include <exception>

#include "gui/ComponentFactory.hpp"
#include "gui/XmlReader.hpp"
#include "CheckButton.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/Image.hpp"

#include "gui/callback/Callback.hpp"

#include "Debug.hpp"
#include "gui_interface/shared_globals.h"

#include "lincity/lctypes.h"

ButtonPanel *ButtonPanelInstance=0;

int selected_module_type=0; // must be set somehow

ButtonPanel *getButtonPanel()
{
  return ButtonPanelInstance;
}

ButtonPanel::ButtonPanel(Component *pWidget,XmlReader &reader):Component(pWidget)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
//        } else if(strcmp(attribute, "background") == 0) {
//            background.reset(texture_manager->load(value));
        } else if(strcmp(attribute, "width") == 0) {
            if(sscanf(value, "%f", &width) != 1) {
                std::stringstream msg;
                msg << "Parse error when parsing width (" << value << ")";
                throw std::runtime_error(msg.str());
           }
        } else if(strcmp(attribute, "height") == 0) {
            if(sscanf(value, "%f", &height) != 1) {
                std::stringstream msg;
                msg << "Parse error when parsing height (" << value << ")";
                throw std::runtime_error(msg.str());
            }
        } else {
            std::cerr << "Skipping unknown attribute '" << attribute << "'.\n";
        }
    }


  // read buttons/menus,etc.
    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth)
     {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT)
         {
            const std::string& element = (const char*) reader.getName();
            if(element == "menu")
              mMenus.push_back(getName(reader));
            else if(element == "button")
              mButtons.push_back(getName(reader));
            else if(element == "menubutton")
              mMenuButtons.push_back(getName(reader));
            else
            {
                Component* component = parseEmbeddedComponent(this, reader);
                addChild(component);
                if(component->getFlags() & FLAG_RESIZABLE) {
                  component->resize(width, height);
                }

                //width = component->getWidth();
                //height = component->getHeight();
            }
          }
       }      
    
    ButtonPanelInstance = this;
    selected_module_type=selected_module=module=CST_GREEN;
    alreadyAttached=false;
    selected_module_type=CST_NONE;
    
    
    checked_cast<CheckButton>(findComponent(mMenuButtons[0]))->check();
}


std::string ButtonPanel::getName(XmlReader &reader) const
{
    std::string rname;
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();
        if(strcmp(name, "name") == 0)
            return value;
    }
    return rname;
}

void ButtonPanel::attachButtons()
{
  if(alreadyAttached)
    return;
  alreadyAttached=true;
  
  for(size_t i=0;i<mMenuButtons.size();i++)
    {
      Component *c=findComponent(mMenuButtons[i]);
      if(c)
      {
        CheckButton* b = dynamic_cast<CheckButton*>(c);
        if(b)
        {
          std::cout<<"Button1 "<<mMenuButtons[i]<<" found!"<<std::endl;
          b->clicked.connect(makeCallback(*this, &ButtonPanel::menuButtonClicked));
        }
      }
    } 
  for(size_t i=0;i<mButtons.size();i++)
    {
      Component *c=findComponent(mButtons[i]);
      if(c)
      {
        CheckButton* b = dynamic_cast<CheckButton*>(c);
        if(b)
        {
          b->clicked.connect(makeCallback(*this, &ButtonPanel::chooseButtonClicked));
        }
      }
    } 
    
   //FIXME : disable all menus
  
  // now hide menu
  for(size_t i=0;i<mMenuButtons.size();i++)
    {
        // get Component
        Component *c=findComponent(mMenus[i]);
        if(c)
        {
          // try en-/disabling compoent
          // first get parent
          Component *p=c->getParent();
          if(p)
          {
            Childs::iterator i=p->childs.begin();
            for(;i!=p->childs.end();i++)
              if(i->getComponent()==c)
               {
                 i->enable(false);
               }
          }
        }
    }
}

void ButtonPanel::draw(Painter &painter)
{
  attachButtons();
  Component::draw(painter);
}

void ButtonPanel::chooseButtonClicked(CheckButton* button,int b)
{
  CTRACE;
  Image *i=dynamic_cast<Image*>(button->getCaption());
  
  std::string mmain=button->getMain();
  if(i)
  {
    std::string filename=i->getFilename();
    cdebug("filename:"<<filename<<" main:"<<mmain);
    
    // set menu-caption
    if(mmain.length())
    {
      Component *c=findComponent(mmain);
      if(c)
      {
        CheckButton *cb=dynamic_cast<CheckButton*>(c);
        if(cb)
        {
          cb->check(); menuButtonClicked(cb,SDL_BUTTON_LEFT);   // simply simulate button press
          dynamic_cast<Image*>(cb->getCaption())->setFile(filename);
        }
      }
    }
  }
  
  // now hide menu
  for(size_t i=0;i<mMenuButtons.size();i++)
    if(mmain==mMenuButtons[i])
    {
        // get Component
        Component *c=findComponent(mMenus[i]);
        if(c)
        {
          // try en-/disabling compoent
          // first get parent
          Component *p=c->getParent();
          if(p)
          {
            Childs::iterator i=p->childs.begin();
            for(;i!=p->childs.end();i++)
              if(i->getComponent()==c)
               {
                 i->enable(false);
               }
          }
        }
      }
   doButton(button->getName());
}




void ButtonPanel::menuButtonClicked(CheckButton* button,int b)
{
  CTRACE;
  selected_module_type=selected_module=module;
  for(size_t i=0;i<mMenuButtons.size();i++)
    if(button->getName()==mMenuButtons[i])
      {
        // get Component
        Component *c=findComponent(mMenus[i]);
        if(c)
        {
          // try en-/disabling compoent
          // first get parent
          Component *p=c->getParent();
          if(p)
          {
            Childs::iterator i=p->childs.begin();
            for(;i!=p->childs.end();i++)
              if(i->getComponent()==c)
               {
                if(i->enabled)
                  i->enable(false);
                else if(b!=SDL_BUTTON_LEFT)
                  i->enable(true);
               }
          }
        }
      }
     else if(b==SDL_BUTTON_LEFT)
     {
      try
      {
        CheckButton *b=checked_cast<CheckButton>(findComponent(mMenuButtons[i]));
        // disable button
        b->uncheck();
      }
      catch(std::exception &e)
      {
      }
     }
     
   // get selected button and set module
}
int ButtonPanel::getModule() const
{
  return module;
}

 bool ButtonPanel::opaque(const Vector2& pos) const
  {
    Childs::const_iterator i=childs.begin();
    for(;i!=childs.end();i++)
      if(i->component->opaque(pos))
        return true;
    return false;
  }
  
  
void ButtonPanel::doButton(const std::string &button)
{
  if(button=="BPMPointerButton")
    selected_module_type=CST_NONE;
  else if(button=="BPMBullDozeButton")
    selected_module_type=CST_GREEN;
}

IMPLEMENT_COMPONENT_FACTORY(ButtonPanel)
