/*
Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>
Copyright (c) 2024 David Bears <dbear4q@gmail.com>

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

#include <SDL.h>                           // for SDL_BUTTON_RIGHT, SDL_BUTT...
#include <assert.h>                        // for assert
#include <stdio.h>                         // for NULL, sscanf
#include <string.h>                        // for strcmp
#include <functional>                      // for bind, _Bind, function, _2, _1
#include <iostream>                        // for basic_ostream, operator<<
#include <list>                            // for list, _List_iterator
#include <sstream>                         // for basic_stringstream, basic_...
#include <stdexcept>                       // for runtime_error
#include <utility>                         // for pair

#include "Game.hpp"                        // for getGame, Game
#include "GameView.hpp"                    // for getGameView, GameView
#include "MapEdit.hpp"                     // for monument_bul_flag, river_b...
#include "Util.hpp"                        // for getCheckButton
#include "gui/CheckButton.hpp"             // for CheckButton
#include "gui/Child.hpp"                   // for Childs, Child
#include "gui/ComponentFactory.hpp"        // for IMPLEMENT_COMPONENT_FACTORY
#include "gui/ComponentLoader.hpp"         // for parseEmbeddedComponent
#include "gui/Image.hpp"                   // for Image
#include "gui/Signal.hpp"                  // for Signal
#include "gui/XmlReader.hpp"               // for XmlReader
#include "gui_interface/shared_globals.h"  // for modern_windmill_flag
#include "libxml/xmlreader.h"              // for XML_READER_TYPE_ELEMENT
#include "lincity/engglobs.h"              // for tech_level, userOperation
#include "lincity/lin-city.h"              // for GOOD
#include "lincity/lintypes.h"              // for ConstructionGroup
#include "lincity/modules/all_modules.h"   // for MODERN_WINDMILL_TECH, Wind...
#include "tinygettext/gettext.hpp"         // for _

#ifdef DEBUG
#include <assert.h>                        // for assert
#endif

class Painter;
class Vector2;

using namespace std::placeholders;

extern void ok_dial_box(const char *, int, const char *);

ButtonPanel *buttonPanelInstance = NULL;

ButtonPanel *getButtonPanel() {
    return buttonPanelInstance;
}

ButtonPanel::ButtonPanel() {
  assert(!buttonPanelInstance);
  buttonPanelInstance = this;
}

ButtonPanel::~ButtonPanel() {
  for(auto menu : menus) {
    delete menu.second;
  }
  for(auto tool : tools) {
    delete tool.second;
  }
}

void
ButtonPanel::parse(XmlReader& reader) {
  // parse top-level attributes
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

  // parse child elements
  std::list<Menu *> menusTmp;
  int depth = reader.getDepth();
  while(reader.read() && reader.getDepth() > depth) {
    if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
      const std::string& element = (const char*) reader.getName();
      if(element == "menu") {
        Menu *menu = new Menu();
        menusTmp.push_back(menu);

        // parse menu attributes
        XmlReader::AttributeIterator iter(reader);
        while(iter.next()) {
          std::string attribute = (const char *)iter.getName();
          std::string value = (const char *)iter.getValue();
          if(attribute == "button-name") {
            menu->button = reinterpret_cast<CheckButton *>(
              new std::string(value));
          }
          else if(attribute == "drawer-name") {
            menu->drawer = reinterpret_cast<CheckButton *>(
              new std::string(value));
          }
          else if(attribute == "default") {
            menu->activeTool = reinterpret_cast<Tool *>(
              new std::string(value));
          }
          else {
            std::cerr << "Skipping unknown attribute '"
              << attribute << "'.\n";
          }
        }

        // parse menu elements
        int depth = reader.getDepth();
        while(reader.read() && reader.getDepth() > depth) {
          if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            const std::string& element = (const char*) reader.getName();
            if(element == "tool") {
              Tool *tool = new Tool();
              tool->menu = menu;
              menu->tools.push_back(tool);
              tool->operation.action = UserOperation::ACTION_BUILD;

              // parse tool attributes
              XmlReader::AttributeIterator iter(reader);
              while(iter.next()) {
                std::string attribute = (const char *)iter.getName();
                std::string value = (const char *)iter.getValue();
                if(attribute == "name") {
                  tool->button = reinterpret_cast<CheckButton *>(
                    new std::string(value));
                }
                else if(attribute == "action") {
                  if(value == "QUERY")
                    tool->operation.action = UserOperation::ACTION_QUERY;
                  else if(value == "EVACUATE")
                    tool->operation.action = UserOperation::ACTION_EVACUATE;
                  else if(value == "BULLDOZE")
                    tool->operation.action = UserOperation::ACTION_BULLDOZE;
                  else if(value == "FLOOD")
                    tool->operation.action = UserOperation::ACTION_FLOOD;
                  else if(value == "BUILD")
                    tool->operation.action = UserOperation::ACTION_BUILD;
                  else
                    tool->operation.action = UserOperation::ACTION_UNKNOWN;
                }
                else if(attribute == "grpid") {
                  tool->operation.constructionGroup =
                    ConstructionGroup::getConstructionGroup(std::stoi(value));
                }
                else if(attribute == "help") {
                  tool->operation.helpName = value;
                }
                else if(attribute == "upmes") {
                  tool->upMessage = value;
                }
                else {
                  std::cerr << "Skipping unknown attribute '"
                    << attribute << "'.\n";
                }
              }

              // parse tool elements (tools don't have nested elements (yet))
              int depth = reader.getDepth();
              while(reader.read() && reader.getDepth() > depth) {
                const std::string& element = (const char*) reader.getName();
                std::cerr << "Skipping unknown element '"
                  << element << "'.\n";
              }
            }
            else {
              std::cerr << "Skipping unknown element '"
                << element << "'.\n";
            }
          }
        }
      }
      else {
          Component* component = parseEmbeddedComponent(reader);
          addChild(component);
          if(component->getFlags() & FLAG_RESIZABLE)
              component->resize(width, height);
      }
    }
  }

  // resolve component names
  for(Menu *menu : menusTmp) {
    std::string *buttonName = reinterpret_cast<std::string *>(menu->button);
    std::string *drawerName = reinterpret_cast<std::string *>(menu->drawer);
    std::string *activeName = reinterpret_cast<std::string *>(menu->activeTool);

    if(buttonName) {
      menu->button = dynamic_cast<CheckButton *>(findComponent(*buttonName));

      if(menu->button)
        menus[menu->button] = menu;
      else
        std::cerr << "error: could not find menu button '"
          << *buttonName << "'\n";
    }
    else {
      std::cerr << "error: no button-name provided for this menu\n";
    }

    if(menu->drawer) {
      menu->drawer = findComponent(*drawerName);

      if(!menu->drawer)
        std::cerr << "error: could not find menu drawer '"
          << *drawerName << "'\n";
    }
    else {
      std::cerr << "error: no drawer-name provided for this menu\n";
    }

    menu->activeTool = NULL;

    for(Tool *tool : menu->tools) {
      std::string *buttonName = reinterpret_cast<std::string *>(tool->button);

      if(buttonName) {
        tool->button = dynamic_cast<CheckButton *>(findComponent(*buttonName));

        if(tool->button && activeName && *buttonName == *activeName)
          menu->setActiveTool(tool);

        if(tool->button) {
          tools[tool->button] = tool;
        }
        else
          std::cerr << "error: could not find tool button '"
            << *buttonName << "'\n";
      }
      else {
        std::cerr << "error: no name provided for this tool\n";
      }

      delete buttonName;
    }

    if(!menu->activeTool) {
      if(activeName)
        std::cerr << "error: could not find default tool '"
          << *buttonName << "'\n";
      else
        std::cerr << "error: no default tool provided for this menu\n";
    }

    delete buttonName;
    delete drawerName;
    delete activeName;
  }

  connectButtons();
}

void ButtonPanel::connectButtons() {

  for(auto t : tools) {
    t.first->clicked.connect(
      std::bind(&ButtonPanel::toolButtonClicked, this, _1, _2));
    activeTool.registerButton(t.first);
  }

  for(auto m : menus) {
    m.first->clicked.connect(
      std::bind(&ButtonPanel::menuButtonClicked, this, _1, _2));
    activeMenu.registerButton(m.first);

    // close the menu
    mOpenMenu = m.second; // need this to close a menu for the first time
    openMenu(NULL);
  }

  activeTool.selected.connect(
    std::bind(&ButtonPanel::toolSelected, this,
      std::bind(&ButtonPanel::getTool, this, _2)));
  activeMenu.selected.connect(
    std::bind(&ButtonPanel::menuSelected, this,
      std::bind(&ButtonPanel::getMenu, this, _2)));
}

/*
 * enable/disable buttons according to tech.
**/
void ButtonPanel::checkTech(bool showInfo) {
  for(auto t : tools){
    Tool *tool = t.second;
    UserOperation& op = tool->operation;

    if(op.constructionGroup == &windmillConstructionGroup) {
      if(tech_level >= MODERN_WINDMILL_TECH) {
        op.constructionGroup = &windpowerConstructionGroup;

        if(!modern_windmill_flag && showInfo) {
          ok_dial_box("mod_wind_up.mes", GOOD, NULL);
        }
        modern_windmill_flag = 1;
      }
    }
    else if(op.constructionGroup == &windpowerConstructionGroup) {
      if(tech_level < MODERN_WINDMILL_TECH) {
        op.constructionGroup = &windmillConstructionGroup;
      }
    }

    if(op.enoughTech()) {
      if(!tool->button->isEnabled()) {
        tool->button->setTooltip(op.createTooltip(false));
        tool->button->enable();

        if(tool == tool->menu->activeTool) {
          tool->menu->button->enable();
        }

        if(!tool->upShown && showInfo && tool->upMessage.length()) {
          ok_dial_box(tool->upMessage.c_str(), GOOD, 0L);
        }
      }
      tool->upShown = true;
    }
    else {
      if(tool->button->isEnabled()) {
        std::ostringstream os;
        os << op.createTooltip(false).c_str() << " ("
          << _("Techlevel") << " " << op.requiredTech()
          << " " << _("required") << ")";
        tool->button->setTooltip(os.str().c_str());
        tool->button->enable(false);

        if(tool == tool->menu->activeTool) {
          tool->menu->button->enable(false);
        }
      }

      if(!showInfo) {
        // a new game was loaded, so reset the upShown
        // TODO: make this less hacky
        tool->upShown = false;
      }
    }
  }
}

