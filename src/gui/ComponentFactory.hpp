#ifndef __COMPONENT_FACTORY_HPP__
#define __COMPONENT_FACTORY_HPP__

#include <map>
#include <string>
#include <memory>

class Component;
class XmlReader;

class Factory
{
public:
    virtual Component* createComponent(XmlReader& reader) = 0;
};

typedef std::map<std::string, Factory*> ComponentFactories;
extern ComponentFactories* component_factories;

/** comment from Matze:
 * Yes I know macros are evil, but in this specific case they save
 * A LOT of typing and evil code duplication.
 * I'll happily acceppt alternatives if someone can present me one that does
 * not involve typing 4 or more lines for each object class
 */
#define DECLARE_COMPONENT_FACTORY(CLASS)                                    \
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
  virtual Component* createComponent(XmlReader& reader)                     \
  {                                                                         \
      std::auto_ptr<CLASS> component (new CLASS());                         \
      component->parse(reader);                                             \
      return component.release();                                           \
  }                                                                         \
};
#define IMPLEMENT_COMPONENT_FACTORY(CLASS)                                  \
DECLARE_COMPONENT_FACTORY(CLASS)                                            \
INTERN_##CLASS##Factory factory_##CLASS;

#endif

