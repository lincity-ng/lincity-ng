#ifndef __CHECK_BUTTON_HPP__
#define __CHECK_BUTTON_HPP__

#include "gui/Button.hpp"

class XmlReader;

class CheckButton : public Button
{
  public:
    CheckButton(Component *pParent,XmlReader &reader);
    void event(const Event& event);
  protected:
    bool checked;    
};


#endif
