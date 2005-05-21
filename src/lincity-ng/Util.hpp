#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <string>

class Button;
class CheckButton;
class Paragraph;
class Component;
class SwitchComponent;

Button* getButton(Component& tree, const std::string& name);
CheckButton* getCheckButton(Component& tree, const std::string& name);
Paragraph* getParagraph(Component& tree, const std::string& name);
SwitchComponent* getSwitchComponent(Component& tree, const std::string& name);

#endif

