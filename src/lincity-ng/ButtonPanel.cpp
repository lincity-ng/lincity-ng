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
#include "ButtonPanel.hpp"
#include "Util.hpp"

#include <exception>
#include <sstream>

#include "gui/ComponentFactory.hpp"
#include "gui/XmlReader.hpp"
#include "CheckButton.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/Image.hpp"

#include "gui/callback/Callback.hpp"

#include "Debug.hpp"
#include "gui_interface/shared_globals.h"

#include "lincity/lctypes.h"
#include "tinygettext/gettext.hpp"

#include "GameView.hpp"
#include "MapEdit.hpp"
#include "ScreenInterface.hpp"
#include "Util.hpp"

ButtonPanel *ButtonPanelInstance=0;

int selected_module_type = CST_NONE;

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
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            const std::string& element = (const char*) reader.getName();
            if(element == "menu") {
                std::string menuName=getAttribute(reader,"name");
                std::string defName=getAttribute(reader,"default");
                mMenus.push_back(menuName);
                doButton(defName);
                mMenuSelected[menuName]=selected_module_type;
            } else if(element == "button") {
                mButtons.push_back(getAttribute(reader,"name"));
            } else if(element == "menubutton") {
                mMenuButtons.push_back(getAttribute(reader,"name"));
            } else {
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
    previousTool = selected_module_type=selected_module=module=CST_GREEN;
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
        if(pName == name)
            return value;
    }
    
    return rname;
}

//show required tech in display-format
float ButtonPanel::requiredTech(int moduleType) {
    if( moduleType == CST_NONE ){
        return 0;
    }
    int group = get_group_of_type( moduleType );
    float tl;
    //High Tech Residences are special
    if( group == GROUP_RESIDENCE_LH || group == GROUP_RESIDENCE_MH 
            || group == GROUP_RESIDENCE_HH ){
        tl = MAX_TECH_LEVEL / 5;
    } else {   
        tl = main_groups[ group ].tech * MAX_TECH_LEVEL/1000;
    }
    
    return tl * 100 / MAX_TECH_LEVEL;
}

//test if there is enough tech to build moduleType.
bool ButtonPanel::enoughTech( int moduleType ){
    if( moduleType == CST_NONE ){
        return true;
    }
    int group = get_group_of_type( moduleType );
    //High Tech Residences are special
    if( group == GROUP_RESIDENCE_LH || group == GROUP_RESIDENCE_MH 
            || group == GROUP_RESIDENCE_HH ){
        if( ( ( tech_level * 10 ) / MAX_TECH_LEVEL) > 2 ){
            return true;
        } else {
            return false;
        }
    }
    if ( tech_level >= main_groups[ group ].tech * MAX_TECH_LEVEL/1000 ){
        return true;
    }
    return false;
}

/*
 * enable/disabel buttons accordig to tech.
 *  lincity/ldsvguts.cpp
 *  oldgui/module_buttons.cpp
 */
void ButtonPanel::checkTech( int showInfo ){
    std::string name;
    //Buttons in SubMenus
    for( size_t i=0; i<mButtons.size(); i++ ){
        examineButton( mButtons[i], showInfo );
    }
    //Buttons in MainMenu
    examineMenuButtons();

    if (tech_level > MODERN_WINDMILL_TECH && modern_windmill_flag == 0 && showInfo != 0){
	    ok_dial_box ("mod_wind_up.mes", GOOD, 0L);
	    modern_windmill_flag = 1;
    }
}

std::string ButtonPanel::createTooltip( int module ){
    switch( module ){
        case CST_NONE: return _( "Query Tool" );
        case CST_GREEN: return _( "Bulldozer" );
                        
        case CST_RESIDENCE_LL: return _( "Residential: 50 denizens, low birthrate, high deathrate" );
        case CST_RESIDENCE_ML: return _( "Residential: 100 denizens, high birthrate, low deathrate" );
        case CST_RESIDENCE_HL: return _( "Residential: 200 denizens, high birthrate, high deathrate" );
        case CST_RESIDENCE_LH: return _( "Residential: 100 denizens, low birthrate, high deathrate" );
        case CST_RESIDENCE_MH: return _( "Residential: 200 denizens, high birthrate, low deathrate" );
        case CST_RESIDENCE_HH: return _( "Residential: 400 denizens, high birthrate, high deathrate" );
  
        default:{
            int group = main_types[ module ].group;
            std::string buildingName = main_groups[ group ].name;
            return dictionaryManager->get_dictionary().translate( buildingName ); 
        }
    }
}

