#ifndef __COMPONENT_FACTORY_HPP__
#define __COMPONENT_FACTORY_HPP__

#include <map>
#include <string>

class Component;
class XmlReader;

class Factory
{
public:
    virtual Component* createComponent(Component* parent, XmlReader& reader) =0;
};

typedef std::map<std::string, Factory*> ComponentFactories;
extern ComponentFactories* component_factories;

Component* parseEmbeddedComponent(Component* parent, XmlReader& reader);
Component* createComponent(const std::string& type, Component* parent,
        XmlReader& reader);
Component* loadGUIFile(const std::string& filename);

/** comment from Matze:
 * Yes I know macros are evil, but in this specific case they save
 * A LOT of typing and evil code duplication.
 * I'll happily acceppt alternatives if someone can present me one that does
 * not involve typing 4 or more lines for each object class
 */
#define IMPLEMENT_COMPONENT_FACTORY(CLASS)                                  \
class INTERN_##CLASS##Factory : public Factory                              \
{                                                                           \
public:                                                                     \
  INTERN_##CLASS##Factory()                                                 \
  {                                                                         \
    if(component_factories == 0)                                            \
      component_factories = new ComponentFactories;                         \
                                                                            \
    component_factories->insert(std::make_pair(#CLASS, this));              \
  }                                                                         \
                                                                            \
  virtual Component* createComponent(Component* parent, XmlReader& reader)  \
  {                                                                         \
    return new CLASS(parent, reader);                                       \
  }                                                                         \
};                                                                          \
//INTERN_##CLASS##Factory factory_##CLASS;

#endif

