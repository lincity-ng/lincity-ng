#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <string>

class Button;
class Paragraph;
class Component;

Button* getButton(Component& tree, const std::string& name);
Paragraph* getParagraph(Component& tree, const std::string& name);

#endif