void ButtonPanel::examineButton( std::string name, int showInfo ){
    int tmp = selected_module_type;
    Component *c=findComponent( name );
    if( !c ) {
        std::cerr << "examineButton# Component "<< name << " not found!?\n";
        return;
    }
    CheckButton* b = dynamic_cast<CheckButton*>(c);
    if( !b ){
        std::cerr << "examineButton# Component "<< name << " is not a Button???\n";
        return;
    }
    doButton( name );
	if ( enoughTech( selected_module_type ) ){
        if( !b->isEnabled() ){
            newTechMessage( selected_module_type, showInfo );
            std::stringstream tooltip;
            b->setTooltip( createTooltip( selected_module_type ) );
            b->enable();
        }
    } else {
        if( b->isEnabled() ){
            b->enable( false );
            char tooltip[2048];
            snprintf(tooltip, sizeof(tooltip), "%s (Techlevel %.1f required.)",
                    createTooltip(selected_module_type).c_str(),
                    requiredTech(selected_module_type));
            b->setTooltip(tooltip);
        }
    }
    selected_module_type = tmp;
}

void ButtonPanel::examineMenuButtons(){
    std::string name; 
    Component *c;
    for( size_t number=0; number < mMenuButtons.size(); number++ ){
        name = mMenuButtons[ number ]; 
        c=findComponent( name );
            
        if( !c ) {
            std::cerr << "examineMenuButton# Component" << name << "not found!?\n";
            return;
        }
        CheckButton* b = dynamic_cast<CheckButton*>(c);
        if( !b ){
            std::cerr << "examineMenuButton# Component "<< name << " is not a Button???\n";
            return;
        }
        int type = mMenuSelected[mMenus[number]];
        if ( enoughTech( type ) ){
            if( !b->isEnabled() ){
                b->setTooltip( createTooltip( type ) );
                b->enable();
            }
        } else {
            if( b->isEnabled() ){
                char tooltip[2048];
                snprintf(tooltip, sizeof(tooltip), "%s (Techlevel %.1f required.)"
                        , createTooltip(type).c_str(),
                        requiredTech(type));
                b->setTooltip( tooltip );
                b->enable( false );
            }
        }
    }
}

