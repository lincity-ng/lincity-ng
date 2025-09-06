/* ---------------------------------------------------------------------- *
 * src/lincity-ng/ButtonPanel.hpp
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

#ifndef __BUTTON_PANEL_HPP__
#define __BUTTON_PANEL_HPP__

#include <stddef.h>                      // for NULL
#include <string>                        // for string, basic_string
#include <unordered_map>                 // for unordered_map
#include <vector>                        // for vector

#include "gui/Component.hpp"             // for Component
#include "gui/RadioButtonGroup.hpp"      // for RadioButtonGroup
#include "gui/Signal.hpp"                // for Signal
#include "lincity-ng/UserOperation.hpp"  // for UserOperation

class CheckButton;
class Game;
namespace xmlpp {
class TextReader;
}  // namespace xmlpp

class ButtonPanel : public Component {
public:
  ButtonPanel();
  virtual ~ButtonPanel();

  void parse(xmlpp::TextReader& reader);

  virtual void draw(Painter &painter);
  virtual bool opaque(const Vector2& pos) const;

  void updateTech();
  void selectQueryTool();
  void toggleBulldozeTool();
  void setGame(Game *game);

  Signal<const UserOperation&> selected;
  RadioButtonGroup activeTool;

private:
  Game *game = nullptr;

  struct Tool;

  struct Menu {
    CheckButton *button = NULL;
    Component *drawer = NULL;
    std::vector<Tool *> tools;
    Tool *activeTool = NULL;
    void setActiveTool(Tool *tool);
  };
  struct Tool {
    CheckButton *button = NULL;
    Menu *menu = NULL;
    UserOperation operation;
    std::string helpName;
  };
  std::unordered_map<CheckButton *, Menu *> menus;
  std::unordered_map<CheckButton *, Tool *> tools;

  Menu *mOpenMenu = NULL;
  RadioButtonGroup activeMenu;

  Menu *getMenu(CheckButton *button) { return menus[button]; }
  Tool *getTool(CheckButton *button) { return tools[button]; }

  void connectButtons();
  void openMenu(Menu* menu);
  void toolButtonClicked(CheckButton* button, int);
  void menuButtonClicked(CheckButton* button, int);
  void toolSelected(Tool *tool);
  void menuSelected(Menu *menu);

  bool bulldozeToggled;

  std::string createTooltip(const Tool *tool);
};

ButtonPanel *getButtonPanel();

#endif

/** @file lincity-ng/ButtonPanel.hpp */
