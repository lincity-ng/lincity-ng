#ifndef __lc_pbar_h__
#define __lc_pbar_h__

#include <vector>
#include <sstream>
#include <iomanip>
#include "gui_interface/pbar_interface.h"
#include "gui/Component.hpp"

#define LCPBarDisplayCount 9
extern char *LCPBarDisplays[];

class LCPBar;
class XmlReader;
extern LCPBar *LCPBarInstance;
extern int LCPBarIDs[];

class LCPBar:public Component//AGTable
{
  //  std::vector<AGText*> mValues;

 public:
  LCPBar(Component *widget,XmlReader &reader):Component(widget)
    {
      (void) reader;
      (void) widget;
      /*
      int i;
      for(i=0;i<2;i++)
	{
	  addFixedColumn();
	}
      for(i=0;i<LCPBarDisplayCount;i++)
	{
	  addFixedRow();
	}

      for(i=0;i<LCPBarDisplayCount;i++)
	{
	  AGText *v;
	  AGFont f("Arial.ttf",13);
	  f.setColor(AGColor(0,0,0));
	  AGTable::addChild(0,i,new AGText(0,AGPoint(0,0),LCPBarDisplays[i],f));
	  AGTable::addChild(1,i,v=new AGText(0,AGPoint(0,0),"0",f));
	  mValues.push_back(v);
	}
	arrange();*/
      LCPBarInstance=this;
    }

    void setValue(int num,int value)
    {
      (void) num;
      (void) value;
      /*
      if(num<mValues.size())
	{
	  int i=0;
	  for(;i<LCPBarDisplayCount;i++)
	    if(LCPBarIDs[i]==num)
	      break;

	  std::ostringstream os;
	  os<<std::fixed;
	  os<<std::setprecision(1);
	  if(i==PTECH)
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
	  mValues[i]->setText(os.str());
	}
	arrange();*/
    }

};

#endif
