#ifndef __BUTTON_PANEL_HPP__
#define __BUTTON_PANEL_HPP__

#include "gui/Component.hpp"

#include <map>

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
  virtual bool opaque(const Vector2& pos) const;
 
 private:
  void attachButtons();
  std::string getAttribute(XmlReader &reader,const std::string &pName) const;
  void doButton(const std::string &button);
  void toggleMenu(std::string pName,bool enable);


  int module;
  bool alreadyAttached;
  
  std::vector<std::string> mMenuButtons;
  std::vector<std::string> mMenus;
  std::vector<std::string> mButtons;
  
  std::map<std::string,int> mMenuSelected;
};

ButtonPanel *getButtonPanel();

#endif
