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


char *ButtonPanelButtonNames[]={"BPPointerButton","BPResidenceButton","BullDozeButton"};
char *ButtonPanelMenuNames[]={"BPPointerMenu","BPResidenceMenu"};

char *NormalButtons[]={"BPMPointerButton","BPMBullDozeButton","BPMResidenceButton","BPMResidence2Button"};
#define NBUTTON_COUNT 4

#define BUTTON_COUNT 2

ButtonPanel *getButtonPanel()
{
  return ButtonPanelInstance;
}

ButtonPanel::ButtonPanel(Component *pWidget,XmlReader &reader):Component(pWidget)
{
    Component* component = parseEmbeddedComponent(this, reader);
    if(component)
        addChild(component);

    width = component->getWidth();
    height = component->getHeight();
    
    ButtonPanelInstance = this;
    selected_module_type=selected_module=module=CST_GREEN;
    alreadyAttached=false;
}

void ButtonPanel::attachButtons()
{
  if(alreadyAttached)
    return;
  alreadyAttached=true;
  
  for(int i=0;i<BUTTON_COUNT;i++)
    {
      Component *c=findComponent(ButtonPanelButtonNames[i]);
      if(c)
      {
        CheckButton* b = dynamic_cast<CheckButton*>(c);
        if(b)
        {
          std::cout<<"Button1 "<<ButtonPanelButtonNames[i]<<" found!"<<std::endl;
          b->clicked.connect(makeCallback(*this, &ButtonPanel::menuButtonClicked));
        }
      }
    } 
  for(int i=0;i<NBUTTON_COUNT;i++)
    {
      Component *c=findComponent(NormalButtons[i]);
      if(c)
      {
        CheckButton* b = dynamic_cast<CheckButton*>(c);
        if(b)
        {
          std::cout<<"Button1 "<<ButtonPanelButtonNames[i]<<" found!"<<std::endl;
          b->clicked.connect(makeCallback(*this, &ButtonPanel::chooseButtonClicked));
        }
      }
    } 
    
   //FIXME : disable all menus
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
  for(int i=0;i<BUTTON_COUNT;i++)
    if(mmain==ButtonPanelButtonNames[i])
    {
        // get Component
        Component *c=findComponent(ButtonPanelMenuNames[i]);
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
void ButtonPanel::menuButtonClicked(CheckButton* button,int b)
{
  CTRACE;
  selected_module_type=selected_module=module;
  for(int i=0;i<BUTTON_COUNT;i++)
    if(button->getName()==ButtonPanelButtonNames[i])
      {
        // get Component
        Component *c=findComponent(ButtonPanelMenuNames[i]);
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
        CheckButton *b=checked_cast<CheckButton>(findComponent(ButtonPanelButtonNames[i]));
        // disable button
        b->uncheck();
      }
      catch(std::exception &e)
      {
      }
     }
}
int ButtonPanel::getModule() const
{
  return module;
}


IMPLEMENT_COMPONENT_FACTORY(ButtonPanel)
