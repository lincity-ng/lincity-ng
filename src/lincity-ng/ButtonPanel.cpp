/* ---------------------------------------------------------------------- *
 * src/lincity-ng/ButtonPanel.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      David Kamphausen <david.kamphausen@web.de>
 * Copyright (C) 2024-2025 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#include "ButtonPanel.hpp"

#include <SDL.h>                          // for SDL_BUTTON_RIGHT, SDL_BUTTO...
#include <assert.h>                       // for assert
#include <fmt/base.h>                     // for println
#include <fmt/format.h>                   // for format
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <stdio.h>                        // for stderr
#include <functional>                     // for _Bind, bind, _2, function, _1
#include <iomanip>                        // for operator<<, setprecision
#include <list>                           // for list, _List_iterator
#include <memory>                         // for dynamic_pointer_cast, uniqu...
#include <sstream>                        // for basic_stringstream, operator<<
#include <stdexcept>                      // for runtime_error
#include <utility>                        // for pair, move

#include "Game.hpp"                       // for Game
#include "UserOperation.hpp"              // for UserOperation
#include "Util.hpp"                       // for getCheckButton
#include "gui/CheckButton.hpp"            // for CheckButton
#include "gui/Child.hpp"                  // for Childs, Child
#include "gui/ComponentFactory.hpp"       // for IMPLEMENT_COMPONENT_FACTORY
#include "gui/ComponentLoader.hpp"        // for parseEmbeddedComponent
#include "gui/Image.hpp"                  // for Image
#include "gui/Signal.hpp"                 // for Signal
#include "lincity/groups.hpp"             // for GROUP_RESIDENCE_HH, GROUP_R...
#include "lincity/lin-city.hpp"           // for MAX_TECH_LEVEL
#include "lincity/lintypes.hpp"           // for ConstructionGroup
#include "lincity/messages.hpp"           // for NotEnoughTechMessage, OutOf...
#include "lincity/modules/windmill.hpp"   // for WindmillConstructionGroup
#include "lincity/modules/windpower.hpp"  // for WindpowerConstructionGroup
#include "lincity/world.hpp"              // for World
#include "util/gettextutil.hpp"           // for _
#include "util/xmlutil.hpp"               // for xmlParse, unexpectedXmlAttr...

using namespace std::placeholders;

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
ButtonPanel::parse(xmlpp::TextReader& reader) {
  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else if(name == "width")
      width = xmlParse<float>(value);
    else if(name == "height")
      height = xmlParse<float>(value);
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  // parse child elements
  std::list<Menu *> menusTmp;
  if(!reader.is_empty_element() && reader.read())
  while(reader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      reader.next();
      continue;
    }
    xmlpp::ustring element = reader.get_name();
    if(element == "menu") {
      Menu *menu = new Menu();
      menusTmp.push_back(menu);

      while(reader.move_to_next_attribute()) {
        xmlpp::ustring name = reader.get_name();
        xmlpp::ustring value = reader.get_value();
        if(name == "button-name")
          menu->button = reinterpret_cast<CheckButton *>(
            new std::string(value));
        else if(name == "drawer-name")
          menu->drawer = reinterpret_cast<CheckButton *>(
            new std::string(value));
        else if(name == "default")
          menu->activeTool = reinterpret_cast<Tool *>(
            new std::string(value));
        else
          unexpectedXmlAttribute(reader);
      }
      reader.move_to_element();

      if(!reader.is_empty_element() && reader.read())
      while(reader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
        if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
          reader.next();
          continue;
        }
        xmlpp::ustring element = reader.get_name();
        if(element == "tool") {
          Tool *tool = new Tool();
          tool->menu = menu;
          menu->tools.push_back(tool);
          tool->operation.action = UserOperation::ACTION_BUILD;

          while(reader.move_to_next_attribute()) {
            xmlpp::ustring name = reader.get_name();
            xmlpp::ustring value = reader.get_value();
            if(name == "name")
              tool->button = reinterpret_cast<CheckButton *>(
                new std::string(value));
            else if(name == "action") {
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
                throw std::runtime_error(fmt::format(
                  "unrecognized action {:?}", value));
            }
            else if(name == "grpid")
              tool->operation.constructionGroup =
                ConstructionGroup::getConstructionGroup(
                  xmlParse<unsigned short>(value));
            else if(name == "help")
              tool->helpName = xmlParse<std::string>(value);
            else
              unexpectedXmlAttribute(reader);
          }
          reader.move_to_element();

          if(!reader.is_empty_element() && reader.read())
          while(reader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
            if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
              reader.next();
              continue;
            }
            // parse tool elements (tools don't have nested elements (yet))
            unexpectedXmlElement(reader);
            reader.next();
          }
        }
        else {
          unexpectedXmlElement(reader);
        }
        reader.next();
      }
    }
    else if(element == "content") {
      std::unique_ptr<Component> component = parseEmbeddedComponent(reader);
      if(component->getFlags() & FLAG_RESIZABLE)
        component->resize(width, height);
      addChild(std::move(component));
    }
    reader.next();
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
        fmt::println(stderr, "error: could not find menu button {:?}",
          *buttonName);
    }
    else {
      fmt::println(stderr, "error: no button-name provided for this menu");
    }

    if(menu->drawer) {
      menu->drawer = findComponent(*drawerName);

      if(!menu->drawer)
        fmt::println(stderr, "error: could not find menu drawer {:?}",
          *drawerName);
    }
    else {
      fmt::println(stderr, "error: no drawer-name provided for this menu");
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
          fmt::println(stderr, "error: could not find tool button {:?}",
            *buttonName);
      }
      else {
        fmt::println(stderr, "error: no name provided for this tool");
      }

      delete buttonName;
    }

    if(!menu->activeTool) {
      if(activeName)
        fmt::println(stderr, "error: could not find default tool {:?}",
          *buttonName);
      else
        fmt::println(stderr, "error: no default tool provided for this menu");
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

void
ButtonPanel::setGame(Game *game) {
  this->game = game;
}

void
ButtonPanel::updateTech() {
  World& world = game->getWorld();
  const int tech_level = world.tech_level;
  for(auto t : tools){
    Tool *tool = t.second;
    UserOperation& op = tool->operation;

    if(op.constructionGroup == &windmillConstructionGroup) {
      if(tech_level >= windpowerConstructionGroup.tech) {
        op.constructionGroup = &windpowerConstructionGroup;
        // TODO: if the windmill tool is in use, need to update the game uop too
      }
    }
    else if(op.constructionGroup == &windpowerConstructionGroup) {
      if(tech_level < windpowerConstructionGroup.tech) {
        op.constructionGroup = &windmillConstructionGroup;
      }
    }

    Message::ptr msg;
    if(op.isAllowed(world, msg)) {
      if(!tool->button->isEnabled() || tool->button->getTooltip() == "") {
        tool->button->setTooltip(createTooltip(tool));
        tool->button->enable();

        if(tool == tool->menu->activeTool) {
          tool->menu->button->enable();
        }
      }
    }
    else if(tool->button->isEnabled() || tool->button->getTooltip() == "") {
      tool->button->enable(false);
      if(tool == tool->menu->activeTool) {
        tool->menu->button->enable(false);
      }

      std::ostringstream os;
      os << createTooltip(tool);
      if(NotEnoughTechMessage::ptr msg_ =
        std::dynamic_pointer_cast<const NotEnoughTechMessage>(msg)
      ) {
        os << " (" << _("requires") << " "
          << std::fixed << std::setprecision(1)
          << (msg_->getRequiredTech() * 100.f / MAX_TECH_LEVEL)
          << " " << _("tech level") << ")";
      }
      else if(OutOfMoneyMessage::ptr msg_ =
        std::dynamic_pointer_cast<const OutOfMoneyMessage>(msg)
      ) {
        os << " (" << _("requires") << " "
          // TODO: format this number properly
          << _("$") << op.constructionGroup->getCosts(world)
          << ")";
      }
      else {
        // TODO: handle NotEnoughStudentsMessage
      }
      tool->button->setTooltip(os.str());
    }
  }
}

std::string
ButtonPanel::createTooltip(const Tool *tool) {
  std::stringstream tooltip;
  const UserOperation& op = tool->operation;
  switch(op.action) {
  case UserOperation::ACTION_QUERY:
    tooltip <<  _("Query Tool") ; break;
  case UserOperation::ACTION_BUILD:
    tooltip << op.constructionGroup->getName();
    switch(op.constructionGroup->group) {
    case GROUP_RESIDENCE_LL:
      tooltip <<  _(": 50 tenants, low birthrate, high deathrate");
      break;
    case GROUP_RESIDENCE_ML:
      tooltip <<  _(": 100 tenants, high birthrate, low deathrate");
      break;
    case GROUP_RESIDENCE_HL:
      tooltip <<  _(": 200 tenants, high birthrate, high deathrate");
      break;
    case GROUP_RESIDENCE_LH:
      tooltip <<  _(": 100 tenants, low birthrate, high deathrate");
      break;
    case GROUP_RESIDENCE_MH:
      tooltip <<  _(": 200 tenants, high birthrate, low deathrate");
      break;
    case GROUP_RESIDENCE_HH:
      tooltip <<  _(": 400 tenants, high birthrate, high deathrate");
      break;
    }
    break;
  case UserOperation::ACTION_BULLDOZE:
      tooltip <<  _("Bulldozer") ; break;
  case UserOperation::ACTION_EVACUATE:
      tooltip << _("Evacuate") ; break;
  case UserOperation::ACTION_FLOOD:
      tooltip <<  _("Water") ; break;
  default:
      tooltip << "unknown useroperation";
  }
  tooltip << " ["<< _("Click right for help.") << "]";
  return tooltip.str();
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
    game->showHelpWindow(tool->helpName);
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

  updateTech();
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

  bulldozeToggled = false;

  selected(tool->operation);
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