void ButtonPanel::draw(Painter &painter) {
  Component::draw(painter);
}

void ButtonPanel::selectQueryTool() {
  CheckButton* queryButton = getCheckButton(*this, "BPMPointerButton");
  queryButton->tryCheck();
}

void ButtonPanel::toggleBulldozeTool() {
  static CheckButton *previousTool = NULL;

  if(!bulldozeToggled) {
    previousTool = activeTool.getSelection();
    CheckButton *bulldozeButton = getCheckButton(*this, "BPMBullDozeButton");
    bulldozeButton->tryCheck();
    bulldozeToggled = bulldozeButton->isEnabled();
  }
  else if(previousTool) {
    previousTool->tryCheck();
    bulldozeToggled = !previousTool->isEnabled();
  }
}

void ButtonPanel::toolButtonClicked(CheckButton* button, int mouseBtnNum) {
  Tool *tool = tools[button];
  if(mouseBtnNum == SDL_BUTTON_RIGHT) {
    getGame()->showHelpWindow(tool->operation.helpName);
    return;
  }

  openMenu(NULL);
}

void ButtonPanel::menuButtonClicked(CheckButton* button, int mouseBtnNum) {
  Menu *menu = menus[button];
  switch(mouseBtnNum) {
  case SDL_BUTTON_LEFT:
    openMenu(menu == mOpenMenu ? NULL : menu);
    break;
  case SDL_BUTTON_RIGHT:
    button->tryCheck();
    openMenu(NULL);
    break;
  }
}

