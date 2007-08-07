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

LCPBar* LCPBarInstance = 0;

LCPBar::LCPBar()
{
    LCPBarInstance = this;
}

LCPBar::~LCPBar()
{
    if(LCPBarInstance == this)
        LCPBarInstance = 0;
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
    
    Component* component = parseEmbeddedComponent(reader);
    addChild(component);

    width = component->getWidth();
    height = component->getHeight();
}

// copied from old-gui
#define pbar_adjust_pop(diff) 2 * diff
#define pbar_adjust_tech(diff) diff > 0 ? diff / 4 + 1 : -((-diff+1)/ 2)
#define pbar_adjust_food(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_jobs(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_coal(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_goods(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_ore(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_steel(diff) diff > 0 ? diff / 2 + 1 : diff
#define pbar_adjust_money(diff) diff  > 0 ? diff / 800 + 1 : diff / 400
 
void
LCPBar::setValue(int num, int value, int diff)
{
    std::ostringstream compname;
    compname << "pbar_text" << (num+1);
    Paragraph* p = getParagraph(*this, compname.str());

    std::ostringstream os;
    os<<std::fixed;
    os<<std::setprecision(1);
    if(num==PTECH)
    {
        os<<value/10000.0;
     }
    else if(num==PMONEY)
    {
        if(abs(value)>=1000000000)
            os<<value/1000000<<"M";
        else if(abs(value)>1000000)
            os<<value/1000000.0<<"M";
        else if(abs(value)>1000)
            os<<value/1000.0<<"K";
        else
            os<<value;
    }
    else
        os<<value;
    if( diff != 0 ){
        p->setText(os.str());
    }
    os.str("");
    os<<"pbar_barview"<<(num+1);

    float sv=0;
    switch(num)
    {
      case PPOP:
        sv=pbar_adjust_pop(diff);
        break;
      case PTECH:
        sv=pbar_adjust_tech(diff);
        break;
      case PFOOD:
        sv=pbar_adjust_food(diff);break;
      case PJOBS:
        sv=pbar_adjust_jobs(diff);break;
      case PCOAL:
        sv=pbar_adjust_coal(diff);break;
      case PGOODS:
        sv=pbar_adjust_goods(diff);break;
      case PORE:
        sv=pbar_adjust_ore(diff);break;
      case PSTEEL:
        sv=pbar_adjust_steel(diff);break;
      case PMONEY:
        sv=pbar_adjust_money(diff);break;
      };
      
    sv/=10.0;
    
    
     if(sv>1.0)
      sv=1.0;
     if(sv<-1.0)
      sv=-1.0; 
        
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
        } else {
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
    painter.setFillColor(Color(0,0xAA,0,255));
  
    if((int)(width*value)>0 && dir) {
        painter.fillRectangle(Rect2D(0,0,width*value,height));
    } else if((int)(width*value)<0 && !dir) {
        painter.setFillColor(Color(0xFF,0,0,255));
        painter.fillRectangle(Rect2D(width-1+width*value,0,width-1,height));
    }
}

IMPLEMENT_COMPONENT_FACTORY(LCPBar);
IMPLEMENT_COMPONENT_FACTORY(BarView);
