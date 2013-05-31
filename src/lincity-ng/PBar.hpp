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
#ifndef __lc_pbar_h__
#define __lc_pbar_h__

#include <vector>
#include <sstream>
#include <iomanip>
#include "gui_interface/pbar_interface.h"
#include "gui/Component.hpp"

#include <map>

class LCPBar;
class XmlReader;
class Component;

//extern LCPBar *LCPBarInstance;
extern LCPBar * LCPBarPage1;
extern LCPBar * LCPBarPage2;
extern int pbarGlobalStyle;
#define PBAR_GLOBAL_STYLES 2

class LCPBar : public Component
{
public:
  LCPBar();
  ~LCPBar();

  void parse(XmlReader& reader);
  void setValue(int num, int value, int diff);
};

class BarView : public Component
{
  public:
   BarView();
   ~BarView();

   void parse(XmlReader& reader);
   
   void setValue(float v);
   virtual void draw(Painter &painter);
  private:
   float value;
   bool dir;
   bool bad;
};

#endif

/** @file lincity-ng/PBar.hpp */

