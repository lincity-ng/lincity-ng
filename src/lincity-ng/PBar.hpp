#ifndef __lc_pbar_h__
#define __lc_pbar_h__

#include <vector>
#include <sstream>
#include <iomanip>
#include "gui_interface/pbar_interface.h"
#include "gui/Component.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/XmlReader.hpp"
#include "gui/Paragraph.hpp"

class LCPBar;
class XmlReader;

extern LCPBar *LCPBarInstance;

class LCPBar:public Component
{
 public:
  LCPBar(Component *widget,XmlReader &reader):Component(widget)
    {
      (void) reader;
      (void) widget;
      
      Component* component = parseEmbeddedComponent(this, reader);
      if(component)
      	addChild(component);

      LCPBarInstance=this;
    }

    void setValue(int num,int value)
    {
    	std::ostringstream os;
      os<<"pbar_text"<<num+1;
    	Component *c=findComponent(os.str());
			if(c)
			{
				Paragraph *p=dynamic_cast<Paragraph*>(c);
				if(p)
				{
	  			std::ostringstream os;
	  			os<<std::fixed;
	  			os<<std::setprecision(1);
	  			if(num==PTECH)
	    			os<<value/10000.0;
	  			else if(num==PMONEY)
	    		{
	      		if(value>1000000)
							os<<value/1000000.0<<"M";
			      else if(value>1000)
							os<<value/1000.0<<"K";
	      		else
							os<<value;
	    		}
	  			else
	    			os<<value;
					
          p->setText(os.str());
         }
       }
      (void) num;
      (void) value;
    }

};

#endif
