#ifndef __BUTTON_PANEL_HPP__
#define __BUTTON_PANEL_HPP__

#include "gui/Component.hpp"

class XmlReader;
class CheckButton;

class ButtonPanel:public Component
{
 public:
  ButtonPanel(Component *pWidget,XmlReader &reader);

  int getModule() const;
  
  virtual void draw(Painter &painter);
  void chooseButtonClicked(CheckButton* button,int);
  void menuButtonClicked(CheckButton* button,int);
 private:
  void attachButtons();
 
  int module;
  bool alreadyAttached;
};

ButtonPanel *getButtonPanel();

#endif
