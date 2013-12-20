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
#include "lincity/lin-city.h"
#include "lincity/groups.h"
#include "lincity/lintypes.h"
#include "lincity/all_buildings.h"
#include "lincity/modules/all_modules.h"
#include "tinygettext/gettext.hpp"

#include "GameView.hpp"
#include "Game.hpp"
#include "MapEdit.hpp"
#include "ScreenInterface.hpp"
#include "Util.hpp"
#include "HelpWindow.hpp"

extern void ok_dial_box(const char *, int, const char *);

ButtonPanel *ButtonPanelInstance=0;

ButtonPanel *getButtonPanel()
{
    return ButtonPanelInstance;
}

ButtonPanel::ButtonPanel()
{
    userOperation = new UserOperation();
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
            if(element == "menu")
            {
                std::string menuName=getAttribute(reader,"name");
                std::string defName=getAttribute(reader,"default");
                mMenus.push_back(menuName);
                activeButtons.push_back(defName);
            }
            else if(element == "button")
            {   mButtons.push_back(getAttribute(reader,"name"));}
            else if(element == "menubutton")
            {   mMenuButtons.push_back(getAttribute(reader,"name"));}
            else
            {
                Component* component = parseEmbeddedComponent(reader);
                addChild(component);
                if(component->getFlags() & FLAG_RESIZABLE)
                {   component->resize(width, height);}
            }
        }
    }
#ifdef DEBUG
    assert(!ButtonPanelInstance);
#endif
    ButtonPanelInstance = this;
    previousName = "BPMPointerButton";
    alreadyAttached=false;

    userOperation->action = UserOperation::ACTION_QUERY;
    checked_cast<CheckButton>(findComponent(mMenuButtons[0]))->check();
    lastShownTechGroup = 0;
    ButtonOperations.clear();//discard initial default names, attachbuttons() will create proper map
}

std::string ButtonPanel::getAttribute(XmlReader &reader,const std::string &pName) const
{
    std::string rname;
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();
        if(pName == name)
        {   return value;}
    }

    return rname;
}