/* Display message for module when it is activated (see above) */
// from oldgui/module_buttons.cpp activate_module
void ButtonPanel::newTechMessage( int moduleType, int showInfo )
{
    if( showInfo == 0) return;

    int module = get_group_of_type( moduleType );

    if (module == GROUP_WINDMILL)
	ok_dial_box ("windmillup.mes", GOOD, 0L);
    else if (module == GROUP_COAL_POWER)
	ok_dial_box ("coalpowerup.mes", GOOD, 0L);
    /*    else if (module == (GROUP_SOLAR_POWER - 1)) */
    else if (module == GROUP_SOLAR_POWER)
	ok_dial_box ("solarpowerup.mes", GOOD, 0L);
    else if (module == GROUP_COALMINE)
	ok_dial_box ("coalmineup.mes", GOOD, 0L);
    else if (module == GROUP_RAIL)
	ok_dial_box ("railwayup.mes", GOOD, 0L);
    else if (module == GROUP_ROAD)
	ok_dial_box ("roadup.mes", GOOD, 0L);
    else if (module == GROUP_INDUSTRY_L)
	ok_dial_box ("ltindustryup.mes", GOOD, 0L);
    else if (module == GROUP_UNIVERSITY)
	ok_dial_box ("universityup.mes", GOOD, 0L);
    else if (module == GROUP_OREMINE)
    {
	if (GROUP_OREMINE_TECH > 0)
	    ok_dial_box ("oremineup.mes", GOOD, 0L);
    }
    else if (module == GROUP_PORT)	/* exports are the same */
	ok_dial_box ("import-exportup.mes", GOOD, 0L);
    else if (module == GROUP_INDUSTRY_H)
	ok_dial_box ("hvindustryup.mes", GOOD, 0L);
    else if (module == GROUP_PARKLAND)
    {
	if (GROUP_PARKLAND_TECH > 0)
	    ok_dial_box ("parkup.mes", GOOD, 0L);
    }
    else if (module == GROUP_RECYCLE)
	ok_dial_box ("recycleup.mes", GOOD, 0L);
    else if (module == GROUP_RIVER)
    {
	if (GROUP_WATER_TECH > 0)
	    ok_dial_box ("riverup.mes", GOOD, 0L);
    }
    else if (module == GROUP_HEALTH)
	ok_dial_box ("healthup.mes", GOOD, 0L);
    else if (module == GROUP_ROCKET)
	ok_dial_box ("rocketup.mes", GOOD, 0L);
    else if (module == GROUP_SCHOOL)
    {
	if (GROUP_SCHOOL_TECH > 0)
	    ok_dial_box ("schoolup.mes", GOOD, 0L);
    }
    else if (module == GROUP_BLACKSMITH)
    {
	if (GROUP_BLACKSMITH_TECH > 0)
	    ok_dial_box ("blacksmithup.mes", GOOD, 0L);
    }
    else if (module == GROUP_MILL)
    {
	if (GROUP_MILL_TECH > 0)
	    ok_dial_box ("millup.mes", GOOD, 0L);
    }
    else if (module == GROUP_POTTERY)
    {
	if (GROUP_POTTERY_TECH > 0)
	    ok_dial_box ("potteryup.mes", GOOD, 0L);
    }
    else if (module == GROUP_FIRESTATION)
	ok_dial_box ("firestationup.mes", GOOD, 0L);
    else if (module == GROUP_CRICKET)
	ok_dial_box ("cricketup.mes", GOOD, 0L);
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
          b->clicked.connect(makeCallback(*this, &ButtonPanel::menuButtonClicked));
          std::stringstream tooltip;
          b->setTooltip( createTooltip( mMenuSelected[ mMenus[ i ] ] ) );
        }
      }
    } 
  int tmp = selected_module_type; 
  for(size_t i=0;i<mButtons.size();i++)
    {
      Component *c=findComponent(mButtons[i]);
      if(c)
      {
        CheckButton* b = dynamic_cast<CheckButton*>(c);
        if(b)
        {
          b->clicked.connect(makeCallback(*this, &ButtonPanel::chooseButtonClicked));
          std::stringstream tooltip;
          doButton( mButtons[i] );
          b->setTooltip( createTooltip( selected_module_type ) );
        }
      }
    } 
  selected_module_type = tmp; 
  checkTech(0);    
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

/*
 * Show Information about selected Tool
 */
void ButtonPanel::updateToolInfo()
{       
    //Tool was changed, so reset the bulldozeflags to enable 
    //the warnings again.
    monument_bul_flag = 0;
    river_bul_flag = 0;
    shanty_bul_flag = 0;
    updateSelectedCost();
    getGameView()->showToolInfo(); //display new values
}

void ButtonPanel::draw(Painter &painter)
{
  attachButtons();
  Component::draw(painter);
}

void ButtonPanel::selectQueryTool(){
    CheckButton* queryButton = getCheckButton( *this, "BPMPointerButton");
    chooseButtonClicked( queryButton, SDL_BUTTON_LEFT ); 
}

void ButtonPanel::toggleBulldozeTool(){
    if( selected_module_type == CST_GREEN ){
        switchToTool( previousTool );
    }
    else{
        previousTool = selected_module_type;
        CheckButton* bulldozeButton = getCheckButton( *this, "BPMBullDozeButton");
        chooseButtonClicked( bulldozeButton, SDL_BUTTON_LEFT ); 
    }
}

