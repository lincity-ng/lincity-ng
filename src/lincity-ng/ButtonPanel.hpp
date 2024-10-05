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
#ifndef __BUTTON_PANEL_HPP__
#define __BUTTON_PANEL_HPP__

#include <map>                      // for map
#include <string>                   // for string, basic_string
#include <unordered_map>                   // for vector

#include "gui/Component.hpp"        // for Component
#include "gui/RadioButtonGroup.hpp"
#include "lincity/UserOperation.h"  // for UserOperation

class CheckButton;
class Painter;
class Vector2;
class XmlReader;

class ButtonPanel : public Component {
public:
  ButtonPanel();
  virtual ~ButtonPanel();

  void parse(XmlReader& reader);

  virtual void draw(Painter &painter);
  virtual bool opaque(const Vector2& pos) const;

  void checkTech(bool showInfo);
  void selectQueryTool();
  void toggleBulldozeTool();

  RadioButtonGroup activeTool;

private:
  struct Tool;
  typedef struct Menu {
    CheckButton *button;
    Component *drawer;
    std::vector<struct Tool *> tools;
    Tool *activeTool;
    void setActiveTool(Tool *tool);
  } Menu;
  typedef struct Tool {
    CheckButton *button;
    Menu *menu;
    UserOperation operation;
    std::string upMessage;
  } Tool;
  std::unordered_map<CheckButton *, Menu *> menus;
  std::unordered_map<CheckButton *, Tool *> tools;

  Menu *mOpenMenu;
  RadioButtonGroup activeMenu;

  Menu *getMenu(CheckButton *button) { return menus[button]; }
  Tool *getTool(CheckButton *button) { return tools[button]; }

  void initComponents();
  void openMenu(Menu* menu);
  void toolButtonClicked(CheckButton* button, int);
  void menuButtonClicked(CheckButton* button, int);
  void toolSelected(Tool *tool);
  void menuSelected(Menu *menu);

  bool bulldozeToggled;

  int lastShownTechGroup;
};

ButtonPanel *getButtonPanel();

#endif

/** @file lincity-ng/ButtonPanel.hpp */