/*
 * enable/disable buttons according to tech.
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


void ButtonPanel::examineButton(const std::string &name, int showInfo )
{
    UserOperation *usrOp = &(ButtonOperations[name]);
    Component *c = findComponent( name );
    if( !c ) {
        std::cerr << "examineButton# Component "<< name << " not found!?\n";
        return;
    }

    CheckButton* b = dynamic_cast<CheckButton*>(c);
    if( !b ){
        std::cerr << "examineButton# Component "<< name << " is not a Button???\n";
        return;
    }

    if ( usrOp->enoughTech() )
    {
        if( !b->isEnabled() )
        {
            newTechMessage( usrOp->constructionGroup->group, showInfo );
            b->setTooltip( usrOp->createTooltip( false ) );
            b->enable();
        }
    }
    else
    {
        if( b->isEnabled() )
        {
            b->enable( false );
            std::ostringstream os;
            os << usrOp->createTooltip( false ).c_str() << " ("
                << _("Techlevel") << " " << usrOp->requiredTech()
                << " " << _("required") << ")";
            b->setTooltip(os.str().c_str());
        }
    }
}

void ButtonPanel::examineMenuButtons(){
    std::string name;
    Component *c;
    for( size_t number=0; number < mMenuButtons.size(); number++ ){
        name =  mMenuButtons[ number ];
        UserOperation *usrOp = &(ButtonOperations[activeButtons[number]]);
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
        if ( usrOp->enoughTech() )
        {
            if( !b->isEnabled() ){
                b->setTooltip( usrOp->createTooltip( ) );
                b->enable();
            }
        }
        else
        {
            if( b->isEnabled() )
            {
                b->enable( false );
                std::ostringstream os;
                os << usrOp->createTooltip( false ).c_str() << " ("
                    << _("Techlevel") << " " << usrOp->requiredTech()
                    << " " << _("required") << ")";
                b->setTooltip(os.str().c_str());
            }
        }
    }
}

/* Display message for module when it is activated (see above) */
// see oldgui/module_buttons.cpp activate_module
void ButtonPanel::newTechMessage( unsigned short group, int showInfo )
{
    if( showInfo == 0)
    {   return;}

    if( lastShownTechGroup == group )
    {
        //std::cout << "suppressing Tech Msg: " << module << "\n";
        return;
    }
    switch( group ){
        case GROUP_WINDMILL:
            ok_dial_box ("windmillup.mes", GOOD, 0L);
            break;
        case GROUP_COAL_POWER:
            ok_dial_box ("coalpowerup.mes", GOOD, 0L);
            break;
        case GROUP_SOLAR_POWER:
            ok_dial_box ("solarpowerup.mes", GOOD, 0L);
            break;
        case GROUP_COALMINE:
            ok_dial_box ("coalmineup.mes", GOOD, 0L);
            break;
        case GROUP_RAIL:
            ok_dial_box ("railwayup.mes", GOOD, 0L);
            break;
        case GROUP_ROAD:
            ok_dial_box ("roadup.mes", GOOD, 0L);
            break;
        case GROUP_INDUSTRY_L:
            ok_dial_box ("ltindustryup.mes", GOOD, 0L);
            break;
        case GROUP_UNIVERSITY:
            ok_dial_box ("universityup.mes", GOOD, 0L);
            break;
        case GROUP_OREMINE:
            if (GROUP_OREMINE_TECH > 0){
                ok_dial_box ("oremineup.mes", GOOD, 0L);
            }
            break;
        case GROUP_PORT:    /* exports are the same */
            ok_dial_box ("import-exportup.mes", GOOD, 0L);
            break;
        case GROUP_INDUSTRY_H:
            ok_dial_box ("hvindustryup.mes", GOOD, 0L);
            break;
        case GROUP_PARKLAND:
            if (GROUP_PARKLAND_TECH > 0){
                ok_dial_box ("parkup.mes", GOOD, 0L);
            }
            break;
        case GROUP_RECYCLE:
            ok_dial_box ("recycleup.mes", GOOD, 0L);
            break;
        case GROUP_RIVER:
            if (GROUP_WATER_TECH > 0){
                ok_dial_box ("riverup.mes", GOOD, 0L);
            }
            break;
        case GROUP_HEALTH:
            ok_dial_box ("healthup.mes", GOOD, 0L);
            break;
        case GROUP_ROCKET:
            ok_dial_box ("rocketup.mes", GOOD, 0L);
            break;
        case GROUP_SCHOOL:
            if (GROUP_SCHOOL_TECH > 0){
                ok_dial_box ("schoolup.mes", GOOD, 0L);
            }
            break;
        case GROUP_BLACKSMITH:
            if (GROUP_BLACKSMITH_TECH > 0){
                ok_dial_box ("blacksmithup.mes", GOOD, 0L);
            }
        break;
        case GROUP_MILL:
            if (GROUP_MILL_TECH > 0){
                ok_dial_box ("millup.mes", GOOD, 0L);
            }
            break;
        case GROUP_POTTERY:
            if (GROUP_POTTERY_TECH > 0){
                ok_dial_box ("potteryup.mes", GOOD, 0L);
            }
            break;
        case GROUP_FIRESTATION:
            ok_dial_box ("firestationup.mes", GOOD, 0L);
            break;
        case GROUP_CRICKET:
            ok_dial_box ("cricketup.mes", GOOD, 0L);
            break;
        default:
            return;
    }
    lastShownTechGroup = group;
    //remember last Type so we don't show the message twice if we lose just a little tech.
}


