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
#include <config.h>

#include "PBar.hpp"
#include "Util.hpp"
#include "gui/ComponentFactory.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/XmlReader.hpp"
#include "gui/Paragraph.hpp"
#include "gui/Painter.hpp"
#include "../lincity/lintypes.h"
#include "../lincity/lclib.h"

//LCPBar* LCPBarInstance = 0;
LCPBar* LCPBarPage1 = 0;
LCPBar* LCPBarPage2 = 0;
int pbarGlobalStyle = PBAR_GLOBAL_STYLES - 1;
extern const char *commodityNames[];


LCPBar::LCPBar()
{
    //LCPBarInstance = this;
}

LCPBar::~LCPBar()
{
/*    if(LCPBarInstance == this)
        LCPBarInstance = 0;*/
}

void
LCPBar::parse(XmlReader& reader)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(name, value)) {
            continue;
        } else {
            std::cerr << "Unknown attribute '" << name
                      << "' skipped in PBar.\n";
        }
    }

    if(getName() == "PBar")
    {
        //LCPBarInstance = this; //FIXME old code compability hack
        LCPBarPage1 = this;
    }
    else if(getName() == "PBar2nd")
    {   LCPBarPage2 = this;}
     else
     {  std::cerr << "Unknown LCBar component '" << getName() << "' found.\n";}

    Component* component = parseEmbeddedComponent(reader);
    addChild(component);

    width = component->getWidth();
    height = component->getHeight();
}

// copied from old-gui
/* adjusted to match changes in stats.cpp */
#define pbar_adjust_pop(diff) 2 * diff
#define pbar_adjust_tech(diff) diff > 0 ? diff / 4 + 1 : -((-diff+1)/ 2)
#define pbar_adjust_money(diff) diff  > 0 ? diff / 800 + 1 : diff / 400
/*
#define pbar_adjust_food(diff) diff > 0 ? diff / 2000 + 1 : diff / 1000
#define pbar_adjust_jobs(diff) diff > 0 ? diff / 2000 + 1 : diff / 1000
#define pbar_adjust_coal(diff) diff > 0 ? diff / 500 + 1 : diff / 250
#define pbar_adjust_goods(diff) diff > 0 ? diff / 1000 + 1 : diff / 500
#define pbar_adjust_ore(diff) diff > 0 ? diff / 1000 + 1 : diff / 500
#define pbar_adjust_steel(diff) diff > 0 ? diff / 50 + 1 : diff / 25
*/
/*
#define PBAR_DIFF_SCALE 1

#define pbar_adjust_food(diff)  PBAR_DIFF_SCALE * diff
#define pbar_adjust_jobs(diff)  PBAR_DIFF_SCALE * diff
#define pbar_adjust_coal(diff)  PBAR_DIFF_SCALE * diff
#define pbar_adjust_goods(diff) PBAR_DIFF_SCALE * diff
#define pbar_adjust_ore(diff)   PBAR_DIFF_SCALE * diff
#define pbar_adjust_steel(diff) PBAR_DIFF_SCALE * diff
*/
/*
static int Pbarorder[] = {
    Construction::STUFF_FOOD,
    Construction::STUFF_JOBS,
    Construction::STUFF_GOODS,
    Construction::STUFF_COAL,
    Construction::STUFF_ORE,
    Construction::STUFF_STEEL,
    Construction::STUFF_WASTE,
    Construction::STUFF_KWH,
    Construction::STUFF_MWH,
    Construction::STUFF_WATER,
    };
*/
void
LCPBar::setValue(int num, int value, int diff)
{
    if ((num > 8) && (pbarGlobalStyle == 0))
    {   return;}
    if ((pbarGlobalStyle == 1) && (num > 2) && (num < 9))
    {   return;}

    std::ostringstream os;
    int line_number = num+1;
    if ( (pbarGlobalStyle == 1) && (num>8))
    {   line_number -= PBAR_PAGE_SHIFT;}

    os << "pbar_text" << line_number;
    Paragraph* p = getParagraph(*this, os.str());
    os.str("");
    //compname << "pbar_title" << line_number;
    //Paragraph* pt = getParagraph(*this, compname.str());

    if(num==PTECH)
    {
        os<<std::fixed;
        os<<std::setprecision(1);
        os<<value/10000.0;
    }
    else if(num==PMONEY || num==PPOP || num==PPOL)
    {
        char s[12];
        num_to_ansi (s, sizeof(s), value);
        os<<s;
    }
    else if ((num >= PFOOD) && (num <= PHOUSE)) //percentages
    {
         os<<value<<"%";
    }
    else
    {
        os<<"default";
    }
    if (p)
    {   p->setText(os.str());}

    float sv=0;
    switch(num)
    {
      case PPOP:
        sv = pbar_adjust_pop(diff);
        break;
      case PTECH:
        sv = pbar_adjust_tech(diff);
        break;
    case PPOL:
        sv = value<5000?100*diff/(1+value):value<25000?500*diff/value:5000*diff/value;
        break;
    case PMONEY:
        sv = pbar_adjust_money(diff);
        break;
    default:
        sv = diff;
        break;
      };

    sv/=10.0;


     if(sv>1.0)
      sv=1.0;
     if(sv<-1.0)
      sv=-1.0;

    os.str("");
    os<<"pbar_barview"<< line_number;
    Component *c=findComponent(os.str()+"a");
    if(c)
    {
      BarView *bv=dynamic_cast<BarView*>(c);
      if(bv)
      {
        bv->setValue(sv);
      }
    }
    c=findComponent(os.str()+"b");
    if(c)
    {
      BarView *bv=dynamic_cast<BarView*>(c);
      if(bv)
      {
        bv->setValue(sv);
      }
    }

}

///////////////////////////////////////////////////////////////////////////////////////
// BarView
///////////////////////////////////////////////////////////////////////////////////////

BarView::BarView()
{
}

BarView::~BarView()
{
}

void
BarView::parse(XmlReader& reader)
{
    dir=true;
    bad=false;
    // parse attributes...
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(name, value)) {
            continue;
        } else if(strcmp(name, "width") == 0) {
            if(sscanf(value, "%f", &width) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse width attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(name, "height") == 0) {
            if(sscanf(value, "%f", &height) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse height attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(name, "dir") == 0) {
            if(strcmp(value,"1") == 0) {
                dir=true;
            } else {
                dir=false;
            }
        } else if(strcmp(name, "bad") == 0) {
            if(strcmp(value,"1") == 0) {
                bad=true;
            } else {
                bad=false;
            }
        }
        else {
            std::cerr << "Unknown attribute '" << name
                      << "' skipped in BarView.\n";
        }
    }
    if(width <= 0 || height <= 0)
        throw std::runtime_error("Width or Height invalid");
    value=0.7;
}

void BarView::setValue(float v)
{
    if(v>=-1.0 && v<=1.0)
        value=v;
}

void BarView::draw(Painter &painter)
{
    if(((int)(width*value)>0 && dir))
    {
        painter.setFillColor(bad?Color(0xFF,0,0,255):Color(0,0xAA,0,255));
        painter.fillRectangle(Rect2D(0,0,width*value,height));
    }
    else if(((int)(width*value)<0 && !dir))
    {
        painter.setFillColor(bad?Color(0,0xAA,0,255):Color(0xFF,0,0,255));
        painter.fillRectangle(Rect2D(width-1+width*value,0,width-1,height));
    }
}

IMPLEMENT_COMPONENT_FACTORY(LCPBar)
IMPLEMENT_COMPONENT_FACTORY(BarView)

/** @file lincity-ng/PBar.cpp */