void ButtonPanel::openMenu(Menu *menu) {
  if(menu == mOpenMenu) return;

  if(menu && mOpenMenu)
    // close the previous menu before opening a new one
    openMenu(NULL);

  Child *pc = (menu ? menu : mOpenMenu)->drawer->getParentChild();
  assert(pc);
  pc->enable(!!menu);
  mOpenMenu = menu;
  setDirty();
}

void ButtonPanel::toolSelected(Tool *tool) {
  Menu *menu = tool->menu;
  menu->setActiveTool(tool);
  activeMenu.select(menu->button);

  userOperation = &tool->operation;

  bulldozeToggled = false;

  // TODO: move this to a function GameView::updateTool(...)
  getGameView()->setCursorSize(tool->operation.cursorSize());
  getGameView()->showToolInfo();

  // TODO: move this to MapEdit.cpp
  monument_bul_flag = river_bul_flag = shanty_bul_flag = 0;
}

void ButtonPanel::menuSelected(Menu *menu) {
  activeTool.select(menu->activeTool->button);
}

void ButtonPanel::Menu::setActiveTool(Tool *tool) {
  if(activeTool == tool)
    return;
  activeTool = tool;
  dynamic_cast<Image *>(button->getCaption())->setFile(
    dynamic_cast<Image *>(tool->button->getCaption())->getFilename());
  button->enable(tool->button->isEnabled());
}

bool ButtonPanel::opaque(const Vector2& pos) const
{
    for(Childs::const_iterator i = childs.begin(); i != childs.end(); ++i) {
        if(i->getComponent()->opaque(pos))
            return true;
    }
    return false;
}

IMPLEMENT_COMPONENT_FACTORY(ButtonPanel)


/** @file lincity-ng/ButtonPanel.cpp */