void ButtonPanel::attachButtons()
{
    if(alreadyAttached)
    {  return;}
    alreadyAttached = true;
    UserOperation *usrOp = 0;

    for(size_t i=0;i<mButtons.size();i++)
    {
        Component *c=findComponent(mButtons[i]);
        if(c)
        {
            CheckButton* b = dynamic_cast<CheckButton*>(c);
            if(b)
            {
                b->clicked.connect(makeCallback(*this, &ButtonPanel::chooseButtonClicked));
                doButton( mButtons[i] );
                usrOp = &(ButtonOperations[mButtons[i]]);
                if( b->isEnabled() )
                {   b->setTooltip( userOperation->createTooltip( false ) );}
                else
                {
                    std::ostringstream os;
                    os << usrOp->createTooltip( false ).c_str() << " ("
                        << _("Techlevel") << " " << usrOp->requiredTech()
                        << " " << _("required") << ")";
                    b->setTooltip(os.str().c_str());
                }
            }
        }
    }

    for(size_t i=0;i<mMenuButtons.size();i++)
    {
        usrOp = &(ButtonOperations[activeButtons[i]]);
        Component *c=findComponent(mMenuButtons[i]);
        if(c)
        {
            CheckButton* b = dynamic_cast<CheckButton*>(c);
            if(b)
            {
                b->clicked.connect(makeCallback(*this, &ButtonPanel::menuButtonClicked));
                if( b->isEnabled() )
                {   b->setTooltip( usrOp->createTooltip( ) );}
                else
                {
                    std::ostringstream os;
                    os << usrOp->createTooltip( false ).c_str() << " ("
                        << _("Techlevel") << " " << usrOp->requiredTech()
                        << _("required") << ")";
                    b->setTooltip(os.str().c_str());
                }
            }
        }
    }

    checkTech(0);
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
                {
                    if(i->getComponent()==c)
                    {    i->enable(false);}
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

void ButtonPanel::toggleBulldozeTool()
{
    if( userOperation == &ButtonOperations["BPMBullDozeButton"] )
    {
        CheckButton* newButton = getCheckButton( *this, previousName );
        chooseButtonClicked( newButton, SDL_BUTTON_LEFT );
    }
    else
    {
        std::string lastpreviousName = previousName;
        CheckButton* bulldozeButton = getCheckButton( *this, "BPMBullDozeButton");
        chooseButtonClicked( bulldozeButton, SDL_BUTTON_LEFT );
        previousName = lastpreviousName; //revert previous tool
    }
}

void ButtonPanel::chooseButtonClicked(CheckButton* button, int mousebutton )
{
    if( mousebutton == SDL_BUTTON_RIGHT )
    {
        getGame()->showHelpWindow( ButtonOperations[button->getName()].helpName );
        return;
    }

    UserOperation *btnOp = &(ButtonOperations[button->getName()]);
    //CK could return here to simply ignore clicks on inactive buttons
    Image *img = dynamic_cast<Image*>(button->getCaption());
    CheckButton *cb = 0;
    std::string mmain = button->getMain();
    if(img)
    {
        std::string filename = img->getFilename();
        // set menu-caption
        if(mmain.length())
        {
            Component *c=findComponent(mmain);
            //update choice if enough tech
            if(c && btnOp->enoughTech())
            {
                cb=dynamic_cast<CheckButton*>(c);
                if(cb)
                {
                    cb->enable();
                    cb->check();
                    // simply simulate button press
                    menuButtonClicked(cb,SDL_BUTTON_RIGHT);
                    dynamic_cast<Image*>(cb->getCaption())->setFile(filename);
                }
            }
        }
    }

    //now hide the menu
    for(size_t i=0;i<mMenuButtons.size();i++)
    {
        if(mmain==mMenuButtons[i])
        {
            if(btnOp->enoughTech())
            {   activeButtons[i] = button->getName();}
            // get Component
            Component *c=findComponent(mMenus[i]);
            if(c)
            {
                // try en-/disabling compoent
                // first get parent
                Component *p=c->getParent();
                if(p)
                {
                    Childs::iterator itr=p->childs.begin();
                    for(;itr!=p->childs.end();++itr)
                    {
                        if(itr->getComponent()==c)
                        {   itr->enable(false);}
                    }
                }
            }
        }
    }

    if(!btnOp->enoughTech())
    {
#ifdef DEBUG
        ConstructionGroup *constructionGroup = userOperation->constructionGroup;
        std::cout <<"chooseButton not enough tech for: " << (constructionGroup?constructionGroup->name:"unknown") << std::endl;
#endif
        return; //Nothing more to do
    }
    //by now we are sure to have valid choice

    previousName = button->getName();
    userOperation = &(ButtonOperations[previousName]);

    if(cb != 0)
    {   cb->setTooltip( userOperation->createTooltip() );}
    examineMenuButtons();
    getGameView()->setCursorSize(   userOperation->cursorSize() );
    updateToolInfo();
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
            Childs::iterator itr=p->childs.begin();
            for(;itr!=p->childs.end();++itr)
            {
                if(itr->getComponent()==c)
                {
                    itr->enable(enable);
                }
            }
        }
    }
}