void ButtonPanel::switchToTool( int newModuleType ){
    std::string newName;
    switch( newModuleType ){
        case CST_NONE: newName = "BPMPointerButton"; break;
        case CST_GREEN :newName = "BPMBullDozeButton"; break;
                        
        case CST_RESIDENCE_LL: newName = "BPMResidence1Button"; break;
        case CST_RESIDENCE_ML: newName = "BPMResidence2Button"; break;
        case CST_RESIDENCE_HL: newName = "BPMResidence3Button"; break;
        case CST_RESIDENCE_LH: newName = "BPMResidence4Button"; break;
        case CST_RESIDENCE_MH: newName = "BPMResidence5Button"; break;
        case CST_RESIDENCE_HH: newName = "BPMResidence6Button"; break;
  
        case CST_FARM_O0: newName ="BPMFarmButton"; break;
        case CST_MILL_0: newName ="BPMMillButton"; break;
  
        case CST_HEALTH: newName="BPMHealthButton"; break;
        case CST_CRICKET_1: newName="BPMSportsButton"; break;
        case CST_FIRESTATION_1: newName="BPMFireButton"; break;
        case CST_SCHOOL: newName="BPMSchoolButton"; break;
        case CST_UNIVERSITY: newName="BPMUniversityButton"; break;
  
        case CST_TRACK_LR: newName="BPMTrackButton"; break;
        case CST_ROAD_LR: newName="BPMStreetButton"; break;
        case CST_RAIL_LR: newName="BPMRailButton"; break;
        case CST_EX_PORT: newName="BPMPortButton"; break;
        case CST_ROCKET_1: newName ="BPMRocketButton"; break;
   
        case CST_POWERL_H_L: newName ="BPMPowerLineButton"; break;
  
        case CST_POWERS_COAL_EMPTY: newName ="BPMCoalPSButton"; break;
        case CST_POWERS_SOLAR: newName ="BPMSolarPSButton"; break;
        case CST_SUBSTATION_R: newName ="BPMSubstationButton"; break;
        case CST_WINDMILL_1_R: newName ="BPMWindmillButton"; break;
  
        case CST_COMMUNE_1: newName ="BPMCommuneButton"; break;
        case CST_COALMINE_EMPTY: newName ="BPMCoalButton"; break;
        case CST_OREMINE_1: newName ="BPMOreButton"; break;
        case CST_TIP_0: newName ="BPMTipButton"; break;
        case CST_RECYCLE: newName ="BPMRecycleButton"; break;
  
        case CST_INDUSTRY_L_C: newName ="BPMLIndustryButton"; break;
        case CST_INDUSTRY_H_C: newName ="BPMHIndustryButton"; break;
        case CST_MARKET_EMPTY: newName ="BPMMarketButton"; break;
        case CST_POTTERY_0: newName ="BPMPotteryButton"; break;
        case CST_BLACKSMITH_0: newName ="BPMBlacksmithButton"; break;

        case CST_MONUMENT_0: newName ="BPMMonumentButton"; break;
        case CST_PARKLAND_PLANE: newName ="BPMParkButton"; break;
        case CST_WATER: newName ="BPMWaterButton"; break;
        default:
            std::cerr << "ButtonPanel::switchToTool# unknown Type " << newModuleType << "\n";
            newName ="BPMPointerButton";
    }
    CheckButton* newButton = getCheckButton( *this, newName );
    chooseButtonClicked( newButton, SDL_BUTTON_LEFT ); 
}
    
