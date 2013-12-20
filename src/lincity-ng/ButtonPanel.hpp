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
#ifndef __BUTTON_PANEL_HPP__
#define __BUTTON_PANEL_HPP__

#include "gui/Component.hpp"
#include "lincity/UserOperation.h"

#include <map>

class XmlReader;
class CheckButton;

class ButtonPanel : public Component
{
 public:
  ButtonPanel();

  void parse(XmlReader& reader);

  virtual void draw(Painter &painter);
  void chooseButtonClicked(CheckButton* button,int);
  void menuButtonClicked(CheckButton* button,int);
  virtual bool opaque(const Vector2& pos) const;
  void checkTech( int showInfo );
  void selectQueryTool();
  void toggleBulldozeTool();

 private:
  void attachButtons();
  std::string getAttribute(XmlReader &reader,const std::string &pName) const;
  void doButton(const std::string &button);
  void toggleMenu(std::string pName,bool enable);
  void updateToolInfo();

  bool alreadyAttached;
  void examineButton(const std::string &name, int showInfo );
  void examineMenuButtons();
  void newTechMessage( unsigned short group, int showInfo );
  void updateSelectedCost();
  std::string previousName;
  int lastShownTechGroup;

  std::vector<std::string> mMenuButtons;
  std::vector<std::string> mMenus;
  std::vector<std::string> mButtons;
  std::vector<std::string> activeButtons;

  std::map<std::string, UserOperation> ButtonOperations;
};

ButtonPanel *getButtonPanel();

#endif

/** @file lincity-ng/ButtonPanel.hpp */