void ButtonPanel::menuButtonClicked(CheckButton* button, int b)
{
    for(size_t i=0;i<mMenuButtons.size();i++)
    {
        if(button->getName()==mMenuButtons[i])
        {
            // get Component
            Component* c=findComponent(mMenus[i]);
            //Check if Techlevel is sufficient.
            if(  ButtonOperations[mMenuButtons[i]].enoughTech() && ( b != SDL_BUTTON_RIGHT ) )
            {
                button->check();
                Image *img = dynamic_cast<Image*>(button->getCaption());
                if (img)
                {
                    //select tool from menubutton and set cursor in GameView
                    UserOperation *usrOp = &(ButtonOperations[activeButtons[i]]);
                    if(usrOp->enoughTech())
                    {
                        previousName = activeButtons[i];
                        userOperation = usrOp;
                        getGameView()->setCursorSize(   userOperation->cursorSize() );
                    }
                    menuButtonClicked(button,SDL_BUTTON_RIGHT); //simulate right click
                }
            }

            if(c)
            {
                // try en-/disabling component
                // first get parent
                Component *p=c->getParent();
                if(p)
                {
                    Childs::iterator i=p->childs.begin();
                    for(;i!=p->childs.end();i++)
                    {
                        if(i->getComponent()==c)
                        {
                            if(i->isEnabled())
                            {   i->enable(false);}
                            else if(b!=SDL_BUTTON_RIGHT)
                            {   i->enable(true);}
                        }
                    }
                }
            }
        }
        else if(b==SDL_BUTTON_RIGHT)
        {
            toggleMenu(mMenus[i],false);
            try
            {
                CheckButton *b=checked_cast<CheckButton>(findComponent(mMenuButtons[i]));
                // uncheck button, ignore disabled buttons
                if( b->isEnabled() )
                {   b->uncheck();}
            }
            catch(std::exception &e)
            {   }
        }
        else
        {
            toggleMenu(mMenus[i],false);
        }
    }
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
    UserOperation *buttonOperation = &(ButtonOperations[button]);
    if(button=="BPMPointerButton")
    {
        buttonOperation->constructionGroup = NULL;
        buttonOperation->action = UserOperation::ACTION_QUERY;
        buttonOperation->helpName = "query";
    }
    else if(button=="BPMBullDozeButton")
    {
        buttonOperation->constructionGroup = NULL;
        buttonOperation->action = UserOperation::ACTION_BULLDOZE;
        buttonOperation->helpName = "bulldoze";
    }
    else if(button=="BPMEvacuateButton")
    {
        buttonOperation->constructionGroup = NULL;
        buttonOperation->action = UserOperation::ACTION_EVACUATE;
        buttonOperation->helpName = "evacuate";
    }
    else if(button=="BPMResidence1Button")
    {
        buttonOperation->constructionGroup = &residenceLLConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "residential";
    }
    else if(button=="BPMResidence2Button")
    {
        buttonOperation->constructionGroup = &residenceMLConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "residential";
    }
    else if(button=="BPMResidence3Button")
    {
        buttonOperation->constructionGroup = &residenceHLConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "residential";
    }
    else if(button=="BPMResidence4Button")
    {
        buttonOperation->constructionGroup = &residenceLHConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "residential";
    }
    else if(button=="BPMResidence5Button")
    {
        buttonOperation->constructionGroup = &residenceMHConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "residential";
    }
    else if(button=="BPMResidence6Button")
    {
        buttonOperation->constructionGroup = &residenceHHConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "residential";
    }
    else if(button=="BPMFarmButton")
    {
        buttonOperation->constructionGroup = &organic_farmConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "farm";
    }
    else if(button=="BPMMillButton")
    {
        buttonOperation->constructionGroup = &millConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "mill";
    }
    else if(button=="BPMHealthButton")
    {
        buttonOperation->constructionGroup = &healthCentreConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "health";
    }
    else if(button=="BPMSportsButton")
    {
        buttonOperation->constructionGroup = &cricketConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "cricket";
    }
    else if(button=="BPMFireButton")
    {
        buttonOperation->constructionGroup = &fireStationConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "firestation";
    }
    else if(button=="BPMSchoolButton")
    {
        buttonOperation->constructionGroup = &schoolConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "school";
    }
    else if(button=="BPMUniversityButton")
    {
        buttonOperation->constructionGroup = &universityConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "university";
    }
    else if(button=="BPMTrackButton")
    {
        buttonOperation->constructionGroup = &trackConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "track";
    }
    else if(button=="BPMStreetButton")
    {
        buttonOperation->constructionGroup = &roadConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "road";
    }
    else if(button=="BPMRailButton")
    {
        buttonOperation->constructionGroup = &railConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "rail";
    }
    else if(button=="BPMPortButton")
    {
        buttonOperation->constructionGroup = &portConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "port";
    }
    else if(button=="BPMRocketButton")
    {
        buttonOperation->constructionGroup = &rocketPadConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "rocket";
    }

    else if(button=="BPMPowerLineButton")
    {
        buttonOperation->constructionGroup = &powerlineConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "powerline";
    }
    else if(button=="BPMCoalPSButton")
    {
        buttonOperation->constructionGroup = &coal_powerConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "powerscoal";
    }
    else if(button=="BPMSolarPSButton")
    {
        buttonOperation->constructionGroup = &solarPowerConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "powerssolar";
    }
    else if(button=="BPMSubstationButton")
    {
        buttonOperation->constructionGroup = &substationConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "substation";
    }
    else if(button=="BPMWindmillButton")
    {
        if (tech_level < WIND_POWER_TECH)
        {   buttonOperation->constructionGroup = &windmillConstructionGroup;}
        else
        {   buttonOperation->constructionGroup = &windpowerConstructionGroup;}
        buttonOperation->helpName = "windmill";
        buttonOperation->action = UserOperation::ACTION_BUILD;
    }

    else if(button=="BPMCommuneButton")
    {
        buttonOperation->constructionGroup = &communeConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "commune";
    }
    else if(button=="BPMCoalButton")
    {
        buttonOperation->constructionGroup = &coalmineConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "coalmine";
    }
    else if(button=="BPMOreButton")
    {
        buttonOperation->constructionGroup = &oremineConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "oremine";
    }
    else if(button=="BPMTipButton")
    {
        buttonOperation->constructionGroup = &tipConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "tip";
    }
    else if(button=="BPMRecycleButton")
    {
        buttonOperation->constructionGroup = &recycleConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "recycle";
    }

    else if(button=="BPMLIndustryButton")
    {
        buttonOperation->constructionGroup = &industryLightConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "industryl";
    }
    else if(button=="BPMHIndustryButton")
    {
        buttonOperation->constructionGroup = &industryHeavyConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "industryh";
    }
    else if(button=="BPMMarketButton")
    {
        buttonOperation->constructionGroup = &marketConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "market";
    }
    else if(button=="BPMPotteryButton")
    {
        buttonOperation->constructionGroup = &potteryConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "pottery";
    }
    else if(button=="BPMBlacksmithButton")
    {
        buttonOperation->constructionGroup = &blacksmithConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "blacksmith";
    }

    else if(button=="BPMMonumentButton")
    {
        buttonOperation->constructionGroup = &monumentConstructionGroup;
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "monument";
    }
    else if(button=="BPMParkButton")
    {
        //doublechecked by mapedit anyways, but who knows
        Uint8 *keystate = SDL_GetKeyState(NULL);
        if ( keystate[SDLK_w] )
        {   buttonOperation->constructionGroup = &parkpondConstructionGroup;}
        else
        {   buttonOperation->constructionGroup = &parklandConstructionGroup;}
        buttonOperation->action = UserOperation::ACTION_BUILD;
        buttonOperation->helpName = "park";
    }
    else if(button=="BPMWaterButton")
    {
        buttonOperation->constructionGroup = NULL;
        buttonOperation->action = UserOperation::ACTION_FLOOD;
        buttonOperation->helpName = "river";
    }
    else if(button=="BPMWaterwellButton")
    {
        buttonOperation->constructionGroup = &waterwellConstructionGroup;
        buttonOperation->helpName = "waterwell";
        buttonOperation->action = UserOperation::ACTION_BUILD;
    }
    else
    {   std::cout << "Unknown Button: " << button << std::endl;}

}

void ButtonPanel::updateSelectedCost()
{
    if (userOperation->action == UserOperation::ACTION_BUILD)
    {   selected_module_cost = userOperation->constructionGroup->getCosts();}
    else if (userOperation->action == UserOperation::ACTION_FLOOD)
    {   selected_module_cost = GROUP_WATER_COST;}
    else
    {   selected_module_cost = 0;}
}

IMPLEMENT_COMPONENT_FACTORY(ButtonPanel)


/** @file lincity-ng/ButtonPanel.cpp */