void ButtonPanel::chooseButtonClicked(CheckButton* button, int )
{
    Image *i=dynamic_cast<Image*>(button->getCaption());
    CheckButton *cb = 0;
    std::string mmain=button->getMain();
    if(i)
    {
        std::string filename=i->getFilename();
        
        // set menu-caption
        if(mmain.length())
        {
            Component *c=findComponent(mmain);
            if(c)
            {
                cb=dynamic_cast<CheckButton*>(c);
                if(cb)
                {
                    cb->enable();
                    cb->check();
                    // simply simulate button press
                    menuButtonClicked(cb,SDL_BUTTON_LEFT);
                    dynamic_cast<Image*>(cb->getCaption())->setFile(filename);
                }
            }
        }
    }
    
    //check tech:
    int prevTech = selected_module_type;
    doButton(button->getName());
    
    // now hide menu
    for(size_t i=0;i<mMenuButtons.size();i++) {
        if(mmain==mMenuButtons[i])
        {
            mMenuSelected[mMenus[i]]=selected_module_type;// set default      
            // get Component
            Component *c=findComponent(mMenus[i]);
            if(c) {
                // try en-/disabling compoent
                // first get parent
                Component *p=c->getParent();
                if(p) {
                    Childs::iterator i=p->childs.begin();
                    for(;i!=p->childs.end();i++) {
                        if(i->getComponent()==c) {
                            i->enable(false);
                        }
                    }
                }
            }
        }
    }
    
    if(!enoughTech( selected_module_type)) {
        std::cout <<"chooseButton not enough tech for " << selected_module_type << "\n";
        selected_module_type = prevTech;
        updateSelectedCost();
    }
    if(cb != 0)
        cb->setTooltip( createTooltip( selected_module_type ) );
    examineMenuButtons();

    //Tell GameView to use the right Cursor
    if( selected_module_type == CST_NONE ) {
        getGameView()->setCursorSize( 0 );
    } else {
        int selected_module_group = get_group_of_type(selected_module_type);
        int size = main_groups[selected_module_group].size;
        getGameView()->setCursorSize( size );
    }
    updateToolInfo();        
}

void ButtonPanel::toggleMenu(std::string pName,bool enable)
{
    Component *c=findComponent(pName);
    if(c) {
        // try en-/disabling compoent
        // first get parent
        Component *p=c->getParent();
        if(p) {
            Childs::iterator i=p->childs.begin();
            for(;i!=p->childs.end();i++)
                if(i->getComponent()==c)
                    i->enable(enable);
        }
    }
}


void ButtonPanel::menuButtonClicked(CheckButton* button,int b)
{
    for(size_t i=0;i<mMenuButtons.size();i++) {
        if(button->getName()==mMenuButtons[i]) {
            // get Component
            Component* c=findComponent(mMenus[i]);
            //Check if Techlevel is sufficient.
            if( enoughTech( mMenuSelected[mMenus[i]] ) && ( b == SDL_BUTTON_LEFT ) ){
                selected_module_type=selected_module=mMenuSelected[mMenus[i]];
                updateSelectedCost();
                button->check();
            }
            if(c) {
                // try en-/disabling compoent
                // first get parent
                Component *p=c->getParent();
                if(p) {
                    Childs::iterator i=p->childs.begin();
                    for(;i!=p->childs.end();i++) {
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
        } else if(b==SDL_BUTTON_LEFT)  {
            toggleMenu(mMenus[i],false);
            try
            {
                CheckButton *b=checked_cast<CheckButton>(findComponent(mMenuButtons[i]));
                // disable button
                b->uncheck();
            } catch(std::exception &e) {
            }
        } else {
            toggleMenu(mMenus[i],false);
        }
    }
     
    // get selected button and set module
    
    //Tell GameView to use the right Cursor
    if( selected_module_type == CST_NONE ) {
        getGameView()->setCursorSize( 0 );
    } else {
        int selected_module_group = get_group_of_type(selected_module_type);
        int size = main_groups[selected_module_group].size;
        getGameView()->setCursorSize( size );
    }
    updateToolInfo();
    setDirty();
}

int ButtonPanel::getModule() const
{
    return module;
}

bool ButtonPanel::opaque(const Vector2& pos) const
{
    for(Childs::const_iterator i = childs.begin(); i != childs.end(); ++i) {
        if(i->getComponent()->opaque(pos))
            return true;
    }
    
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

void ButtonPanel::updateSelectedCost()
{
    selected_module_cost = get_type_cost (selected_module_type); 
}

IMPLEMENT_COMPONENT_FACTORY(ButtonPanel);

