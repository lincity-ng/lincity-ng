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

ButtonPanel::ButtonPanel()
{
}

void
ButtonPanel::parse(XmlReader& reader)
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
            {
              std::string menuName=getAttribute(reader,"name");
              std::string defName=getAttribute(reader,"default");
              mMenus.push_back(menuName);
              doButton(defName);
              mMenuSelected[menuName]=selected_module_type;
              
            }
            else if(element == "button")
              mButtons.push_back(getAttribute(reader,"name"));
            else if(element == "menubutton")
              mMenuButtons.push_back(getAttribute(reader,"name"));
            else
            {
                Component* component = parseEmbeddedComponent(reader);
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

std::string ButtonPanel::getAttribute(XmlReader &reader,const std::string &pName) const
{
    std::string rname;
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();
        if(pName==name)
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

void ButtonPanel::chooseButtonClicked(CheckButton* button, int )
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
  
   doButton(button->getName());
  // now hide menu
  for(size_t i=0;i<mMenuButtons.size();i++)
    if(mmain==mMenuButtons[i])
    {
        mMenuSelected[mMenus[i]]=selected_module_type;// set default      
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

void ButtonPanel::toggleMenu(std::string pName,bool enable)
{
        Component *c=findComponent(pName);
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
                i->enable(enable);
          }
        }
}


void ButtonPanel::menuButtonClicked(CheckButton* button,int b)
{
  CTRACE;
  for(size_t i=0;i<mMenuButtons.size();i++)
    if(button->getName()==mMenuButtons[i])
      {
        // get Component
        Component *c=findComponent(mMenus[i]);
        selected_module_type=selected_module=mMenuSelected[mMenus[i]];
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
                if(i->isEnabled())
                  i->enable(false);
                else if(b!=SDL_BUTTON_LEFT)
                  i->enable(true);
               }
          }
        }
      }
     else if(b==SDL_BUTTON_LEFT)
     {
      toggleMenu(mMenus[i],false);
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
     else
     {
      toggleMenu(mMenus[i],false);
     }
     
   // get selected button and set module
   setDirty();
}

int ButtonPanel::getModule() const
{
  return module;
}

bool ButtonPanel::opaque(const Vector2& pos) const
{
    for(Childs::const_iterator i = childs.begin(); i != childs.end(); ++i)
      if(i->getComponent()->opaque(pos))
        return true;
    
    return false;
}
 
void ButtonPanel::doButton(const std::string &button)
{
  if(button=="BPMPointerButton")
    selected_module_type=CST_NONE;
  else if(button=="BPMBullDozeButton")
    selected_module_type=CST_GREEN;
    
    
  else if(button=="BPMResidence1Button")
    selected_module_type=CST_RESIDENCE_LL; 
  else if(button=="BPMResidence2Button")
    selected_module_type=CST_RESIDENCE_ML;
  else if(button=="BPMResidence3Button")
    selected_module_type=CST_RESIDENCE_HL;
  else if(button=="BPMResidence4Button")
    selected_module_type=CST_RESIDENCE_LH;
  else if(button=="BPMResidence5Button")
    selected_module_type=CST_RESIDENCE_MH;
  else if(button=="BPMResidence6Button")
    selected_module_type=CST_RESIDENCE_HH;
  
  
  else if(button=="BPMFarmButton")
    selected_module_type=CST_FARM_O0;
  else if(button=="BPMMillButton")
    selected_module_type=CST_MILL_0;
  
  else if(button=="BPMHealthButton")
    selected_module_type=CST_HEALTH;
  else if(button=="BPMSportsButton")
    selected_module_type=CST_CRICKET_1;
  else if(button=="BPMFireButton")
    selected_module_type=CST_FIRESTATION_1;
  else if(button=="BPMSchoolButton")
    selected_module_type=CST_SCHOOL;
  else if(button=="BPMUniversityButton")
    selected_module_type=CST_UNIVERSITY;
  
  else if(button=="BPMTrackButton")
    selected_module_type=CST_TRACK_LR;
  else if(button=="BPMStreetButton")
    selected_module_type=CST_ROAD_LR;
  else if(button=="BPMRailButton")
    selected_module_type=CST_RAIL_LR;
  else if(button=="BPMPortButton")
    selected_module_type=CST_EX_PORT;
  else if(button=="BPMRocketButton")
    selected_module_type=CST_ROCKET_1;
  
  else if(button=="BPMPowerLineButton")
    selected_module_type=CST_POWERL_H_L;
  else if(button=="BPMCoalPSButton")
    selected_module_type=CST_POWERS_COAL_EMPTY;
  else if(button=="BPMSolarPSButton")
    selected_module_type=CST_POWERS_SOLAR;
  else if(button=="BPMSubstationButton")
    selected_module_type=CST_SUBSTATION_R;
  else if(button=="BPMWindmillButton")
    selected_module_type=CST_WINDMILL_1_R;
  
  else if(button=="BPMCommuneButton")
    selected_module_type=CST_COMMUNE_1;
  else if(button=="BPMCoalButton")
    selected_module_type=CST_COALMINE_EMPTY;
  else if(button=="BPMOreButton")
    selected_module_type=CST_OREMINE_1;
  else if(button=="BPMTipButton")
    selected_module_type=CST_TIP_0;
  else if(button=="BPMRecycleButton")
    selected_module_type=CST_RECYCLE;
  
  else if(button=="BPMLIndustryButton")
    selected_module_type=CST_INDUSTRY_L_C;
  else if(button=="BPMHIndustryButton")
    selected_module_type=CST_INDUSTRY_H_C;
  else if(button=="BPMMarketButton")
    selected_module_type=CST_MARKET_EMPTY;
  else if(button=="BPMPotteryButton")
    selected_module_type=CST_POTTERY_0;
  else if(button=="BPMBlacksmithButton")
    selected_module_type=CST_BLACKSMITH_0;

  else if(button=="BPMMonumentButton")
    selected_module_type=CST_MONUMENT_0;
  else if(button=="BPMParkButton")
    selected_module_type=CST_PARKLAND_PLANE;
  else if(button=="BPMWaterButton")
    selected_module_type=CST_WATER;
    
    
    
}

IMPLEMENT_COMPONENT_FACTORY(ButtonPanel)
